#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "chiffrage_xor.h"
#include "cbc.h"

void print_utilisation() {
    printf("Usage: sym_crypt -i <input_file> -o <output_file> -k <key> -m <method> [-v <iv_file>] [-l <log_file>] [-h]\n");
    printf("Options:\n");
    printf("  -i <input_file>      : Nom du fichier contenant le message clair à chiffrer\n");
    printf("  -o <output_file>     : Nom du fichier où sera écrit le message chiffré\n");
    printf("  -k <key>             : Clé de chiffrement\n");
    printf("  -f <key_file>        : Nom du fichier contenant la clé\n");
    printf("  -m <method>          : Méthode de chiffrement/déchiffrement (xor, cbc-crypt, cbc-uncrypt, mask)\n");
    printf("  -v <iv_file>         : Nom du fichier contenant le vecteur d'initialisation (pour CBC)\n");
    printf("  -l <log_file>        : Nom du fichier de log (facultatif)\n");
    printf("  -h                   : Affiche l'aide\n");
}

unsigned char * read_content(const char * filename) {
    printf("Tentative d'ouverture du fichier : %s\n", filename); // Debug pour voir le nom du fichier
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture d'un fichier");
        exit(2);
    }

    printf("Fichier ouvert. \n");
    // obtenir taille du fichier
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("Erreur lors de fseek pour obtenir la taille");
        fclose(file);
        return NULL;
    }
    long size = ftell (file);
    if (size < 0) {
        perror("Erreur lors de ftell pour obtenir la taille");
        fclose(file);
        return NULL;
    }
    // revenir au debut du fichier
    rewind(file);

    // On alloue de l'espace pour le contenu du fichier (+1 pour le caractère de fin de chaîne)
    unsigned char * content = (unsigned char*)malloc((size + 1) * sizeof(unsigned char));
    if (content == NULL) {
        perror("Erreur d'allocation mémoire pour le contenu du fichier");
        fclose(file);
        return NULL;
    }

    //Lecture du fichier
    printf("Lecture du message\n");
    if (fread(content, sizeof(char), size, file) != (size_t)size) {
        perror("Erreur lors de la lecture du fichier de clé");
        free(content);
        fclose(file);
        return NULL;
    }
    content[size] = '\0';
    fclose(file);

    return content;
}

int main(int argc, char *argv[]) {
    char *input_file = NULL;
    char *output_file = NULL;
    unsigned char *key = NULL;
    char *key_file = NULL;
    char *method = NULL;
    char *vini_file = NULL;
    /*char *log_file = NULL;*/
    int opt;

    if (argc < 9 || argc > 14) {
        print_utilisation();
        exit(1);
    }

    // Analyse des options en ligne de commande
    while ((opt = getopt(argc, argv, "i:o:k:f:m:v:l:h")) != -1) {
        switch (opt) {
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'k':
                key = (unsigned char *)optarg;
                break;
            case 'f':
                key_file = optarg;
                break;
            case 'm':
                method = optarg;
                break;
            case 'v':
                vini_file = optarg;
                break;
            case 'l':
                // log_file = optarg;
                break;
            case 'h':
                printf("case h \n");
                print_utilisation();
                exit(0);
            default:
                printf("default \n");
                print_utilisation();
                exit(1);
        }
    }

    // Vérification des options obligatoires
    // if (!input_file || !output_file || !method || (!key && !key_file)) {
    //     fprintf(stderr, "Erreur : Les options -i, -o, -k et -m sont obligatoires.\n");
    //     print_utilisation();
    //     exit(3);
    // }

    // Charger le fichier de clé si nécessaire
    if (key_file) {
        key = read_content(key_file);
        if (key == NULL) {
            exit(2);
        }
    }

    unsigned char * vini;
    // charger vecteur d'initialisation si nécessaire'
    if (vini_file && (strcmp(method, "cbc-crypt") == 0 || strcmp(method, "cbc-uncrypt") == 0)) {
        vini = read_content(vini_file);
        if (vini == NULL) {
            free(key);
            exit(2);
        }
    } else if ((strcmp(method, "cbc-crypt") == 0 || strcmp(method, "cbc-uncrypt") == 0) && (!vini_file)) {
        fprintf(stderr, "Erreur : L'option -v est obligatoire si la méthode est cbc-crypt ou cbc_uncrypt.\n");
        print_utilisation();
        free(key);
        exit(3);
    }

    // charger message
    unsigned char * message = read_content(input_file);
    if (message == NULL) {
        free(key);
        free(vini);
        exit(2);
    }

    // Cas de la fonction xor
    if (strcmp(method, "xor") == 0) {
        // ouvrir fichier output
        FILE* output = fopen(output_file, "w");
        if (output == NULL) {
            perror("Erreur lors de l'ouverture d'un fichier");
            free(key);
            free(vini);
            free(message);
            exit(2);
        }
        // appel à chiffrage_xor
        unsigned char * output_buffer = chiffrageXor(message, key, strlen((char*)message));
        // écriture
        fwrite(output_buffer, strlen((char*)output_buffer),1,output);
        // liberation ressources et fermeture fichiers
        free(output_buffer);
        fclose(output);
    } 
    
    
    // Cas de la fonction cbc-crypt
    else if (strcmp(method, "cbc-crypt") == 0) {
        cbc_crypt(input_file, output_file, key, vini);
    } 
    
    // Cas de la fonction cbc-uncrypt
    else if (strcmp(method, "cbc-uncrypt") == 0) {
        cbc_uncrypt(input_file, output_file, key, vini);
    } 
    
    // Cas de la fonction mask
    else if (strcmp(method, "mask") == 0) {
        // ouvrir fichier output
        FILE* output = fopen(output_file, "w");
        if (output == NULL) {
            perror("Erreur lors de l'ouverture d'un fichier");
            free(key);
            free(vini);
            free(message);
            exit(2);
        }
        // Taille message 
        int taille = strlen((char *) message);
        unsigned char *key = NULL;

        // Vérifier si on est en mode chiffrement ou déchiffrement
        FILE *key_file = fopen("key.temp", "r");
        if (key_file != NULL) {
            // Mode déchiffrement : lire la clé depuis "key.temp"
            key = read_content("key.temp");
            fclose(key_file);

            if (key == NULL) {
                perror("Erreur lors de la lecture de la clé temporaire");
                free(message);
                fclose(output);
                exit(2);
            }
        } else {
            // Création de la clé aléatoire
            key = gen_key(taille + 1);
            if (key == NULL) {
                perror("Erreur lors de la génération de la clé aléatoire");
                free(message);
                fclose(output);
                exit(2);
            }

            // Sauvegarde de la clé dans un fichier temporaire pour le déchiffrement
            FILE *key_file = fopen("key.temp", "w");
            if (key_file == NULL) {
                perror("Erreur lors de la sauvegarde de la clé temporaire");
                free(key);
                free(message);
                fclose(output);
                exit(2);
            }
            fwrite(key, 1, taille, key_file);
            fclose(key_file);
        }

        // Appel à la fonction masque_jetable
        unsigned char * output_buffer = masque_jetable(message, key, taille);
        // écriture
        fwrite(output_buffer, strlen((char*)output_buffer),1,output);
        // liberation ressources et fermeture fichiers
        free(output_buffer);
        fclose(output);

        if (key_file != NULL && key == NULL) {
            remove("key.temp");
        }
    } 

    else {
        fprintf(stderr, "Erreur : Méthode de chiffrement non reconnue.\n");
        print_utilisation();
    }

    free(vini);
    free(message);
    exit(0);
}
