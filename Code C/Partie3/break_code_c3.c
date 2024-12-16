#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>
#define MOTMAX 27 // estimation taille maximale d'un mot du dictionnaire
#define CARAMAX 50 // longeur maximale d'une clé
#define NBMAXMOT 528 //estimation nombre maximal de mots dans un fichier
#define TAILLEMAXMOT 528 //estimation taille max d'un mot décodé séparé par séparateurs
#define NBMAXCLE 99999999 // nombre maximum de clés possibles

#include "break_code.h" // à ajouter

char alphabetMaj[27] =  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; //pour fonction trouverMots
char alphabetMin[28] = "abcdefghijklmnopqrstuvwxyz-";
#define NBLASTMOT 9 
char lastLetters[10] = "rstuvwxyz";
int nbMotLettresFr[27] = 
{267362,
157142,
385608,
481627,
390457,
130434,
105917,
63062,
139678,
23125,
4723,
68935,
176253,
40177,
56392,
301027,
13226,
407616,
251387,
176445,
10172,
74724,
2009,
645,
754,
7520}; // le nombre de caractères des mots qui commencent par la lettre dans le dictionnaire francais (pour pouvoir utiliser fseek dans estDansDico)

int nbMotLettresEn[27] = 
{904,
739,
1214,
746,
682,
647,
468,
384,
386,
116,
116,
464,
686,
258,
347,
880,
55,
694,
1597,
738,
235,
304,
403,
0,
33,
20}; //comme nbMotsLettresFr mais pour le dico anglais

struct cleDico { // struct pour associer une clé au nombre de mots qui sont dans le dico après decyptage
  char cle[CARAMAX];
  int nbDico;
};
typedef struct cleDico Cle_Dico;

bool isEqualWord (char mot1[],char mot2[],int taille){ // si deux mots égaux
  for (int i=0;i<taille;i++){
    if (mot1[i]!=mot2[i]){
      return false;
    }
  }
  return true;
}

int sommeInd (int nbMotLettres[], int ind){ // revoit la somme d'une liste jusqu'à un certain indice
  int somme=0;
  for (int i=0;i<ind;i++){
    somme+=nbMotLettres[i];
  }
  return somme;
}

long getFileSize(const char *filename) {
    FILE *file = fopen(filename, "rb"); 
    if (file == NULL) {
        perror("fopen fail");
        return 1;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file); //pour trouver taille fichier
    fclose(file); 

    return size;
}

int incrementer(int nombre, int nbMod){
    return (nombre+1) % nbMod;
}

void decrypter (unsigned char crypt[], char cle[], int tailleCrypt,char decrypt[]){ //pour decoder le fichier avec une clé
    int i=0;
    for (int j=0;i<tailleCrypt;i++){
        decrypt[i]=crypt[i]^cle[j];
        j=incrementer(j,strlen(cle));
    }
}

bool isCarIn (char car, char tab[]){ // vérifier si un caractère car fait parti du tableau tab
  for (int i=0;i<strlen(tab);i++){
    if (car == tab[i]){
      return true;
    }
  }
  return false;
}

int trouverPos (char car, char chaine[]){ // trouver la position d'un caractère car dans la chaine donnée
  for (int i=0;i<strlen(chaine);i++){
    if (car == chaine[i]){
      return i;
    }
  }
  return -1;
}

int trouverMots (char message[],char mots[][TAILLEMAXMOT],int tailleMessage){ //retourne le nombre de mots dans la chaine de caractère et rempli le tableau mots[][] des mots trouvés

  bool isMotMaj = true; //si il y a une majuscule autre part qu'au premier caractère du mot, dans ce cas ce n'est pas un mot pris en compte
  bool isMotSup0 = false; //si le mot est supperieur à 0
  bool isMotInfMax = true; //si le mot est inferieur à MOTMAX
  int debutMot; //indice du commencement du mot courant
  int longeurMot = 0;
  int nbMots = 0; //le nombre de mots trouvé en tout

  for (int i=0;i<tailleMessage;i++){
    bool isMaj=false;
    if (isCarIn(message[i],alphabetMin) || (isMaj=isCarIn(message[i],alphabetMaj))){ // si le caractère est une lettre en l'alphabet en miniscule ou majuscule
      if(isMaj){
        if (longeurMot==0){
          int pos = trouverPos(message[i],alphabetMaj);
          message[i]=alphabetMin[pos]; //je remplace la majuscule par un minuscule
          debutMot=i;
        } else {
          isMotMaj = false; // si la majuscule n'est pas au premier caractère nous savons que ce n'est pas un mot possible
        }
      } else {
        if (longeurMot == 0){
          debutMot = i; 
        }
      }
      longeurMot++;
      isMotSup0 = true;
      if (longeurMot > MOTMAX){ // si le mot est trop long il ne peut pas être un mot du dictionnaire
        isMotInfMax = false;
      }  
    }
    if ((!isCarIn(message[i],alphabetMin) && !(isCarIn(message[i],alphabetMaj))) || i==tailleMessage-1){ //si pas une lettre ou '-' ou à la fin du message
      if (isMotMaj && isMotInfMax && isMotSup0){ 
        int posFin=debutMot+longeurMot;
        if ((message[debutMot+longeurMot-1]=='-')){//le tiret ne peu être qu'au mileu du mot sinon on enlève le tiret du mot
          posFin--;
        }
        else if ((message[debutMot]=='-')){
          debutMot++;
        } 
        int pos=0;
        for (int j=debutMot;j<posFin;j++,pos++){  //on ajoute le mot au tableau s'il respecte bien les conditions de mot valide
          mots[nbMots][pos]=message[j];
        }
        nbMots++;
      }
      longeurMot = 0;
      isMotSup0 = false;
      isMotMaj = true;
      isMotInfMax = true;
    } 
  }
  return nbMots;
}

bool estDansDico (char mot[],FILE *fd,long tailleDico, int isEnglish){

  int pos = trouverPos(mot[0],alphabetMin);
  if (pos == -1 || pos == 26){
    return false;
  }

  int posDico,posDicoFin;
  if (isEnglish){
    posDico = sommeInd(nbMotLettresEn,pos);
    posDicoFin = nbMotLettresEn[pos];
  } else {
    posDico = sommeInd(nbMotLettresFr,pos);
    posDicoFin = nbMotLettresFr[pos];
  }

  if (fseek(fd, posDico, SEEK_SET) != 0) { //pour optimiser on teste que les mots du dictionnaire qui commence par la même lettre que le notre
    perror("fseek SEEK_SET fail");
    exit(1);
  }

  int taille = strlen(mot);
  int tailleDicoMot;
  char motDico[MOTMAX];

  for (int i=0;i<posDicoFin;i++){
    fgets(motDico, sizeof(motDico), fd); //lecture ligne par ligne pour chaque mot du fichier dictionnaire
    tailleDicoMot = strlen(motDico)-1; //on enlève \n

    if (taille == tailleDicoMot){ //pour optimiser les appels à la lecture de tout le dico
      if (isEqualWord (mot,motDico,taille)){ //on vérifie l'égalité entre le mot du dico et notre mot à nous
          return true;
      }
    }
  }
  return false;
}

void afficherDicoCle (struct cleDico mots[NBMAXCLE],int nbCle,int tailleCle,char * fichierLog){

int stdout_backup;

  if (fichierLog != NULL){
    stdout_backup = dup(STDOUT_FILENO); // enregistre stdout
    int fd = open(fichierLog, O_WRONLY | O_APPEND);
    if (fd == -1) {
        perror("Erreur d'ouverture du fichier");
        exit(1);
    }
    dup2(fd, STDOUT_FILENO); // redirige stdout vers fd
    close(fd);
  }
  printf("\n");
  for (int i=0;i<nbCle;i++){
    printf("Cle n°%d = ",i); //afficher le nombre de mots dans le dico des clés
    for (int j=0;j<tailleCle;j++){
      printf("%c",(mots[i].cle)[j]);
    }
    printf(" et nbDico = %d\n",mots[i].nbDico);
  }

  if (fichierLog != NULL){
    dup2(stdout_backup, STDOUT_FILENO); //remettre stdout dans le terminal
    close(stdout_backup);
  }

}

int fposMax(struct cleDico mots[NBMAXCLE],int nbCle,int tailleCle){ // trouver la position de la clé avec le nbDico maximal

  int max = mots[0].nbDico;
  int posMax = 0;
  for (int i=1;i<nbCle;i++){
    if (mots[i].nbDico>max){
      max = mots[i].nbDico;
      posMax=i;
    }
  }
  return posMax;
}


void trierListe (struct cleDico mots[NBMAXCLE],struct cleDico cleTriee[NBMAXCLE],int nbCle, int tailleCle){

  int nbCleCourant = nbCle; //nombre de clé dans le tableau mots que l'on modifie au fur et à mesure
  int tailleTriee = 0; //taille du tableau cleTriee

  for (int posMax;nbCleCourant!=0;){
    posMax = fposMax (mots,nbCleCourant,tailleCle);

    cleTriee[tailleTriee].nbDico = mots[posMax].nbDico; //on ajoute la cle avec le plus de mots[] décodé dans le dico dans la liste triée
    for (int i=0;i<tailleCle;i++){
      (cleTriee[tailleTriee].cle)[i] = (mots[posMax].cle)[i];
    }
    tailleTriee++;

    for (int i=posMax;i<nbCleCourant-1;i++){ // on retire la cle à l'indice posMax du tableau mots[]
      mots[i].nbDico = mots[i+1].nbDico;
      for (int j=0;j<tailleCle;j++){
        (mots[i].cle)[j] = (mots[i+1].cle)[j];
      }
    }
    nbCleCourant--;
  }
}


void break_code_c3 (char * nomCrypt,char** cles,char * nomDico, unsigned long nbCle, char cleFinale[CARAMAX],int tailleCle,int isEnglish, char * fichierLog){ 

  int fdCrypt = open (nomCrypt,O_RDWR); 
  if (fdCrypt == -1){
    perror("open crypt fail");
    exit(1);
  }

  FILE *fd = fopen(nomDico, "r"); 
  if (fd == NULL) {
    perror("fopen nomDico fail");
    exit(1);
  }
  long tailleDico = getFileSize(nomDico); //taille du fichier crypté

  Cle_Dico* cleD = malloc(nbCle * sizeof(Cle_Dico)); //tableau de toutes les clés avec toutes leurs occurences de mots du dico
  if (cleD == NULL) {
    perror("Failed to allocate memory");
    exit(EXIT_FAILURE);
  }
  long tailleMessage = getFileSize(nomCrypt); //taille du fichier crypté

  unsigned char crypt[tailleMessage]; //le fichier crypté    
  if (read(fdCrypt,crypt,tailleMessage*sizeof(unsigned char))<0){
    perror("read fail");
    exit(1);
  }
  char* decode = malloc (tailleMessage*sizeof(char)); // le message decodé avec la clé courante

  for (unsigned long i=0;i<nbCle-2;i++){ //pour chaque clé 
    printf("cle à tester n°%ld = %s\n",i,cles[i]);
    strcpy(cleD[i].cle,cles[i]);
    cleD[i].nbDico=0;

    memset(decode,0,tailleMessage* sizeof(char));
    decrypter(crypt,cleD[i].cle,tailleMessage,decode);

    char mots[NBMAXMOT][TAILLEMAXMOT]; //tous les mots contenu dans le message decrypteré avec cette clé
    memset(mots, 0,sizeof(mots));
    int nbMots = trouverMots(decode,mots,tailleMessage);

    for (int j=0; j<nbMots; j++){ //pour chaque mot de mots de mots[]
      if (estDansDico(mots[j],fd,tailleDico,isEnglish)){ 
        cleD[i].nbDico+=1; // si le mot est dans le dico on incrémente la nbDico de la clé courante
      }   
    }
  }

  struct cleDico * cleTriee = malloc(nbCle * sizeof(Cle_Dico)); 
  if (cleTriee == NULL) {
    perror("Failed to allocate memory");
    exit(EXIT_FAILURE);
  }
  trierListe (cleD,cleTriee,nbCle-2,tailleCle);
  afficherDicoCle (cleTriee,nbCle-2,tailleCle,fichierLog);

  close(fdCrypt);
  fclose(fd);
  printf("nbDico vrai clé = %d\n",cleTriee[0].nbDico);
  strcpy(cleFinale,cleTriee[0].cle); // on rempli la clé finale avec la clé qui décode le plus de mots dans le dico

  free(cleD);
  free(cleTriee);
} 


// int main (int argc, char * argv[]){

  // test de isDansDico <3 <3 <3

  // char mot1[5]="open";
  // char mot2[10]="hilibolii";
  // char mot3[6]="jewel";
  // char mot4[5]="bomb";
  // char mot5[7]="strgvn";

  // bool dico1 = estDansDico (mot1,argv[1]);
  // printf("%s est dans dico  :  %d\n",mot1,dico1);

  // bool dico2 = estDansDico (mot2,argv[1]);
  // printf("%s est dans dico  :  %d\n",mot2,dico2);

  // bool dico3 = estDansDico (mot3,argv[1]);
  // printf("%s est dans dico  :  %d\n",mot3,dico3);

  // bool dico4 = estDansDico (mot4,argv[1]);
  // printf("%s est dans dico  :  %d\n",mot4,dico4);

  // bool dico5 = estDansDico (mot5,argv[1]);
  // printf("%s est dans dico  :  %d\n",mot5,dico5);

  // char mot[6]="about";

  // bool dico = estDansDico (mot,argv[1]);
  // printf("%s est dans dico  :  %d\n",mot,dico);

  // char moot[6]="zebra";

  // bool dicoo = estDansDico (moot,argv[1]);
  // printf("%s est dans dico  :  %d\n",moot,dicoo);

  // char moot1[6]="bigla";
  // char moot2[7]="rvznlo";
  // char moot3[6]="salut";
  // char moot4[9]="okdokphh";
  // char moot5[8]="gambade";

  // bool dicoo1 = estDansDico (moot1,argv[1]);
  // printf("%s est dans dico  :  %d\n",moot1,dicoo1);

  // bool dicoo2 = estDansDico (moot2,argv[1]);
  // printf("%s est dans dico  :  %d\n",moot2,dicoo2);

  // bool dicoo3 = estDansDico (moot3,argv[1]);
  // printf("%s est dans dico  :  %d\n",moot3,dicoo3);

  // bool dicoo4 = estDansDico (moot4,argv[1]);
  // printf("%s est dans dico  :  %d\n",moot4,dicoo4);

  // bool dicoo5 = estDansDico (moot5,argv[1]);
  // printf("%s est dans dico  :  %d\n",moot5,dicoo5);


  // //test de trouverMot <3 <3 <3

  // char mots[NBMAXMOT][TAILLEMAXMOT];
  // int fd = open (argv[2],O_RDWR);
  // if(fd == -1){
  //   perror("open fail");
  //   exit(1);
  // }

  // long tailleF = getFileSize(argv[2]); //very important
  // printf("tailleF = %ld\n",tailleF);
  // char message[tailleF];
  // if (read(fd,message,tailleF*sizeof(char))<0){
  //   perror("read fail");
  //   exit(1);
  // }
  // int nbMots = trouverMots (message,mots,tailleF);

  // printf("nbMots = %d\n",nbMots);
  // printf("mots = ");
  // for (int i=0;i<nbMots;i++){
  //   printf("%s\n",mots[i]);
  // }

  // test trie de tableau struct <3 <3 <3

  // struct cleDico triee[NBMAXCLE];
  // struct cleDico cles[NBMAXCLE];

  //   cles[0].nbDico=6;
  // (cles[0].cle)[0]='n';
  // (cles[0].cle)[1]='h';
  // (cles[0].cle)[2]='8';
  // (cles[0].cle)[3]='g';
  //   cles[1].nbDico=0;
  // (cles[1].cle)[0]='m';
  // (cles[1].cle)[1]='a';
  // (cles[1].cle)[2]='p';
  // (cles[1].cle)[3]='6';
  //   cles[2].nbDico=34;
  // (cles[2].cle)[0]='m';
  // (cles[2].cle)[1]='a';
  // (cles[2].cle)[2]='x';
  // (cles[2].cle)[3]='x';
  //   cles[3].nbDico=0;
  // (cles[3].cle)[0]='a';
  // (cles[3].cle)[1]='m';
  // (cles[3].cle)[2]='o';
  // (cles[3].cle)[3]='m';
  //   cles[4].nbDico=4;
  // (cles[4].cle)[0]='d';
  // (cles[4].cle)[1]='f';
  // (cles[4].cle)[2]='9';
  // (cles[4].cle)[3]='8';
  //   cles[5].nbDico=676;
  // (cles[5].cle)[0]='a';
  // (cles[5].cle)[1]='d';
  // (cles[5].cle)[2]='j';
  // (cles[5].cle)[3]='7';

  // int nbCles = 6;
  // int lenCle = 4;

  // trierListe (cles,triee,nbCles,lenCle);

  // for (int i=0;i<nbCles;i++){
  //   printf("cle %d : \t %s : \t %d\n",i,triee[i].cle,triee[i].nbDico);
  // }

  // test fonction principale <3 <3 <3

  // // crypter un message:
  // int fdMessage = open(argv[1],O_RDWR);
  // if (fdMessage==-1){
  //   perror("open message fail");
  //   exit(1);
  // }
  // int fdCrypt = open (argv[2],O_RDWR);
  // if (fdCrypt==-1){
  //   perror("open crypt fail");
  //   exit(1);
  // }

  // int tailleMessage = 40;
  // char message[tailleMessage];
  // if (read(fdMessage,message,tailleMessage*sizeof(char))<0){
  //   perror("read message fail");
  //   exit(1);
  // }

  // char cle[5]="adj6";
  // char crypt[tailleMessage];

  // decrypter (message,cle,tailleMessage,crypt);

  // if (write(fdCrypt,crypt,tailleMessage*sizeof(char))<0){
  //   perror("write crypt fail");
  //   exit(1);
  // }

  // close(fdMessage);
  // close(fdCrypt);


// autre partie idk anymore

//   int fdCrypt = open (argv[1],O_RDWR);
//   if (fdCrypt==-1){
//     perror("open crypt fail");
//     exit(1);
//   }
//   printf("apres open 1\n");

//   int tailleMessage = getFileSize(argv[1]);
//   // char* crypt = malloc(tailleMessage*sizeof(char));
//   unsigned char crypt[tailleMessage];
//   if (read(fdCrypt,crypt,tailleMessage*sizeof(char))<0){
//     perror("read crypt fail");
//     exit(1);
//   }

//   char cleFinale[CARAMAX];
//   // char cles[NBMAXCLE][CARAMAX];
//       char **cles = malloc(7 * sizeof(char *));
//     if (cles == NULL) {
//         perror("malloc failed for cles");
//         exit(1);
//     }

//      for (int i = 0; i < 7; i++) {
//         cles[i] = malloc(5 * sizeof(char));
//         if (cles[i] == NULL) {
//             perror("malloc failed for cles[i]");
//             for (int j = 0; j < i; j++) {
//                 free(cles[j]);
//             }
//             free(cles);
//             exit(1);
//         }
//     }

//   printf("après malloc cles\n");
//     cles[0][0]='h';
//   cles[0][1]='i';
//   cles[0][2]='p';
//   cles[0][3]='y';
//   cles[0][4]='\0';
//     cles[1][0]='a';
//   cles[1][1]='d';
//   cles[1][2]='j';
//   cles[1][3]='7';
//   cles[1][4]='\0';
//     cles[2][0]='d';
//   cles[2][1]='f';
//   cles[2][2]='9';
//   cles[2][3]='8';
//   cles[2][4]='\0';
//     cles[3][0]='l'; 
//   cles[3][1]='o';
//   cles[3][2]='v';
//   cles[3][3]='e'; 
//   cles[3][4]='\0';
//     cles[4][0]='c'; // la vrai clé
//   cles[4][1]='o';
//   cles[4][2]='a';
//   cles[4][3]='6';
//   cles[4][4]='\0';
//     cles[5][0]='9';
//   cles[5][1]='6';
//   cles[5][2]='9';
//   cles[5][3]='6';
//   cles[5][4]='\0';
//     cles[6][0]='a';
//   cles[6][1]='l';
//   cles[6][2]='i';
//   cles[6][3]='e'; 
//   cles[6][4]='\0'; 
//   printf("where\n");  
  
//   int nbCle = 7;
//   int tailleCle = 4;

//   crack_code_c3 (argv[1],cles,argv[2],nbCle,cleFinale,tailleCle);

//   printf("cleFinale = %s\n",cleFinale);
//   char chaine_decrypter[tailleMessage];
//   decrypter (crypt,cleFinale,tailleMessage,chaine_decrypter);

//   printf("chaine_decrypter = ");
//   for (int i=0;i<tailleMessage;i++){
//     printf("%c",chaine_decrypter[i]);
//   }
//   printf("\n");

//       for (int i = 0; i < NBMAXCLE; i++) {
//         free(cles[i]); // Libère chaque clé
//     }
//     free(cles); // Libère le tableau de pointeurs


//   close(fdCrypt);

//   return 0;
// }