#include "chiffrage_xor.h"  // Inclusion du fichier d'en-tête pour les définitions des fonctions
#include <stdio.h>  // Pour l'utilisation des fonctions d'entrée/sortie comme printf
#include <stdlib.h> // Pour les fonctions de gestion de la mémoire et utilitaires comme malloc, free
#include <string.h> // Pour manipuler les chaînes de caractères (strlen, etc.)
#include <time.h>

/**************************      Fonction incrémentation (selon un modulo)      **************************/
int inc(int nombre, int nbMod){
    return (nombre+1) % nbMod;
}

/**************************      Fonction génération d'une clé alphanumérique aléatoirement      **************************/

unsigned char* gen_key(int longueur){
    const char caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" ;
    unsigned char * new_key = (unsigned char *)malloc(longueur * sizeof(unsigned char)); // allocation de la clé

    //on va créer la clé avec ses caractères choisis aléatoirement
    int index = (rand() % (sizeof(caracteres) - 1)); // On sélectionne un index aléatoire dans le tableau caracteres
    for (int i = 0; i < longueur; i++)
    {
        new_key[i] = (unsigned char)caracteres[index]; //on associe à notre clé le caractère généré 
    }    
    return new_key;   
}

/**************************      Fonction chiffrage XOR      **************************/

// Fonction qui permet et le chiffrement et le déchiffrement d'un message 

unsigned char * chiffrageXor(unsigned char * msg, unsigned char * key){
    int taille_msg = strlen((char*)msg); //on cast le msg en char* pour lire sa taille
    int taille_key = strlen((char*)key);
    unsigned char * msg_chiffre = (unsigned char *)malloc(taille_msg * sizeof(unsigned char)); //allocation pour le message crypté
    if (msg_chiffre == NULL) {
        perror("Erreur malloc\n");
        exit(1);  // Code de retour d'erreur
    }

    for (int i=0, j=0; i < taille_msg; i++){
        msg_chiffre[i] = msg[i] ^ key[j];
        j = inc(j, taille_key);
    }
    return msg_chiffre;
}

