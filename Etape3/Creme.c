#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "creme.h"

int preparer_socket_beuip() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    int broadcast_perm = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast_perm, sizeof(broadcast_perm));
    struct sockaddr_in serv_addr = {AF_INET, htons(PORT_BEUIP), {INADDR_ANY}};
    bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    return sock;
}

void envoyer_paquet_beuip(int sock, struct sockaddr_in *dest, char code, char *p1, char *p2) {
    char buffer[BUF_SIZE];
    memset(buffer, 0, BUF_SIZE);
    buffer[0] = code;
    memcpy(buffer + 1, BEUIP_MAGIC, 5);
    if (p1) strcpy(buffer + 6, p1);
    if (p2) strcpy(buffer + 6 + strlen(p1) + 1, p2);
    sendto(sock, buffer, BUF_SIZE, 0, (struct sockaddr *)dest, sizeof(*dest));
}