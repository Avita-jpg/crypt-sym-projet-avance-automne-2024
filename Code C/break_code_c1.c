#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LONGUEUR_MAX_CLE 10
#define NOMBRE_CARACTERES 62

//ppour respecter le format de la cle
int est_affichable(char c) {
    return (isalpha(c) || isdigit(c) || ispunct(c) || isspace(c));
}

//ce n'est pas sur si je peux utiliser la genkey de eve car c la meme idee mais ca ne marche pas compeltement ici 
/*unsigned char* gen_key(int longueur){
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
}*/



void obtenir_caracteres_valides(char* msg_chiffre, int longueur_cle, int indice, char* clef_candidats) {
    const char caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; 
    int nb_caracteres_valides = 0;
//soucis ici c que dans aucune des cle il met des chiffres que des lettre 
    
for (int i = 0; i < strlen(caracteres); i++) {
        char c_candidat = caracteres[i];
        int valide = 1;

       
        for (int j = indice; j < strlen(msg_chiffre); j += longueur_cle) {
            char caractere_message = msg_chiffre[j];
            char decrypted_char = caractere_message ^ c_candidat;

            //inclus
            if (!est_affichable(decrypted_char)) {
                valide = 0;
                break;
            }
        }

    
        if (valide) {
            clef_candidats[nb_caracteres_valides] = c_candidat;
            nb_caracteres_valides++;
        }
    }
    clef_candidats[nb_caracteres_valides] = '\0'; 
}

void cracker_message(char* msg_chiffre, int l_cle) {
    char clef_candidats[l_cle][NOMBRE_CARACTERES + 1];

   
    for (int i = 0; i < l_cle; i++) {
        obtenir_caracteres_valides(msg_chiffre, l_cle, i, clef_candidats[i]);
        printf("Caractères valides pour clef[%d] : %s\n", i, clef_candidats[i]);
    }

    
    printf("Essai de toutes les combinaisons possibles :\n");
    for (int i = 0; i < strlen(clef_candidats[0]); i++) {
        for (int j = 0; j < strlen(clef_candidats[1]); j++) {
        for (int k = 0; k < strlen(clef_candidats[2]); k++) {
            char tentative_clef[l_cle + 1];
                tentative_clef[0] = clef_candidats[0][i];
                tentative_clef[1] = clef_candidats[1][j];
                tentative_clef[2] = clef_candidats[2][k];
                tentative_clef[l_cle] = '\0';

                
                int valide = 1;
                for (int l = 0; l < strlen(msg_chiffre); l++) {
                    char decrypted_char = msg_chiffre[l] ^ tentative_clef[l % l_cle];
                    if (!est_affichable(decrypted_char)) {
                        valide = 0;
                        break;
                    }
                }

                
                if (valide) {
                    printf("Clé candidate valide : %s\n", tentative_clef);
                }
            }
        }
    }
}

int main() {
    char msg_chiffre[] = "s(/1& !";
    int l_cle = 3;

    cracker_message(msg_chiffre, l_cle);

    return 0;
}
