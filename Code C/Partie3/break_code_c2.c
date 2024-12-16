#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include "../Partie1/chiffrage_xor.h"
#include "break_code.h"
#define DICO_FRANCAIS 0

#define NOMBRE_CARACTERES 62
#define ALPHABET_SIZE 26
//#define LEN_TAB_KEY 7


//Structure intermédiaire pour avoir la clé associé à son score
typedef struct {
    char* key;
    float score;
}KeyValuePair;

//tableau des fréquences théoriques d'apparitions de chaque lettres en anglais
float stat_thEn[ALPHABET_SIZE] = {8.167, 1.492, 2.782,4.253,12.702,2.228,2.015,6.094,
        6.966,0.153,0.772,4.025,2.406,6.749,7.507,1.929,0.095,5.987,6.327,9.056,2.758,
        0.978,2.36,0.15,1.974,0.074};

//tableau des fréquences théoriques d'apparitions de chaque lettres en français 
float stat_thFr[ALPHABET_SIZE] = {9.42, 1.02, 2.64, 3.39, 15.87, 0.95, 1.04, 0.77, 8.41, 0.89,
		0.00, 5.34, 3.24, 7.15,  5.14, 2.86, 1.06, 6.46, 7.90, 7.26,
		6.24, 2.15, 0.00, 0.30, 0.24, 0.32};



void letter_frequency_calculator(char* text, float frequences[ALPHABET_SIZE]){
    //on initialise le tableau des fréquences à 0
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        frequences[i] = 0;
    }
    int total_letters = 0;
    //on va parcourir le texte 
    for(int i=0; text[i] != '\0'; i++){
        char c = text[i]; //on prend le caractère courant
        if(isalpha(c)){
            c = toupper(c); //on met le caractère en majuscule pour uniformiser
            frequences[c-'A']++; //on incrémente  la fréquence de la lettre correspondante
            total_letters++;
        }
    }
    //On calcule les fréquances relatives 
    if(total_letters > 0){
        for(int k = 0; k < ALPHABET_SIZE; k++){
            frequences[k] /= total_letters;
        }
    }
}

// Fonction pour afficher les fréquences de chaque lettres
void afficherFrequences(float frequences[ALPHABET_SIZE]) {
    printf("Fréquence des lettres :\n");
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (frequences[i] > 0) { // Afficher seulement les lettres présentes
            printf("%c : %f\n", 'A' + i, frequences[i]);
        }
    }
}

// Fonction pour afficher toutes les clés avec leurs scores (float)
void afficherPaires(KeyValuePair pairs[], int taille) {
    printf("Clés et scores :\n");
    for (int i = 0; i < taille; i++) {
        printf("Clé : %s, Score : %.2f\n", pairs[i].key, pairs[i].score);
    }
}

//décoder le texte avec la clé (c-à-d : avoir un texte avec des lettres)
//appel à ma fonction xor, param : unsigned char * mais retour : char *
char * decoder_texte(unsigned char* text_crypted, char* key, int len_text){
    unsigned char* key_unsigned = (unsigned char *)key;
    unsigned char * decode = chiffrageXor(text_crypted, key_unsigned, len_text);
    return (char *)decode; //voir si ça marche
}

//fonction calcul de la distance entre les fréquences (correspond à la d dans l'énoncé)
float calcul_distance(float freq_th[], float freq[]){
    float distance = 0;
    float diff;
    for(int i = 0; i < ALPHABET_SIZE; i++){
        diff = freq_th[i] - freq[i];
        distance += (diff * diff);
    }
    return distance;
}

//fonction de comparaison de score (pour l'utiliser avec qsort)
int compare(const void* a, const void* b){
    KeyValuePair *pairA = (KeyValuePair*) a;
    KeyValuePair *pairB = (KeyValuePair*) b;

    if (pairA->score < pairB->score) return -1;
    else if (pairA->score > pairB->score) return 1;
    return 0;
}

//tab_keys : tableau des clés qui vient de break_code_c1
//fonction (principale) qui renvoie un tableau des clés classés par leurs score
char **break_code_c2 (char** tab_keys, unsigned char* crypted, unsigned long len_tab_key, int len_text, unsigned long* nb_clefs_retour, int dico_language){
    KeyValuePair * pairs = malloc(len_tab_key * sizeof(KeyValuePair)); //tableau de structures KeyValuePair
    if (pairs == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    /************    Remplissage du tab intermédiaire    **************/ 
    for(unsigned long i = 0; i < len_tab_key; i++){
        pairs[i].key = tab_keys[i]; //on affecte à la structure la clé i 
        char* decode = decoder_texte(crypted, tab_keys[i], len_text); //on decode

        float frequencies[ALPHABET_SIZE]; //sera initialisé dans la prochaine fonction
        letter_frequency_calculator(decode, frequencies); //remplit le tableau frequencies
        //afficherFrequences(frequencies); //on regarde si le tableau est bien remplit

        float keyScore;
        if(dico_language == DICO_FRANCAIS){ // dico en français
            keyScore = calcul_distance(stat_thFr, frequencies);
        }
        else{ //dico en anglais
            keyScore = calcul_distance(stat_thEn, frequencies);
        }
        
        pairs[i].score = keyScore;
    } //clé et score remplit dans le tableau pair
    
    /************    Tri du tableau et remplissage du tableau final    **************/
    unsigned long n = len_tab_key; //taille tableau de structures


    // if(dico_language == 0){ // dico en français
    // printf("on est dans le dico français et n/5000\n");
    //     *nb_clefs_retour = n/50;
    // }
    // else{ //dico en anglais
    //     printf("on est dans le dico anglais et n/500\n");
    //     *nb_clefs_retour = n/1000;
    // }
    
    // printf("n : %d\n",n);
    // printf("nb_clef : %d\n", *nb_clefs_retour);

    qsort(pairs, n, sizeof(KeyValuePair), compare); //tri du tableau par ordre decroissant

    /************    Déterminer la taille des centilles    **************/
    unsigned long centille_size = len_tab_key / 100; // Taille d'un centille
    if (centille_size == 0) centille_size = 1; // Gérer les petits tableaux

    int centilles_to_keep = 1; // Nombre de centilles à garder (10% des clés les mieux scorées)
    unsigned long keys_to_keep = centilles_to_keep * centille_size;

    if (keys_to_keep > len_tab_key) keys_to_keep = len_tab_key; // Ne pas dépasser le nombre total de clés

    *nb_clefs_retour = keys_to_keep;

    //************************************

    char** clefs = malloc(sizeof(char*) * keys_to_keep); // Allouer de l'espace pour N clés (remplace N par la taille souhaitée)
    if (clefs == NULL) {
        perror("Erreur d'allocation mémoire");
        return NULL; // Gérer l'erreur si l'allocation échoue
    }

    //voir si besoin d'un malloc pour le tableau final
    for(unsigned long j = 0; j < *nb_clefs_retour - 1; j++){
        clefs[j] = pairs[j].key; //on remplit le tableau final
    }

    // // Afficher les clés triées
    // printf("Clés triées par score décroissant :\n");
    // for (int i = 0; i < *nb_clefs_retour - 1; i++) {
    //     printf("%s\n", clefs[i]);
    // }

    free(pairs);
    return clefs;
}



// //main pour tester les fonctions auxiliaires
// int main(void){

//     //initialisations
//     unsigned char *message;
//     long taille_fichier;
//     char* tab_key[LEN_TAB_KEY] = {"jghfkgdk","df9","rutabaga","salut","cool", "df8", "xaa"};

//     // Ouverture du fichier à chiffrer
//     FILE* fich = NULL;

//     // On ouvre le fichier avec le message que l'on doit chiffrer
//     fich  = fopen("coucou", "rb");

//     if (fich == NULL) {
//         perror("Erreur lors de l'ouverture du fichier");
//         exit(1);
//     }

//     //On prends la taille du message dans le fichier
//     fseek(fich, 0, SEEK_END);
//     taille_fichier = ftell(fich);  // Obtenir la taille du fichier
//     rewind(fich);  // Remettre le curseur au début du fichier

//     // Allouer suffisamment de mémoire pour stocker tout le fichier
//     message = (unsigned char*) malloc(sizeof(unsigned char) * taille_fichier + 1);
//     if (message == NULL) {
//         perror("Erreur allocation mémoire");
//         fclose(fich);
//         exit(1);
//     }

//     // Lecture dans le fichier et on met ça dans une variable unsigned char*
//     if(fread(message, 1, taille_fichier, fich) != (size_t)taille_fichier){
//         perror("Erreur lecture fichier : ");
//         free(message);
//         fclose(fich);
//         exit(1);
//     }

//     message[taille_fichier] = '\0';

//     //on chiffre le message 
//     unsigned char * msg_chiffre = chiffrageXor(message, (unsigned char *)tab_key[1], taille_fichier);

//     //test fonction break_code_2
//     int* nb_cles_tab = malloc(sizeof(int));

//     char ** tab_final = break_code_c2(tab_key, msg_chiffre, LEN_TAB_KEY, taille_fichier, nb_cles_tab, 0); //doit afficher les clés finales

//     // //test decode
//     // char * mess_decode = decoder_texte(msg_chiffre, tab_key[0], taille_fichier);

//     // //test fréquences
//     // float frequencies[ALPHABET_SIZE];
//     // letter_frequency_calculator(mess_decode, frequencies);
//     // afficherFrequences(frequencies); //on affiche

//     //test score

//     //test compare


//     //test break_code_c2


//     return 0;
// }