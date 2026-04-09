#ifndef CREME_H
#define CREME_H

#include <netinet/in.h>

#define PORT_BEUIP 9998
#define BEUIP_MAGIC "BEUIP" 
#define MAX 255 
#define BUF_SIZE 512 

typedef struct { 
    struct in_addr addr; 
    char pseudo[64]; 
} Reseau;

int preparer_socket_beuip(void);
void envoyer_paquet_beuip(int sock, struct sockaddr_in *dest, char code, char *p1, char *p2);

/* Note sur les codes de message :
 '0' : Déconnexion (Quitter le réseau)
 '1' : Broadcast d'identification (Annonce)
 '2' : Accusé de réception (Réponse à l'annonce)
 '3' : Commande "liste" (Local uniquement)
 '4' : Envoi de message à un pseudo (Local uniquement)
 '5' : Message à tout le monde (Local uniquement)
 '9' : Réception d'un message privé
 */

#endif