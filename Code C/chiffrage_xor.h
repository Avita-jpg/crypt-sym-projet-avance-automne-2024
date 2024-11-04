// chiffrage_xor.h
#ifndef CHIFFRAGE_XOR_H
#define CHIFFRAGE_XOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Fonction qui incrémente à l'aide d'un modulo nbMod une variable nombre
 */
int inc(int nombre, int nbMod);


/*
 * Fonction qui génère une clé alphanumérique aléatoirement en fonction d'une longueur 
 */
unsigned char * gen_key(int longueur);

/*
 * Fonction qui permet de chiffrer et de déchiffrer un message caractère par caractère à l'aide d'un XOR
 */
unsigned char * chiffrageXor(unsigned char * msg, unsigned char * key, int taille_msg);

/*
 * Fonction de tests pour la fonction de chiffrage xor
 */
void test_chiffrage_xor(char *fich_name, char *key);


/*
 * Fonction qui permet de chiffrer et de déchiffrer un message caractère par caractère à l'aide d'un XOR
    Utilise la fonctions chiffrageXor 
 */
unsigned char * masque_jetable(unsigned char * msg, unsigned char * key, int taille_msg);


/*
 * Fonction de tests pour les masques jetables
 */
void tests_masque_jetable(char *fich_name);


#endif // CHIFFRAGE_XOR_H