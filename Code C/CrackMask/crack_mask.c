#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

unsigned char* tabAllouer (int taille){
    unsigned char *tabElem;

    tabElem=malloc(taille*sizeof(unsigned char));
    if (tabElem==NULL){
        printf("erreur d'allocation mémoire\n");
        return NULL;
    }
    // printf("dans tabAllouer taille = %d\n",tailleUnsigned(tabElem));
    return tabElem;
}

void tabFree (unsigned char** tab){
    free(*tab);
    tab=NULL;
}

int inc(int nombre, int nbMod){
    return (nombre+1) % nbMod;
}

unsigned char* decrypt (unsigned char * message, unsigned char * cle, int tailleMessage){
    printf("debut xooooooor\n");
    unsigned char* messXor=tabAllouer(tailleMessage);
    if (!messXor){
        perror("echec allocation");
        return NULL;
    }
    int i=0;
    for (int j=0;i<tailleMessage;i++){
        messXor[i]=message[i]^cle[j];
        j=inc(j,tailleMessage);
    }
    return messXor;
}


// partie crack_mask

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

void isError(int fd,int numOpen){
  if (fd == -1){
    printf("open n°%d : ",fd);
    perror("open fail");
    exit(1);
  }
}


// int main (int argc, char * argv[]){

//   int fdC1 = open(argv[1],O_RDWR);
//   isError(fdC1,1);
//   int fdC2 = open(argv[2],O_RDWR);
//   isError(fdC2,2);
//   int fdM1 = open(argv[3],O_RDWR);
//   isError(fdM1,3);
//   int fdM2 = open(argv[4],O_RDWR);
//   isError(fdM2,4);

//   long sizeC1 = getFileSize(argv[1]);
//   long sizeC2 = getFileSize(argv[2]);
//   long sizeM1 = getFileSize(argv[3]);

//   unsigned char * C1 = tabAllouer(sizeC1);
//   unsigned char * C2 = tabAllouer(sizeC2);
//   unsigned char * M1 = tabAllouer(sizeM1);
//   unsigned char * cle;
//   unsigned char * M2;

//   if (read(fdC1,C1,sizeC1*sizeof(unsigned char))<0){
//     perror("read C1 fail");
//     exit(1);
//   }
//   if (read(fdC2,C2,sizeC2*sizeof(unsigned char))<0){
//     perror("read C2 fail");
//     exit(1);
//   }
//   if (read(fdM1,M1,sizeM1*sizeof(unsigned char))<0){
//     perror("read M1 fail");
//     exit(1);
//   }

//   cle = decrypt (M1,C1,sizeM1); //partie de la formule
//   M2 = decrypt (C2,cle,sizeC2);

//   if (write(fdM2,M2,sizeC2*sizeof(unsigned char))<0){ // écriture du message clair M2
//     perror("write M2 fail");
//     exit(1);
//   }

//   tabFree (&C1);
//   tabFree (&C2);
//   tabFree (&M1);
//   tabFree (&cle);
//   tabFree (&M2);

//   close(fdC1);
//   close(fdC2);
//   close(fdM1);
//   close(fdM2);

//   return 0;
// }