#! /bin/sh
#il faudra donc que tous nos fichiers possèdent les bons includes
CC=gcc
CFLAGS=-Wall
LDLIBS=-lm
RM=rm -f
OBJ=  dh_prime.o dh_gen_group.o 
# stats.o pour après
# EXE_CBC=cbc
# EXE_CHIFFRAGE_XOR=chiffrage_xor
EXE_GEN_GROUP=dh_gen_group
# Pour produire les fichiers exécutables
$(EXE_GEN_GROUP):$(OBJ)
	$(CC) $(CFLAGS) -o $(EXE_GEN_GROUP) $(OBJ) $(LDLIBS)

# $(EXE_CBC):$(OBJ)
# 	$(CC) $(CFLAGS) $(OBJ) -o $(EXE_CBC) $(LDLIBS)

# $(EXE_CHIFFRAGE_XOR):$(OBJ)
# 	$(CC) $(CFLAGS) $(OBJ) -o $(EXE_GEN_GROUP) $(LDLIBS)

# Pour produire les fichiers objets
dh_gen_group.o:dh_gen_group.c dh_prime.h
	$(CC) $(CFLAGS) -c dh_gen_group.c
dh_prime.o:dh_prime.c dh_prime.h
	$(CC) $(CFLAGS) -c dh_prime.c
# Pour effacer les objets et les exécutables
clean:
	$(RM) $(OBJ)   $(EXE_GEN_GROUP)  
#$(EXE_CBC) $(EXE_CHIFFRAGE_XOR)