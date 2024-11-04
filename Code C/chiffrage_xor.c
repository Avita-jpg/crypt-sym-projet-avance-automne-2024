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

