#include "chiffrage_xor.h"  // Inclusion du fichier d'en-tête pour les définitions des fonctions
#include <stdio.h>  // Pour l'utilisation des fonctions d'entrée/sortie comme printf
#include <stdlib.h> // Pour les fonctions de gestion de la mémoire et utilitaires comme malloc, free
#include <string.h> // Pour manipuler les chaînes de caractères (strlen, etc.)
#include <time.h>

/**************************      Fonction génération d'une clé alphanumérique aléatoirement      **************************/

unsigned char* gen_key(int longueur){
    const char caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" ;
    unsigned char * new_key = (unsigned char *)malloc(longueur * sizeof(unsigned char)); // allocation de la clé

    srand(time(0));

    //on va créer la clé avec ses caractères choisis aléatoirement
    int index = (rand() % (sizeof(caracteres) - 1)); // On sélectionne un index aléatoire dans le tableau caracteres
    for (int i = 0; i < longueur; i++)
    {
        new_key[i] = (unsigned char)caracteres[index]; //on associe à notre clé le caractère généré
        index = (rand() % (sizeof(caracteres) - 1));
    }
    return new_key;
}


/**************************      Fonction chiffrage XOR      **************************/

// Fonction qui permet et le chiffrement et le déchiffrement d'un message 

unsigned char * chiffrageXor(unsigned char * msg, unsigned char * key, int msg_len){
    int key_len = strlen((char*)key);

    unsigned char *cipher = (unsigned char *)malloc(msg_len * sizeof(unsigned char)); //allocation pour le message crypté
    if (cipher == NULL) {
        perror("Erreur malloc\n");
        exit(1);  // Code de retour d'erreur
    }

    for (int i=0, j=0; i < msg_len; i++){
        cipher[i] = msg[i] ^ key[j];
        j = (j+1) % key_len;
    }
    return cipher;
}

void test_chiffrage_xor(char *fich_name, char *key){

    printf("le fichier testé est :  %s \n", fich_name);

    // On transforme la clé en unsigned char
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

