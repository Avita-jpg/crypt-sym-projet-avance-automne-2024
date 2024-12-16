#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NOMBRE_CARACTERES 62

// Liste des caractères valides pour la clé pret au debut (plusieurs utilisations )
const char caracteres[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

int est_affichable(char c) {
    return (isalpha(c) || isdigit(c) || ispunct(c) || isspace(c));
}

// fonction pour obtenir les caractères valides pour un indice donné de la clé
int obtenir_caracteres_valides(char* msg_chiffre, int longueur_cle, int indice, char* clef_candidats) {
    int nb_caracteres_valides = 0;
    int msg_len = strlen(msg_chiffre);

    for (int i = 0; i < strlen(caracteres); i++) {
        char c_candidat = caracteres[i];
        int valide = 1;

        for (int j = indice; j < msg_len; j += longueur_cle) {
            char caractere_message = msg_chiffre[j];
            char decrypted_char = caractere_message ^ c_candidat;

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
    clef_candidats[nb_caracteres_valides] = '\0'; // Terminer la chaine pour pas rajouter 
    return nb_caracteres_valides;
}

char** generer_combinations(char** clef_candidats, int* len_candidats, int longueur_cle, unsigned long* nb_clefs_retour) {
    unsigned long nb_clefs = 1;
    for (int i = 0; i < longueur_cle; i++) {
        nb_clefs *= len_candidats[i];
    }

    *nb_clefs_retour = nb_clefs;
    char** clefs = malloc(nb_clefs * sizeof(char*));
    for (int i = 0; i < nb_clefs; i++) {
        clefs[i] = malloc((longueur_cle + 1) * sizeof(char));
    }

    int* indices_courants = calloc(longueur_cle, sizeof(int));

    for (int i = 0; i < nb_clefs; i++) {
        for (int j = 0; j < longueur_cle; j++) {
            clefs[i][j] = clef_candidats[j][indices_courants[j]];
        }
        clefs[i][longueur_cle] = '\0';

        for (int k = longueur_cle - 1; k >= 0; k--) {
            if (indices_courants[k] + 1 < len_candidats[k]) {
                indices_courants[k]++;
                break;
            } else {
                indices_courants[k] = 0;
            }
        }
    }

    free(indices_courants);
    return clefs;
}

// ffonction  pour cracker le message
char** break_code_c1(char* msg_chiffre, int longueur_cle, unsigned long * nb_clefs_retour) {
    // Allocation dynamique pour clef_candidats car finalstatique 
    char** clef_candidats = malloc(longueur_cle * sizeof(char*));
    for (int i = 0; i < longueur_cle; i++) {
        clef_candidats[i] = malloc((NOMBRE_CARACTERES + 1) * sizeof(char));
    }

    int* len_candidats = malloc(longueur_cle * sizeof(int));
    for (int i = 0; i < longueur_cle; i++) {
        len_candidats[i] = obtenir_caracteres_valides(msg_chiffre, longueur_cle, i, clef_candidats[i]);
        printf("Caractères valides pour clef[%d] : %s\n", i, clef_candidats[i]);
    }

  
    char** clefs = generer_combinations(clef_candidats, len_candidats, longueur_cle, nb_clefs_retour);

    // Libérer la mémoire des candidats
    for (int i = 0; i < longueur_cle; i++) {
        free(clef_candidats[i]);
    }
    free(clef_candidats);
    free(len_candidats);

    return clefs;
}
// unsigned char *read_content_c1(const char *filename) {
//     printf("Tentative d'ouverture du fichier : %s\n", filename); // Debug pour voir le nom du fichier
//     FILE* file = fopen(filename, "r");
//     if (file == NULL) {
//         perror("Erreur lors de l'ouverture d'un fichier");
//         exit(2);
//     }

//     printf("Fichier ouvert. \n");
//     // obtenir taille du fichier
//     if (fseek(file, 0, SEEK_END) != 0) {
//         perror("Erreur lors de fseek pour obtenir la taille");
//         fclose(file);
//         return NULL;
//     }
//     long size = ftell (file); //pointeur sur un long (aka la taille du fichier)
//     if (size < 0) {
//         perror("Erreur lors de ftell pour obtenir la taille");
//         fclose(file);
//         return NULL;
//     }

//     // revenir au debut du fichier
//     rewind(file);

//     // On alloue de l'espace pour le contenu du fichier (+1 pour le caractère de fin de chaîne)
//     unsigned char * content = (unsigned char*)malloc((size + 1) * sizeof(unsigned char));
//     if (content == NULL) {
//         perror("Erreur d'allocation mémoire pour le contenu du fichier");
//         fclose(file);
//         return NULL;
//     }

//     //Lecture du fichier
//     printf("Lecture du message\n");
//     if (fread(content, sizeof(char), size, file) != (size_t)size) {
//         perror("Erreur lors de la lecture du fichier de clé");
//         free(content);
//         fclose(file);
//         return NULL;
//     }
//     content[size] = '\0';
//     fclose(file);
//     printf("Fichier fermé\n");

//     return content;
// }

// int main() {
    
//     unsigned char* msg_chiffre=read_content("crypt.txt");
//     int longueur_cle;

//     printf("Entrez la longueur de la clé (1 à 10) : ");
//     scanf("%d", &longueur_cle);

//     if (longueur_cle < 1 || longueur_cle > 10) {
//         printf("Longueur invalide. Elle doit être entre 1 et 10.\n");
//         return 1;
//     }
//     //pour les autres crakcs prets 
//     int nb_clefs;
//     char** clefs_candidates = cracker_message((char *)msg_chiffre, longueur_cle, &nb_clefs);

//     printf("\nClés candidates générées (%d) :\n", nb_clefs);
//     for (int i = 0; i < nb_clefs; i++) {
//         printf("%s\n", clefs_candidates[i]);
//     }

//    //fini segmentation fault 
//     for (int i = 0; i < nb_clefs; i++) {
//         free(clefs_candidates[i]);
//     }
//     free(clefs_candidates);
//     free(msg_chiffre);

//     return 0;
// }