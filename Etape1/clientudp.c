#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 9999
#define BUF_SIZE 1024

void attendre_confirmation(int sock) {
    char buffer[BUF_SIZE];
    struct sockaddr_in from;
    socklen_t len = sizeof(from);
    
    int n = recvfrom(sock, buffer, BUF_SIZE, 0, (struct sockaddr *)&from, &len);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Confirmation serveur : %s\n", buffer);
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in target = {AF_INET, htons(PORT), {inet_addr("127.0.0.1")}};
    char *msg = "Test de connexion";

    sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&target, sizeof(target));
    attendre_confirmation(sock);

    close(sock);
    return 0;
}