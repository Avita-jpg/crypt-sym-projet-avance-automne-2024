#ifndef BREAK_CODE_H 
#define BREAK_CODE_H 
#define CARAMAX 50 // longeur maximale d'une clé

/*
 * Fonction read_content
 * Permet de lire le conteu d'un fichier
 */


/*
 * Fonction break_code_c1
 * Emondage des carcatères possibles pour la clé
 */
char** break_code_c1(char* msg_chiffre, int longueur_cle, unsigned long * nb_clefs_retour);

/*
 * Fonction break_code_c2
 * Statistiques des lettres dans la langue cible
 */
char ** break_code_c2 (char** tab_keys, unsigned char* crypted, unsigned long len_tab_key, int len_text, unsigned long* nb_clefs_retour, int dico_language);

/*
 * Fonction break_code_c3
 * Utilisation d'un dictionnaire de la langue cible
 */
void break_code_c3 (char * nomCrypt,char** cles/*[][CARAMAX]*/ ,char * nomDico, unsigned long nbCle, char cleFinale[CARAMAX],int tailleCle, int isEnglish, char *log_file);

#endif 
