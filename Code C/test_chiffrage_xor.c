#include "chiffrage_xor.h"  // Inclusion du fichier d'en-tête pour accéder aux fonctions XOR
#include <stdio.h>  // Pour l'affichage des résultats avec printf
#include <string.h> // Pour manipuler les chaînes de caractères (strlen, etc.)
#define TAILLE_CLE 25


int main(void){
    printf("---------------- Test fonction XOR chiffrement/déchiffrement ---------------- \n\n ");

    // Clé qui va permettre le chiffrement
    char* key = "rutabaga";
    unsigned char *key_unsigned = (unsigned char *)key;
    unsigned char *message;
    long taille_fichier;

    // Ouverture du fichier à chiffrer
    FILE* fich = NULL; 

    // On ouvre le fichier avec le message que l'on doit chiffrer
    fich  = fopen("../script/CRYPT/tests/ref/msg2.txt", "rb");
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

    // On chiffre ensuite le message
    unsigned char * msg_chiffre = chiffrageXor(message, key_unsigned, taille_fichier);

    // On déchiffre le message
    unsigned char *  msg_clair = chiffrageXor(msg_chiffre, key_unsigned,taille_fichier); //on doit retomber sur le message de base

    // On 
    if(memcmp(message, msg_clair, taille_fichier) == 0){
        printf("Same content \n");
    }
    else{
        printf("Not the same content \n");
    }

    // On libère la mémoire et on ferme les fichiers
    // free(hex_contenu);
    // free(hex_result_final);
    free(message);
    free(msg_chiffre);
    free(msg_clair);
    fclose(fich);

    return 0;
}