#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define N 16






// typedef unsigned char tTab;


int tailleUnsigned (unsigned char * message){
    int taille;
    for (taille=0;message[taille]!='\0';taille++);
    return taille;
}

int inc(int nombre, int nbMod){
    return (nombre+1) % nbMod;
}

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

void xor_blocks(unsigned char* block1, unsigned char* block2, size_t len) {
    for (size_t i = 0; i < len; i++) {
        block1[i] ^= block2[i];
    }
}

unsigned char* decrypt (unsigned char * message, unsigned char * cle, int tailleTab){
    printf("debut xooooooor\n");
    printf("taille clé = %d\n",tailleUnsigned(cle));
    unsigned char* messXor=tabAllouer(tailleTab);
    if (!messXor){
        perror("echec allocation");
        return NULL;
    }
    int i=0;
    for (int j=0;i<tailleTab;i++){
        printf("xor entre message = %c et cle = %c\n",message[i],cle[j]);
        messXor[i]=message[i]^cle[j];
        printf("caractère decrypté = %c\n",messXor[i]);
        // printf("mess is      =    %02x\n",messXor[i]);
        j=inc(j,tailleUnsigned(cle));
    }
    printf("fin de xooooor\n\n\n");
    return messXor;
}


void isError(int nb, int nbError){// 0 for write, 1 for Read, 2 for open
    if (nbError==1 && nb==-1){
        perror("read fail");
        exit(1);
    } else if (nbError==0 && nb==-1){
        perror("write fail");
        exit(1);
    } else if (nbError==2 && nb==-1){
        perror("open fail");
        exit(1);
    }
}

void tabFree (unsigned char** tab){
    free(*tab);
    tab=NULL;
}
void cbc_crypt(char *nomFichEntre, char *nomFichSortie, unsigned char* cle, unsigned char* vi) {
    int fdE = open(nomFichEntre, O_RDONLY);
    int fdS = open(nomFichSortie, O_WRONLY | O_CREAT );
    unsigned char* Bi = malloc(N * sizeof(unsigned char));
    unsigned char* Ci = malloc(N * sizeof(unsigned char));
    unsigned char* Cinter = malloc(N * sizeof(unsigned char));
    int nbRead, nbWrite;

   isError(fdE, 2);
   isError(fdS, 2);

    for (int i = 0; i < N; i++) {
        Ci[i] = vi[i];
    }

    while ((nbRead = read(fdE, Bi, N)) > 0) {
       
        if (nbRead < N) {
            memset(Bi + nbRead, 0, N - nbRead);  
        }

        
        for (int i = 0; i < N; i++) {
            Cinter[i] = Bi[i] ^ Ci[i];
        }

        
        for (int i = 0, j = 0; i < N; i++, j = (j + 1) % tailleUnsigned(cle)) {
            Ci[i] = Cinter[i] ^ cle[j];
        }

        
        nbWrite = write(fdS, Ci, N);
        isError(nbWrite, 0);
    }
    
    isError(nbRead, 1);
    close(fdE);
    close(fdS);
    free(Bi);
    free(Ci);
    free(Cinter);
}


void cbc_uncrypt (char * nomFichEntre, char * nomFichSortie, unsigned char * cle,unsigned char * vi){
    int fdE=open(nomFichEntre,O_RDONLY);
    int fdS=open(nomFichSortie,O_WRONLY);
    unsigned char* Ci=tabAllouer(N);
    unsigned char* Cinter=tabAllouer(N);

    unsigned char* Bi;
    unsigned char* CiDecrypt;
    int nbRead,nbWrite;
    int isFirst=1;

    while ((nbRead=read(fdE,Ci,N*sizeof(unsigned char)))){
        CiDecrypt=decrypt(Ci,cle,N/*&CiDecrypt*/);
        if (isFirst){
            Bi=decrypt(CiDecrypt,vi,N/*&Bi*/); 
        } else {
            Bi=decrypt(CiDecrypt,Cinter,N/*&Bi*/);
        }
        isFirst=0;
        for (int i=0;i<N;i++){
            Cinter[i]=Ci[i];
        }
        nbWrite=write(fdS,Bi,N*sizeof(unsigned char));
        isError(nbWrite,0);
    }
    isError(nbRead,1);
    tabFree (&CiDecrypt);
    tabFree (&Bi);
    tabFree (&Ci);
    tabFree (&Cinter);

    close(fdE);
    close(fdS);
}



//    int main() {
//
//     unsigned char cle[] = "adijd";
//     unsigned char vi[N];
//
//     memset(vi, 0, N);
//
//     char *fichierEntree = "cbc_adijd_mini.txt";
//     char *fichierSortie = "output_crypt.txt";
//
//     cbc_crypt(fichierEntree, fichierSortie, cle, vi);
//
//     printf("Chiffrement terminé. Résultat dans '%s'.\n", fichierSortie);
//
//     return 0;
// }
//
