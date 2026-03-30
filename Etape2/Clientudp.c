#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "creme.h"

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in local_serv = {AF_INET, htons(PORT_BEUIP), {inet_addr("127.0.0.1")}};
    char buffer[BUF_SIZE] = {0};
    memcpy(buffer + 1, BEUIP_MAGIC, 5);

    if (strcmp(argv[1], "liste") == 0) {
        buffer[0] = '3'; 
    } else if (strcmp(argv[1], "mess") == 0 && argc == 4) {
        buffer[0] = '4'; 
        int p_len = snprintf(buffer + 6, 64, "%s", argv[2]); // Pseudo cible
        strncpy(buffer + 6 + p_len + 1, argv[3], BUF_SIZE - (7 + p_len)); // Message
    } else if (strcmp(argv[1], "all") == 0 && argc == 3) {
        buffer[0] = '5'; 
        strncpy(buffer + 6, argv[2], BUF_SIZE - 7); 
    }

    sendto(sock, buffer, BUF_SIZE, 0, (struct sockaddr *)&local_serv, sizeof(local_serv));
    close(sock);
    return 0;
}