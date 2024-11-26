#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
// #include "dh_prime.h" 
// à ajouter plus tard
#define MAX_PRIME 4294967296/1000 // 2^32 = sqrt(2^64)
#define MIN_PRIME 100
/*
long random_long(long min,long max){
/// \brief génère un uint aléatoire entre min et max
/// \param[in] min et max des uint
/// \return n : min≤n≤max
  srand(time(NULL)); // ajout de cette ligne pour que rand() renvoit des nombres differents à chaque excécution
  long nb = rand()%(max-min);
  printf("nb random = %ld\n",nb);
  return (nb) + min;
}

long puissance_mod_n (long a, long e, long n) {
  /// /brief puissance modulaire, calcule a^e mod n
  /// a*a peut dépasser la capacité d'un long
  /// https://www.labri.fr/perso/betrema/deug/poly/exp-rapide.html
  /// vu au S1 en Python
  long p;
  for (p = 1; e > 0; e = e / 2) {
    if (e % 2 != 0)
      p = (p * a) % n;
    a = (a * a) % n;
  }
  return p;
}



int rabin (long a, long n) {
  /// \brief test de Rabin sur la pimarité d'un entier
  /// \brief c'est un test statistique. Il est plus rapide que le précédent.
  /// \param[in] a : on met 2, ça marche
  /// \param[in] n : le nombre à tester
  /// \returns 1 il est premeir, 0 sinon
  long p, e, m;
  int i, k;

  e = m = n - 1;
  for (k = 0; e % 2 == 0; k++)
    e = e / 2;

  p = puissance_mod_n (a, e, n);
  if (p == 1) return 1;

  for (i = 0; i < k; i++) {
    if (p == m) return 1;
    if (p == 1) return 0;
    p = (p * p) % n;
  }

  return 0;
}


// à enlever plus tard
long genPrimeSophieGermain(long min,long max,int *cpt){
  /// \brief fournit un nombre premier de Sophie Germain vérifié avec le test de rabin
  /// \param[in] min et max
  /// \param[out] cpt : le nombre d'essais pour trouver le nombre
  /// \returns un nombre premier p min≤p≤max && p = 2*q+1 avec q premier
  long num;
  *cpt=1;
  int a=2;
  do{
    num = random_long(min,max);
    printf("in do --> num = %ld\n",num);
  } while (num%2!=1);

  while ((!rabin(a,num) || !rabin(a,2*num+1)) && num<max){
    (*cpt)++;
    num=num+2;
  }
  printf("num = %ld\n",num);
  return 2*num+1;
}


long genPrimeSophieGermain(long min,long max,int *cpt){
  /// \brief fournit un nombre premier de Sophie Germain vérifié avec le test de rabin
  /// \param[in] min et max
  /// \param[out] cpt : le nombre d'essais pour trouver le nombre
  /// \returns un nombre premier p min≤p≤max && p = 2*q+1 avec q premier

  long num;
  *cpt=1;
  int a=2;

  do{
    num = random_long(min,max);
  } while (num%2!=1);

  while ((!rabin(a,num) || !rabin(a,2*num+1)) && num<max){
    (*cpt)++;
    num=num+2;
  }
  return num;
}


long seek_generator(long start,long p){
  /// \brief recherche d'un générateur du groupe (corps) cyclique Z/pZ avec p premier
  long q = (p-1)/2; /// \note p est un premier de Sophie Germain ie p =2q+1 avec q premier

  /// \note si p = \prod(i=1,k,{p_i}^{n_i}) alors g est un générateur si \forall i 1..k, g^p_i != 1 (mod p)
  /// comme p = 2q+1 et q premier, k = 2 et p_i = 2 et q.
  /// il suffit que g^2 et g^q soient tous les deux différents de 1 (mod p)
  while ((puissance_mod_n(start, 2, p) == 1 || puissance_mod_n(start,q,p) == 1) && start < p-1){
    start++;
  }
  if (start == p-1) return -1;
  printf("start = %ld\n",start);
  return start;
} */


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
