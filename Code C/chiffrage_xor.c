#include "chiffrage_xor.h"  // Inclusion du fichier d'en-tête pour les définitions des fonctions
#include <stdio.h>  // Pour l'utilisation des fonctions d'entrée/sortie comme printf
#include <stdlib.h> // Pour les fonctions de gestion de la mémoire et utilitaires comme malloc, free
#include <string.h> // Pour manipuler les chaînes de caractères (strlen, etc.)

int inc(int nombre, int nbMod){
    return (nombre+1) % nbMod;
}


char * chiffrageXor(unsigned char * msg, unsigned char * key){
    int taille = strlen(msg);
    unsigned char msg_chiffre[taille] ;

    for (int i=0, j=0; i < taille; i++){
        msg_chiffre[i] = msg[i] ^ key[j];
        j = inc(j, strlen(key));
    }
    return msg_chiffre;
}