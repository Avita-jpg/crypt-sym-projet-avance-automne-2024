#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Partie1/chiffrage_xor.h"
#include "../Partie1/cbc.h"
#include "../Partie3/break_code.h"
#include <time.h>


#define NOMBRE_CARACTERES 62
#define MAX_KEYS 100 // apres on change la valeur juste au debut 
#define DEFAULT_LOG_FILE "log.txt"
#define DICO_FRANCAIS 0
#define DICO_ANGLAIS 1

// Structure pour gérer les clés
typedef struct {
    unsigned char *key;
    int used; // 0 : non utilisée, 1 est utilisée
} Key;

Key keys[MAX_KEYS];
int key_count = 0;
FILE *log_file ; // Fichier de log global

// Prototypes des fonctions présentes
void afficher_aide();
void list_keys();
void generate_key(int n);
void delete_key(const unsigned char *key);
void execute_encrypt(const char *in, const char *out, const char *key, const char *method, const char *iv_file);
void execute_decrypt(const char *in, const char *out, const char *key, const char *method, const char *iv_file);
void execute_crack(char *input_file, const char *out, int length, char * dictionary_file);
void ecrire_log(char *message);
unsigned char *read_file_content(const char *filename, int *size);
void write_file_content(const char *filename, unsigned char *content, long size);

int main() {
    char command[256];

    // Ouvrir le fichier de log
    log_file = fopen(DEFAULT_LOG_FILE, "w");
    if (!log_file) {
        perror("Erreur lors de l'ouverture du fichier de log");
        return 1;
    }
    ecrire_log("Programme principal lancé.");

    printf("Bienvenue dans le programme principal de cryptographie symétrique.\n");
    printf("Tapez 'help' pour afficher la liste des commandes.\n");

    while (1) {
        printf("\n> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            perror("Erreur lors de la lecture de la commande.");
            ecrire_log("Erreur lors de la lecture d'une commande.");
            break;
        }

        // Supprimer le caractère de fin de ligne
        command[strcspn(command, "\n")] = 0;

        // Analyser la commande
        if (strcmp(command, "help") == 0) {
            afficher_aide();
            ecrire_log("Commande exécutée : help.");}
         else if (strncmp(command, "list-keys", 9) == 0) {
            list_keys();
            ecrire_log("Commande exécutée : list-keys.");
        } else if (strncmp(command, "gen-key", 7) == 0) {
            int n = atoi(command + 8); // Extraire la taille de la clé
            if (n > 0) {
                generate_key(n);
                char buffer[64];
                snprintf(buffer, sizeof(buffer), "Commande exécutée : gen-key. Taille de la clé : %d.", n);
                ecrire_log(buffer);
            } else {
                printf("Erreur : taille de clé invalide.\n");
                ecrire_log("Erreur : taille de clé invalide lors de gen-key.");
            }
        } else if (strncmp(command, "del-key", 7) == 0) {
            unsigned char *key = (unsigned char *)command + 8;
            delete_key(key);
            ecrire_log("Commande exécutée : del-key.");
        } else if (strncmp(command, "encrypt", 7) == 0) {
            char in[256], out[256], key[256], method[256], iv_file[256] = {0};
            sscanf(command + 8, "%s %s %s %s %s", in, out, key, method, iv_file);
            execute_encrypt(in, out, key, method, iv_file[0] ? iv_file : NULL);
            ecrire_log("Commande exécutée : encrypt.");
        } else if (strncmp(command, "decrypt", 7) == 0) {
            char in[256], out[256], key[256], method[256], iv_file[256] = {0};
            sscanf(command + 8, "%s %s %s %s %s", in, out, key, method, iv_file);
            execute_decrypt(in, out, key, method, iv_file[0] ? iv_file : NULL);
            ecrire_log("Commande exécutée : decrypt.");
        } else if (strncmp(command, "crack", 5) == 0) {
            char in[256], out[256], dico[256];
            int length;
            sscanf(command + 6, "%s %s %d %s", in, out, &length,dico);
            execute_crack(in, out, length, dico);
            ecrire_log("Commande exécutée : crack.");
         } 
         else if (strcmp(command, "quit") == 0) {
            printf("Au revoir !\n");
            ecrire_log("Commande exécutée : quit. Fin du programme.");
            break;
        }else {
            printf("Commande inconnue. Tapez 'help' pour la liste des commandes.\n");
            ecrire_log("Commande inconnue exécutée.");
        }
    }

    // Fermer le fichier de log
    fclose(log_file);

    // Libérer les clés générées
    for (int i = 0; i < key_count; i++) {
        free(keys[i].key);
    }

    return 0;
}

// Commandes
void afficher_aide() {
    printf("Commandes disponibles :\n");
    printf("  help                : Affiche cette liste d'aide.\n");
    printf("  list-keys           : Affiche la liste des clés générées.\n");
    printf("  gen-key <n>         : Génère une clé de longueur n.\n");
    printf("  del-key <key>       : Supprime la clé spécifiée.\n");
    printf("  encrypt <in> <out> <key> <method> [<iv>] : Chiffre un fichier.\n");
    printf("  decrypt <in> <out> <key> <method> [<iv>] : Déchiffre un fichier.\n");
    printf("  crack <in> <out> <length> <dico> : Cracker un fichier (dico 0 fr dico 1 ang).\n");
    printf("  quit                : Quitte le programme.\n");
}

void list_keys() {
    if (key_count == 0) {
        printf("Aucune clé générée.\n");
    } else {
        printf("Clés générées :\n");
        for (int i = 0; i < key_count; i++) {
            printf("Clé %d : %s (utilisée : %s)\n", i + 1, keys[i].key, keys[i].used ? "oui" : "non");
        }
    }
}


void generate_key(int n){
    const char caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    if (key_count < MAX_KEYS) {
        // Allocation de la clé
        keys[key_count].key = (unsigned char *)malloc(n * sizeof(unsigned char));

        srand(time(NULL));  // Initialiser le générateur de nombres aléatoires

        // Générer la clé avec des caractères choisis aléatoirement dans `caracteres`
        for (int i = 0; i < n; i++) {
            int index = rand() % NOMBRE_CARACTERES;  // Choisir un index aléatoire
            keys[key_count].key[i] = caracteres[index];  // Assigner le caractère alphanumérique
        }
        
        keys[key_count].used = 0;  // Initialiser comme non utilisée
        key_count++;
        printf("Clé générée avec succès de longueur %d.\n", n);
    } else {
        printf("Erreur : nombre maximal de clés atteint.\n");
    }
}

// Supprimer une clé
void delete_key(const unsigned char *key) {
    for (int i = 0; i < key_count; i++) {
        if (memcmp(keys[i].key, key, strlen((char *)key)) == 0) {
            free(keys[i].key);
            keys[i] = keys[key_count - 1];  // Remplacer la clé supprimée par la dernière
            key_count--;
            printf("Clé supprimée avec succès.\n");
            return;
        }
    }
    printf("Erreur : clé non trouvée.\n");
}

// Chiffrer un fichier
void execute_encrypt(const char *in, const char *out, const char *key, const char *method, const char *iv_file) {
    int input_size;
    unsigned char *input_content = read_file_content(in, &input_size);

    if (strcmp(method, "xor") == 0) {
        unsigned char *result = chiffrageXor(input_content, (unsigned char *)key, input_size);
        write_file_content(out, result, input_size);
        free(result);
        ecrire_log("Fichier chiffré avec XOR.");
    } else if (strcmp(method, "cbc") == 0) {
        unsigned char *iv = iv_file ? read_file_content(iv_file, &input_size) : NULL;
        cbc_crypt((char *)in, (char *)out, (unsigned char *)key, iv);
        free(iv);
        ecrire_log("Fichier chiffré avec CBC.");
    } else {
        printf("Erreur : Méthode non supportée.\n");
        ecrire_log("Erreur : Méthode de chiffrement non supportée.");
    }

    free(input_content);
}

// Déchiffrer un fichier
void execute_decrypt(const char *in, const char *out, const char *key, const char *method, const char *iv_file) {
    int input_size;
    unsigned char *input_content = read_file_content(in, &input_size);

    if (strcmp(method, "xor") == 0) {
        unsigned char *result = chiffrageXor(input_content, (unsigned char *)key, input_size);
        write_file_content(out, result, input_size);
        free(result);
        ecrire_log("Fichier déchiffré avec XOR.");
    } else if (strcmp(method, "cbc") == 0) {
        unsigned char *iv = iv_file ? read_file_content(iv_file, &input_size) : NULL;
        cbc_uncrypt((char *)in, (char *)out, (unsigned char *)key, iv);
        free(iv);
        ecrire_log("Fichier déchiffré avec CBC.");
    } else {
        printf("Erreur : Méthode non supportée.\n");
        ecrire_log("Erreur : Méthode de déchiffrement non supportée.");
    }

    free(input_content);
}



// Lecture d'un fichier
unsigned char *read_file_content(const char *filename, int *size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        ecrire_log("Erreur : Impossible d'ouvrir le fichier.");
        exit(1);
    }
    if((fseek(file, 0, SEEK_END))!=0){
        perror("Erreur lors de fseek pour obtenir la taille");
        fclose(file);
        return NULL;
    }
    *size = ftell(file);
    if (size<0){
        perror("Erreur lors de ftell pour obtenir la taille");
        fclose(file);
        return NULL;
    }
    rewind(file);

    unsigned char *content = malloc(*size);

    if (content == NULL) {
        perror("Erreur d'allocation mémoire pour le contenu du fichier");
        fclose(file);
        return NULL;
    }
    printf("Lecture du message\n");
    if (fread(content, sizeof(char), *size, file) != (size_t)*size) {
        perror("Erreur lors de la lecture du fichier de clé");
        free(content);
        fclose(file);
        return NULL;
    }
    content[*size] = '\0';
    fclose(file);
    printf("Fichier fermé\n");
    return content;
}

// Écriture dans un fichier
void write_file_content(const char *filename, unsigned char *content, long size) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier pour écriture");
        ecrire_log("Erreur : Impossible d'écrire dans le fichier.");
        exit(1);
    }
    fwrite(content, 1, size, file);
    fclose(file);
}
void execute_crack(char *input_file, const char *out, int length, char *dictionary_file) {
    // parametres
    int key_length = length;
    int dico;
    if(dictionary_file != NULL){
        //Dico en Français ou en Anglais
        if(strcmp(dictionary_file, "dicoFrSA.txt") == 0){ //dictionnaire français
            dico = DICO_FRANCAIS;
        }
        else if( strcmp(dictionary_file, "english.txt") == 0){ //dictionnaire anglais
            dico = DICO_ANGLAIS;
        }
        else{
            perror("Le dictionnaire n'est ni en anglais ni en français ");
            exit(1);
        }
    }
    //char *log_file = NULL; 
    unsigned long nb_clef_c1;
    unsigned long nb_clef_c2;
    char cleFinale[CARAMAX]; //clé qui est sencé decrypter le message
    char **tab_key1;
    char **tab_key2;
    int message_length;
    
    int size;
    unsigned char * message = read_file_content(input_file,&size);
    message_length=size;
    

    printf("Lancement des trois méthodes (c1, c2, c3)...\n");
    ecrire_log("Lancement des trois méthodes (c1, c2, c3)...\n");
        printf("Lancement de la méthode c1...\n");
        tab_key1 = break_code_c1((char *)message, key_length, &nb_clef_c1);

        printf("Lancement de la méthode c2...\n");
        tab_key2 = break_code_c2 (tab_key1, message, nb_clef_c1 , message_length, &nb_clef_c2, dico); //tab keys de la fonction1 de Maria
        
        printf("Nombre de clés avant bc2 : %ld\n", nb_clef_c1);
        printf("Nombre de clés retour : %ld\n", nb_clef_c2);

        printf("Lancement de la méthode c3...\n");
        break_code_c3 (input_file, tab_key2 , dictionary_file, nb_clef_c2, cleFinale, key_length, dico,DEFAULT_LOG_FILE); // qu'est ce que nbCle et aussi cleFinale
        
        printf("clé finale = %s", cleFinale);
    
    FILE  * output = fopen(out, "w");
    fprintf(output, "%s", cleFinale);
    fclose(output);
    // int
    // unsigned char *msg_chiffre = read_file_content(in, NULL);

    
    // printf("Cracking en cours...\n");

    
    // int nb_clefs;
    // char **clefs_candidates = cracker_message((char *)msg_chiffre, length, &nb_clefs);

    // printf("\nClés candidates générées (%d) :\n", nb_clefs);
    // for (int i = 0; i < nb_clefs; i++) {
    //     printf("%s\n", clefs_candidates[i]);
    // }

    
    // for (int i = 0; i < nb_clefs; i++) {
    //     free(clefs_candidates[i]);
    // }
    // free(clefs_candidates);
    // free(msg_chiffre);
}
// Ecrire dans le fichier de log
void ecrire_log(char *message) {
    if (log_file) {
        fprintf(log_file, "%s\n", message);
    }
}
