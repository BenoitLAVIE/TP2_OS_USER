#ifndef CREME_H
#define CREME_H

#include <netinet/in.h>

/* Configuration du protocole BEUIP */
#define PORT_BEUIP 9998 // Port libre défini dans le sujet
#define BEUIP_MAGIC "BEUIP" // Signature de contrôle du protocole
#define MAX_PEERS 255 // Capacité maximale de la table
#define BUF_SIZE 512 // Taille tampon pour les messages

/* Structure pour stocker un couple Utilisateur */
typedef struct {
    struct in_addr addr; // Adresse IP de l'étudiant
    char pseudo[64]; // Pseudo choisi par l'utilisateur
} Peer;

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