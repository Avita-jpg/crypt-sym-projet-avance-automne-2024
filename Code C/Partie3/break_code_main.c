#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "../Partie1/chiffrage_xor.h"
#include "break_code.h"
#define DICO_FRANCAIS 0
#define DICO_ANGLAIS 1

void afficher_aide() {
    printf("Usage: ./break_code [options]\n");
    printf("Options:\n");
    printf("  -i <fichier>       Fichier à décrypter\n");
    printf("  -m <c1|all>        Méthode de décryptage (c1 pour méthode 1, all pour toutes les méthodes dans l'ordre)\n");
    printf("  -k <longueur>      Longueur de la clef\n");
    printf("  -d <fichier>       Fichier dictionnaire (utilisée seulement avec -m all)\n");
    printf("  -l <fichier>       Fichier de log\n");
    printf("  -h                 Afficher cette aide\n");
}

//fonction qui renvoie la taille du fichier 
void prendreTaille(char *fich_name, int *taille){
    // Ouverture du fichier à chiffrer
    FILE* fich = NULL;

    // On ouvre le fichier avec le message que l'on doit chiffrer
    // fich  = fopen("../script/CRYPT/tests/ref/msg2.txt", "rb");
    fich  = fopen(fich_name, "rb");

    if (fich == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(1);
    }

    //On prends la taille du message dans le fichier
    fseek(fich, 0, SEEK_END);
    int taille_fichier = ftell(fich);  // Obtenir la taille du fichier
    rewind(fich);  // Remettre le curseur au début du fichier

    *taille = taille_fichier;    
    fclose(fich);
}

unsigned char *read_content(const char *filename) {
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
    long size = ftell (file); //pointeur sur un long (aka la taille du fichier)
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
    printf("Fichier fermé\n");

    return content;
}


int main(int argc, char *argv[]){
    printf("**************   Début du programme break_code_main   **************\n\n");
    // Variables pour les options
    char *input_file = NULL;
    char *method = NULL; //c1 ou all
    int key_length = 0;
    char *dictionary_file = NULL;
    char *log_file = NULL; 

    unsigned long nb_clesC1_retour = 0;
    unsigned long nb_clesC2_retour;
    int dico_language;
    char cleFinale[CARAMAX]; //clé qui est sencé decrypter le message
    char **tab_key1;
    char **tab_key2;

    int opt;

    while ((opt = getopt(argc, argv, "i:m:k:d:l:h")) != -1) {
        switch (opt) {
            case 'i':
                input_file = optarg;
                break;
            case 'm':
                method = optarg;
                break;
            case 'k':
                key_length = atoi(optarg); // Convertir en entier la clé
                break;
            case 'd':
                dictionary_file = optarg;
                break;
            case 'l':
                log_file = optarg;
                break;
            case 'h':
                afficher_aide();
                return 0;
            default:
                afficher_aide();
                return 1;
        }
    }

    // Validation des options
    if (!input_file || !method || key_length <= 0 ) {
        fprintf(stderr, "Erreur : les options -i, -m et -k sont obligatoires.\n");
        afficher_aide();
        return 1;
    }

    if (strcmp(method, "all") == 0 && !dictionary_file) {
        fprintf(stderr, "Erreur : l'option -d est obligatoire avec -m all.\n");
        afficher_aide();
        return 1;
    }

    if(dictionary_file != NULL){
        //Dico en Français ou en Anglais
        if(strcmp(dictionary_file, "dicoFrSA.txt") == 0){ //dictionnaire français
            dico_language = DICO_FRANCAIS;
        }
        else if( strcmp(dictionary_file, "english.txt") == 0){ //dictionnaire anglais
            dico_language = DICO_ANGLAIS;
        }
        else{
            perror("Le dictionnaire n'est ni en anglais ni en français ");
            exit(1);
        }
    }
    printf("Dictionnaires (0 -> français | 1 -> anglais)\n");
    printf("Le dictionnaire utilisé est : %s -> %d\n",dictionary_file, dico_language);

    //Récupération dela longueur du fichier fournit + Récupération du contenu du fichier
    int message_length;

    unsigned char * message = read_content(input_file);
    if (message == NULL) {
        perror("Message est à NULL");
        exit(2);
    }

    prendreTaille(input_file, &message_length); //voir si ça marche bien
    printf("message_length : %d\n", message_length);

    // Exécution des fonctions selon la méthode choisie
    if (strcmp(method, "c1") == 0) {
        // Appeler la méthode c1
        printf("Lancement de la méthode c1...\n");
        tab_key1 = break_code_c1((char *)message, key_length, &nb_clesC1_retour);
    } 

    
    //cas de toutes les méthodes (dans l'ordre)
    else if (strcmp(method, "all") == 0) {

        // Appeler les trois méthodes
        printf("Lancement des trois méthodes (c1, c2, c3)...\n");
        printf("Lancement de la méthode c1...\n");
        tab_key1 = break_code_c1((char *)message, key_length, &nb_clesC1_retour);

        printf("Lancement de la méthode c2...\n");
        tab_key2 = break_code_c2 (tab_key1, message, nb_clesC1_retour , message_length, &nb_clesC2_retour, dico_language); //tab keys de la fonction1 de Maria
        printf("Nombre de clés avant bc2 : %ld\n", nb_clesC1_retour);
        printf("Nombre de clés retour : %ld\n", nb_clesC2_retour);

        printf("Lancement de la méthode c3...\n");
        if(log_file == NULL){
            break_code_c3 (input_file, tab_key2 , dictionary_file, nb_clesC2_retour, cleFinale, key_length, dico_language, NULL);
        }
        else{
            break_code_c3 (input_file, tab_key2 , dictionary_file, nb_clesC2_retour, cleFinale, key_length, dico_language, log_file);
        }
    } 
    
    //cas de la mauvaise méthode
    else {
        fprintf(stderr, "Erreur : méthode non reconnue. Utilisez 'c1' ou 'all'.\n");
        afficher_aide();
        return 1;
    }

    printf("La clé trouvée est : %s\n", cleFinale);

    return 0;
}