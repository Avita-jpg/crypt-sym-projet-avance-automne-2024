#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "dh_prime.h" 
#define MAX_PRIME 4294967296/1000 
#define MIN_PRIME 100


void print_help() { // message à afficher en cas de commande avec paramètre -h
    printf("Utilisation : ./dh-gen-group [OPTIONS]\n");
    printf("Options disponibles :\n");
    printf("  -o        Le fichier où l'on va écrire les arguments p et g\n");
    printf("  -h        Affiche ce message d'aide\n");
    printf("Pour créer ensuite la clé: taper en ligne de commande:\n");
    printf("python3 dh_genkey.py -i param.txt -o key.txt\n");
    printf("avec param.txt qui représente le fichier avec les paramètres généré par ce programme\n");
    printf("et key.txt dans lequel va être écrit la clé calculée par dh_genkey.py\n");
}


int main (int argc, char * argv[]){

    char *output = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) { //cas avec paramètre -h pour message d'aide
            print_help();
            return 0;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) { //cas avec fichier de output
            output = argv[i + 1];  
            i++;  // on saute le nom du fichier pour ne traiter que les paramètres
        }
    }
    // on vérifie si output a bien été défini
    if (output != NULL) {
        printf("Le fichier de sortie est : %s\n", output);
    } else {
        fprintf(stderr, "Aucun fichier de sortie spécifié avec -o.\n");
        return 1;
    }

    FILE *file = fopen(output,"w");
    if (file == NULL){
        perror("fopen fail");
        return 1;
    }

    long p,g; // p le nombre premier de sophie germain et g generateur
    int cpt;
    p=genPrimeSophieGermain(MIN_PRIME,MAX_PRIME,&cpt); 
    g=seek_generator(2,p);
    printf("p = %ld\n",p);
    printf("g = %ld\n",g);

    fprintf(file,"%ld\n%ld",p,g); // écriture de p et g dans fichier avec saut à la ligne pour lecture en python
    fclose(file);
    return 0;
}