#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <arpa/inet.h>
#include "creme.h"

#define PORT_BEUIP 9998
#define NBMAXC 15
#define HIST_FILE ".biceps_history"

static char *Mots[100];
static int NMots;
static pid_t pid_serveur = 0;

typedef struct { 
    char *nom; 
    int (*fonction)(int, char **); 
} ComInt; 

static ComInt TabCom[NBMAXC];
static int NbCom = 0;

void ajouteCom(char *n, int (*f)(int, char**)) {
    if (NbCom < NBMAXC) {
        TabCom[NbCom].nom = n;
        TabCom[NbCom++].fonction = f;
    }
}

int Sortie(int n, char **p) {
    exit(0); 
}
int ChangeDir(int n, char **p) { 
    if(n>1) chdir(p[1]); 
    return 1; 
}
int PrintDir(int n, char **p) { 
    char b[1024]; 
    getcwd(b, 1024); 
    printf("%s\n", b); 
    return 1; 
}
int Version(int n, char **p) { 
    printf("biceps version 1.0\n"); 
    return 1; 
}

int cat(int n, char **p) {
    if (n < 2) {
        fprintf(stderr, "Usage: cat <filename>\n");
        return 1;
    }
    FILE *f = fopen(p[1], "r");
    if (!f) {
        perror("cat");
        return 1;
    }
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), f)) {
        printf("%s", buffer);
    }
    fclose(f);
    printf("\n");
    return 1;
}
int ps(int n, char **p) {
    system("ps aux");
    return 1;
}
int ls(int n, char **p) {
    system("ls -l");
    return 1;
}
int echo(int n, char **p) {
    for (int i = 1; i < n; i++) {
        printf("%s ", p[i]);
    }
    printf("\n");
    return 1;
}
int lancer_serveur_beuip(int n, char **p) {
    if (n < 2) {
        fprintf(stderr, "Usage: Start <pseudo>\n");
        return 1;
    }
    pid_serveur = fork();
    if (pid_serveur == 0) {
        execl("./Servudp", "Servudp", p[1], NULL);
        perror("Erreur execl");
        exit(1);
    }
    printf("Serveur BEUIP lancé (PID : %d)\n", pid_serveur);
    return 1;
}

int stop_serveur_beuip(int n, char **p) {
    if (pid_serveur > 0) {
        kill(pid_serveur, SIGINT);
        printf("Signal d'arrêt envoyé au serveur.\n");
        pid_serveur = 0;
    } else {
        printf("Aucun serveur en cours d'exécution.\n");
    }
    return 1;
}

int cmd_mess(int n, char **p) {
    if (n < 2) return printf("Usage: mess <list|all|to> [args]\n"), 1;
    
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv = {AF_INET, htons(PORT_BEUIP), {inet_addr("127.0.0.1")}};
    
    if (strcmp(p[1], "list") == 0) {
        envoyer_paquet_beuip(sock, &serv, '3', NULL, NULL); 
    } 
    else if (strcmp(p[1], "all") == 0 && n > 2) {
        envoyer_paquet_beuip(sock, &serv, '5', p[2], NULL); 
    } 
    else if (strcmp(p[1], "to") == 0 && n > 3) {
        envoyer_paquet_beuip(sock, &serv, '4', p[2], p[3]); 
    }
    printf("Commande mess envoyée au serveur local\n");
    
    close(sock);
    return 1;
}
void listeComInt() {
    printf("Commandes internes :\n");
    for(int i=0; i<NbCom; i++) {
        printf("- %s\n", TabCom[i].nom);
    }
}

int help(int n, char **p) {
    listeComInt();
    return 1;
}

void majComInt() {
    ajouteCom("exit", Sortie);
    ajouteCom("cd", ChangeDir);
    ajouteCom("pwd", PrintDir);
    ajouteCom("vers", Version);
    ajouteCom("cat", cat);
    ajouteCom("ps", ps);
    ajouteCom("ls", ls);
    ajouteCom("echo", echo);
    ajouteCom("Start", lancer_serveur_beuip);
    ajouteCom("Stop", stop_serveur_beuip);
    ajouteCom("mess", cmd_mess);
    ajouteCom("help", help);
}



int execComInt(int n, char **p) {

    for(int i=0; i<NbCom; i++) {
        if(strcmp(p[0], TabCom[i].nom) == 0) {
            TabCom[i].fonction(n, p); 
            return 1; 
        }
    }
    return 0;
}

void execComExt(char **p) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(p[0], p);
        perror("biceps"); 
        exit(1); 
    } else {
        waitpid(pid, NULL, 0);
    }
}

char *copyString(char *s) {
    char *c = malloc(strlen(s)+1);
    return strcpy(c, s);
}

int analyseCom(char *b) {
    char *token;
    char *rest = b;
    NMots = 0;
    token = strtok(rest, " \t\n\r");
    while (token != NULL && NMots < 99) {
        Mots[NMots++] = copyString(token);
        token = strtok(NULL, " \t\n\r");
    }
    Mots[NMots] = NULL;
    return NMots;
}

int main() {
    majComInt();
    char hostname[256];
    gethostname(hostname, 256);
    char *prompt = malloc(512);
    if (prompt == NULL) exit(1);
    sprintf(prompt, "%s@%s$ ", getenv("USER"), hostname);
    signal(SIGINT, SIG_IGN);
    read_history(HIST_FILE);
    char *ligne, *segment, *ptr;
    while ((ligne = readline(prompt)) != NULL) {

        if (ligne && *ligne != '\0') {
            add_history(ligne);
            append_history(1, HIST_FILE);
        }

        ptr = ligne;
         while ((segment = strsep(&ptr, ";")) != NULL) {
            int n = analyseCom(segment);
            if (n > 0) {
                if (!execComInt(n, Mots)) {
                    execComExt(Mots);
                }
                for(int i=0; i<n; i++) free(Mots[i]);
            }
        }
        free(ligne);
    }
    printf("Signal EOF reçu fermeture du programme\n");
    write_history(HIST_FILE);
    return 0;
}