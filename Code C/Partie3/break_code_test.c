#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>

#include "break_code.h"


int main (int argc, char* argv[]){


 // pour crypter un fichier

//argv[1] = message à crypter
//argv[2] = message crypté

  // int fdMessage = open (argv[1],O_RDWR);
  // if(fdMessage == -1){
  //   perror("open fail");
  //   exit(1);
  // }

  // long tailleF = getFileSize(argv[1]); //very important
  // printf("tailleF = %ld\n",tailleF);
  // unsigned char message[tailleF];
  // if (read(fdMessage,message,tailleF*sizeof(char))<0){
  //   perror("read fail");
  //   exit(1);
  // } 

  // char cle[6]="gf1u2";
  // char crypted[tailleF+1];
  // decrypt (message,cle,tailleF,crypted);

  // int fdCrypt = open (argv[2],O_RDWR);
  // if(fdCrypt == -1){
  //   perror("open fail");
  //   exit(1);
  // }

  // if (write(fdCrypt,crypted,tailleF*sizeof(char))<0){
  //   perror("read fail");
  //   exit(1);
  // } 

  //début test
  
//argv[1] = message à décrypter
//argv[2] = longueur de la clé
//argv[3] = dictionnaire (français ou anglais)

  int fd = open (argv[1],O_RDWR);
  if(fd == -1){
    perror("open fail");
    exit(1);
  }

  long tailleF = getFileSize(argv[1]); //very important
  printf("tailleF = %ld\n",tailleF);
  unsigned char message[tailleF]; //!!unsigned
  if (read(fd,message,tailleF*sizeof(char))<0){
    perror("read fail");
    exit(1);
  } 

  int nbCles;
  int longueurCle = atoi(argv[2]);

  //partie 1
  printf("avant crack1\n");
  char ** cleCrack1 = cracker_message(message,longueurCle,&nbCles,tailleF); //ajout tailleF
  
  //partie 2
  int nbClesC2;
  printf("avant crack2\n");
  printf("nbCles = %d\n",nbCles);
  char ** cleCrack2 = break_code_c2 (cleCrack1,(unsigned char*)message,nbCles,tailleF,&nbClesC2);
  

  //partie 3
  printf("avant for dans main partie 3\n");
  char cleFinale[CARAMAX];

  printf("avant crack3\n");
  crack_code_c3 (argv[1],cleCrack2,argv[3],nbClesC2,cleFinale,longueurCle,1);

  //résultat final
  printf("cle finale : ");
  for (int i=0;i<longueurCle;i++){
    printf("%c",cleFinale[i]);
  }
  printf("\n");

  return 0;
}