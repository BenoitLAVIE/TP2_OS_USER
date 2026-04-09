#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include "creme.h"

Reseau table[MAX];
int nb_personne = 0;

// Ajout à la table
void ajouter_utilisateur(struct sockaddr_in *addr, char *pseudo) {
    for (int i = 0; i < nb_personne; i++) {
        if (table[i].addr.s_addr == addr->sin_addr.s_addr) return;
    }
    if (nb_personne < MAX) {
        table[nb_personne].addr = addr->sin_addr;
        strncpy(table[nb_personne].pseudo, pseudo, 63);
        nb_personne++;
        printf("Nouveau pair : %s (%s)\n", pseudo, inet_ntoa(addr->sin_addr));
    }
}

// Envoi de message formaté 
void envoyer_beuip(int sock, struct sockaddr_in *dest, char code, char *pseudo) {
    char buffer[BUF_SIZE] = {code};
    memcpy(buffer + 1, BEUIP_MAGIC, 5);
    strncpy(buffer + 6, pseudo, BUF_SIZE - 7);
    sendto(sock, buffer, BUF_SIZE, 0, (struct sockaddr *)dest, sizeof(*dest));
}

// Configuration du socket broadcast 
int initialiser_socket() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    int broadcast_perm = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast_perm, sizeof(broadcast_perm));
    struct sockaddr_in serv_addr = {AF_INET, htons(PORT_BEUIP), {INADDR_ANY}};
    bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    return sock;
}

// Affiche la table (Code 3)
void lister_utilisateurs() {
    printf(" Liste des connectés %d\n", nb_personne);
    for (int i = 0; i < nb_personne; i++) {
        printf("%d: %s [%s]\n", i+1, table[i].pseudo, inet_ntoa(table[i].addr));
    }
}

// Relai vers un pseudo (Code 4 - 9) 
void relayer_message(int sock, char *payload) {
    char *target_pseudo = payload;
    char *msg_content = payload + strlen(target_pseudo) + 1;
    for (int i = 0; i < nb_personne; i++) {
        if (strcmp(table[i].pseudo, target_pseudo) == 0) {
            struct sockaddr_in dest = {AF_INET, htons(PORT_BEUIP), table[i].addr};
            envoyer_beuip(sock, &dest, '9', msg_content);
            return;
        }
    }
    printf("Pseudo '%s' introuvable.\n", target_pseudo);
}

// Envoi à tous (Code 5) 
void diffuser_global(int sock, char *msg) {
    for (int i = 0; i < nb_personne; i++) {
        struct sockaddr_in dest = {AF_INET, htons(PORT_BEUIP), table[i].addr};
        envoyer_beuip(sock, &dest, '9', msg);
    }
}

char* chercher_pseudo(struct in_addr addr) {
    for (int i = 0; i < nb_personne; i++) {
        if (table[i].addr.s_addr == addr.s_addr) return table[i].pseudo;
    }
    return "Inconnu"; // Si l'IP n'est pas dans la table
}


// Fonction de traitement 
void traiter_paquet(int sock, char *buf, struct sockaddr_in *src, char *mon_pseudo) {
    if (memcmp(buf + 1, BEUIP_MAGIC, 5) != 0) return;

    int est_local = (src->sin_addr.s_addr == inet_addr("127.0.0.1"));

    if (buf[0] == '1') { // Inscription distante
        ajouter_utilisateur(src, buf + 6);
        envoyer_beuip(sock, src, '2', mon_pseudo);
    } else if (buf[0] == '2') { // Accusé réception distant
        ajouter_utilisateur(src, buf + 6);
    } else if (est_local) { // Commandes de pilotage locales 
        if (buf[0] == '3') lister_utilisateurs();
        else if (buf[0] == '4') relayer_message(sock, buf + 6);
        else if (buf[0] == '5') diffuser_global(sock, buf + 6);
        char *ok = "OK";
        sendto(sock, ok, strlen(ok), 0, (struct sockaddr *)src, sizeof(*src));
    } else if (buf[0] == '9') { // Message reçu d'un pair 
        char *expediteur = chercher_pseudo(src->sin_addr); // Trouve le nom via l'IP 
        printf("Message de %s : %s\n", expediteur, buf + 6);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;
    int sock = initialiser_socket(); // Contient socket(), setsockopt(), bind()
    struct sockaddr_in bcast = {AF_INET, htons(PORT_BEUIP), {inet_addr("192.168.88.255")}};
    
    // Annonce initiale
    envoyer_beuip(sock, &bcast, '1', argv[1]);

    while (1) {
        char buf[BUF_SIZE];
        struct sockaddr_in src;
        socklen_t len = sizeof(src);
        memset(buf, 0, BUF_SIZE);
        int n = recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&src, &len);
        if (n > 0) {
            traiter_paquet(sock, buf, &src, argv[1]);
        }
    }
    return 0;
}