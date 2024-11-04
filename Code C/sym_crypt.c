#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "chiffrage_xor.h"
#include "cbc.h"

//TODO ajouter module masque jettable
//#include ""

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
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture d'un fichier");
        exit(2);
    }
    // obtenir taille du fichier de clé
    fseek (file, 0, SEEK_END);
    long size=ftell (file);
    // revenir au debut du fichier
    rewind(file);

    // On alloue un espace pour la clé, en ajoutant 1 pour le caractère de fin de chaîne
    unsigned char * content = (unsigned char*)malloc((size + 1) * sizeof(unsigned char));

    // Lecture de la clé
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
                print_utilisation();
                exit(0);
            default:
                print_utilisation();
                exit(1);
        }
    }

    // Vérification des options obligatoires
    if (!input_file || !output_file || !key || !method) {
        fprintf(stderr, "Erreur : Les options -i, -o, -k et -m sont obligatoires.\n");
        print_utilisation();
        exit(3);
    }

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
    } else if (!vini_file) {
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
    } else if (strcmp(method, "cbc-crypt") == 0) {
        cbc_crypt(input_file, output_file, key, vini);
    } else if (strcmp(method, "cbc-uncrypt") == 0) {
        cbc_uncrypt(input_file, output_file, key, vini);
    } else if (strcmp(method, "mask") == 0) {
        printf("TODO (to add)\n");
        // TODO: implémenter la fonction de chiffrement par masque
        // mask_encrypt(input_file, output_file, key);
    } else {
        fprintf(stderr, "Erreur : Méthode de chiffrement non reconnue.\n");
        print_utilisation();
    }

    free(vini);
    free(key);
    free(message);
    exit(0);
}
