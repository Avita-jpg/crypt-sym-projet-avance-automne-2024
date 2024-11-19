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

int obtenir_caracteres_valides(char* msg_chiffre, int longueur_cle, int indice, char* clef_candidats) {
    const char caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int nb_caracteres_valides = 0;
//soucis ici c que dans aucune des cle il met des chiffres que des lettre
    // parcourir caracteres candidats
    for (int i = 0; i < strlen(caracteres); i++) {
        char c_candidat = caracteres[i];
        int valide = 1;

        // debug
        printf("\t caractere étudié: %c\n", c_candidat);
        // end debug

        // parcourir msg en faisants des sauts de longueur =  longueur_cle
        for (int j = indice; j < strlen(msg_chiffre); j += longueur_cle) {
            char caractere_message = msg_chiffre[j];
            char decrypted_char = caractere_message ^ c_candidat;

            // debug
            int res = est_affichable(decrypted_char);
            printf("\t\t message[%d] = %c", j, msg_chiffre[j]);
            if (res) {
                printf(", %c xor %c = %c \n", msg_chiffre[j], c_candidat, decrypted_char);
            }
            else {
                printf(", %c xor %c (= %c) non affichable\n", msg_chiffre[j], c_candidat, decrypted_char);
            }
            // end debug

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
    return nb_caracteres_valides;
}

void cracker_message(char* msg_chiffre, int l_cle) {
    // tableau contenant les caracteres candidats pour chaque caractere de la clef
    char clef_candidats[l_cle][NOMBRE_CARACTERES + 1];
    // tableau contenant le nb de candidats par caractere de la clef
    int len_candidats[l_cle];

    int nb_clefs = 1;
   
    // avoir candidats pour chaque caractere de la clef
    for (int i = 0; i < l_cle; i++) {
        // printf("Calcul candidats de clef[%d]:\n ", i);
        len_candidats[i] = obtenir_caracteres_valides(msg_chiffre, l_cle, i, clef_candidats[i]);
        printf("Caractères valides pour clef[%d] : %s\n", i, clef_candidats[i]);

        // calcul du nombre total de clefs candidates
        nb_clefs = nb_clefs * len_candidats[i];
    }

    // calcul produit cartesien
    char clefs[nb_clefs][l_cle];

    int indice_courant [l_cle]; // index courant dans chaque tableau de caracteres candidats
    for (int i = 0; i < l_cle; i++) {
        indice_courant[i] = 0;
    }

    // int nb_clefs = 6;
    // char clefs[nb_clefs][len_clef + 1];

    for (int i = 0; i < nb_clefs; i++) {
        for (int j = 0; j < l_cle; j++) {
            clefs[i][j] = clef_candidats[j][indice_courant[j]];
        }
        clefs[i][l_cle] = '\0';
        printf("%s ", clefs[i]);

        // mise à jour des indices_courants
        // on commence à la fin
        for (int k = l_cle - 1; k >= 0; k--) {
            // (un peu comme if hasnext() then next())
            if (indice_courant[k] + 1 < len_candidats[k]) {
                indice_courant[k]++;
            } else {
                // sinon réinitialiser
                indice_courant[k] = 0;
            }
        }
        printf("\n");
    }

    // end calcul produit cartesien


//     printf("Essai de toutes les combinaisons possibles :\n");
//     for (int i = 0; i < strlen(clef_candidats[0]); i++) {
//         for (int j = 0; j < strlen(clef_candidats[1]); j++) {
//         for (int k = 0; k < strlen(clef_candidats[2]); k++) {
//             char tentative_clef[l_cle + 1];
//                 tentative_clef[0] = clef_candidats[0][i];
//                 tentative_clef[1] = clef_candidats[1][j];
//                 tentative_clef[2] = clef_candidats[2][k];
//                 tentative_clef[l_cle] = '\0';
//
//
//                 int valide = 1;
//                 for (int l = 0; l < strlen(msg_chiffre); l++) {
//                     char decrypted_char = msg_chiffre[l] ^ tentative_clef[l % l_cle];
//                     if (!est_affichable(decrypted_char)) {
//                         valide = 0;
//                         break;
//                     }
//                 }
//
//
//                 if (valide) {
//                     // printf("Clé candidate valide : %s\n", tentative_clef);
//                 }
//             }
//         }
//     }
}

int main() {
    char msg_chiffre[] = "s(/1& !";
    int l_cle = 3;

    cracker_message(msg_chiffre, l_cle);

    return 0;
}
