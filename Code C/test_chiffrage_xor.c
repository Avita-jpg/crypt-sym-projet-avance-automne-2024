#include "chiffrage_xor.h"  // Inclusion du fichier d'en-tête pour accéder aux fonctions XOR
#include <stdio.h>  // Pour l'affichage des résultats avec printf
#include <string.h> // Pour manipuler les chaînes de caractères (strlen, etc.)
#define TAILLE_CLE 25


int main(void){

    printf("---------------- Tests masque jetable ---------------- \n\n ");
    //test sur des textes
    tests_masque_jetable("../script/CRYPT/tests/ref/msg2.txt");
    tests_masque_jetable("../script/CRYPT/tests/ref/bovary-isolatin1.txt");

    printf("Test sur un fichier avec des accents \n");
    tests_masque_jetable("../script/CRYPT/tests/ref/moyen.txt");

    tests_masque_jetable("../script/CRYPT/tests/ref/chinois-utf-8.rtf");

    //test sur une image
    tests_masque_jetable("../script/CRYPT/tests/ref/chaton.jpg");


    printf("---------------- Tests fonction XOR chiffrement/déchiffrement ---------------- \n\n ");

    // Clé qui va permettre le chiffrement
    char *key = "rutabaga";
    //char *key = "df9";
    //char *key = "adijd";
    //char *key = "DfklEa_";

    //test sur des textes
    test_chiffrage_xor("../script/CRYPT/tests/ref/msg2.txt", key);
    test_chiffrage_xor("../script/CRYPT/tests/ref/bovary-isolatin1.txt", key);
    test_chiffrage_xor("../script/CRYPT/tests/ref/moyen.txt", key);
    test_chiffrage_xor("../script/CRYPT/tests/ref/chinois-utf-8.rtf", key);

    //test sur une image
    test_chiffrage_xor("../script/CRYPT/tests/ref/chaton.jpg", key);
    

    return 0;
}