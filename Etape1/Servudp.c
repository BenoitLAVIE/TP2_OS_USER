#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 9999
#define BUF_SIZE 1024

void envoyer_ack(int sock, struct sockaddr_in *client_addr) {
    char *ack_msg = "Bien reçu 5/5 !";
    socklen_t addr_len = sizeof(*client_addr);
    
    // Utilisation de MSG_CONFIRM comme demandé au point 19
    sendto(sock, ack_msg, strlen(ack_msg), MSG_CONFIRM, 
           (struct sockaddr *)client_addr, addr_len);
}

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv_addr = {AF_INET, htons(PORT), {INADDR_ANY}};
    char buffer[BUF_SIZE];

    bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    while (1) {
        struct sockaddr_in cli_addr;
        socklen_t len = sizeof(cli_addr);
        int n = recvfrom(sock, buffer, BUF_SIZE, 0, (struct sockaddr *)&cli_addr, &len);
        
        if (n > 0) {
            buffer[n] = '\0';
            printf("Message reçu : %s\n", buffer);
            envoyer_ack(sock, &cli_addr);
        }
    }
    return 0;
}