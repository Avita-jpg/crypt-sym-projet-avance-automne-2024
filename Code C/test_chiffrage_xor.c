#include "chiffrage_xor.h"  // Inclusion du fichier d'en-tête pour accéder aux fonctions XOR
#include <stdio.h>  // Pour l'affichage des résultats avec printf
#include <string.h> // Pour manipuler les chaînes de caractères (strlen, etc.)

// int taille_key(){ //renvoie la taille de la clé
//     //ouvrir le fichier de clés 
//     // lire une ligne du fichier while( c == '\n' || c == (fin du fichier))  on veut que une clé 
//     // c c'est le caractère courant
//     // on va compter à chaque caractère et incrémenter la variable taille 
// }


int main (void){

    // int taille = 0; //taille du message 

    printf("****************** Début des tests de chiffrage_xor ******************\n\n");
    printf("TEST 1 : (fichier cbc_adijd_accents-utf-8.txt)");
    
    /* Faire une fonction pour généraliser pour passer le ficher que l'on veut */

    // ouvrir un fichier open()
    // le parcourir tant qu'on est pas à la fin et incrémenter la variable taille (taille du message)
    
    // crypter le message à l'aide de la fonction et de la clé (il faut une clé)
    
    // remettre la taille à 0 pour le prochain fichier (mais si on est dans la fonction pas besoin car variable local dans la fonction)
    // fermer le fichier à la fin close()


    printf(" test fonction gen_key : \n");
    unsigned char * cle = gen_key(12);
    printf("La clé générée est : \n");
    for (int i = 0; i < 3; i++)
    {
        printf("%c",cle[i]);
    }
    printf("\n");
    

    return 0;
}