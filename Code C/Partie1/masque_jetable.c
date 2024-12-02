#include "chiffrage_xor.h"  // Inclusion du fichier d'en-tête pour les définitions des fonctions
#include <stdio.h>  // Pour l'utilisation des fonctions d'entrée/sortie comme printf
#include <stdlib.h> // Pour les fonctions de gestion de la mémoire et utilitaires comme malloc, free
#include <string.h> // Pour manipuler les chaînes de caractères (strlen, etc.)
#include <time.h>

/**************************      Fonction masque jetable      **************************/

unsigned char * masque_jetable(unsigned char * msg, unsigned char * key, int taille_msg){
    unsigned char * cipher = chiffrageXor(msg,key,taille_msg); //on chiffre le message avec la clé qui a préalablement été générée avant
    return cipher;
}

//fonction de test sur le masque jetable
void tests_masque_jetable(char *fich_name){

    printf("le fichier testé est :  %s \n", fich_name);

    unsigned char *message;
    long taille_fichier;

    // Ouverture du fichier à chiffrer
    FILE* fich = NULL; 

    // On ouvre le fichier avec le message que l'on doit chiffrer
    fich  = fopen(fich_name, "rb");

    if (fich == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(1);
    }

    //On prends la taille du message dans le fichier
    fseek(fich, 0, SEEK_END);
    taille_fichier = ftell(fich);  // Obtenir la taille du fichier
    rewind(fich);  // Remettre le curseur au début du fichier

    // Allouer suffisamment de mémoire pour stocker tout le fichier
    message = (unsigned char*) malloc(sizeof(unsigned char) * taille_fichier + 1);
    if (message == NULL) {
        perror("Erreur allocation mémoire");
        fclose(fich);
        exit(1);
    }

    // Lecture dans le fichier et on met ça dans une variable unsigned char*
    if(fread(message, 1, taille_fichier, fich) != (size_t)taille_fichier){
        perror("Erreur lecture fichier : ");
        free(message);
        fclose(fich);
        exit(1);
    }

    message[taille_fichier] = '\0'; 

    // On crée une clé générée aléatoirement (et de taille égale ou supérieure au message)
    unsigned char *key = gen_key(taille_fichier+1); 

    // On chiffre ensuite le message
    unsigned char * msg_chiffre = masque_jetable(message, key, taille_fichier);

    // On déchiffre le message
    unsigned char *  msg_clair = masque_jetable(msg_chiffre, key,taille_fichier); //on doit retomber sur le message de base

    // On compare les 2 messages
    if(memcmp(message, msg_clair, taille_fichier) == 0){
        printf("Same content \n");
        printf(" OK \n");
    }
    else{
        printf("Not the same content \n");
        printf(" KO \n");
    }

    printf("---------------------\n\n");

    // On libère la mémoire et on ferme les fichiers
    free(message);
    free(msg_chiffre);
    free(msg_clair);
    fclose(fich);
}