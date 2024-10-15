#! /bin/sh
#il faudra donc que tous nos fichiers possèdent les bons includes
CC=gcc
CFLAGS=-Wall
LDLIBS=-lm
RM=rm -f
OBJ=  test_chiffrage_xor.o cbc.o chiffrage_xor.o dh_prime.o 
# stats.o pour après
# EXE_CBC=cbc
# EXE_CHIFFRAGE_XOR=chiffrage_xor
EXE_TEST_CHIFFRAGE_XOR=test_chiffrage_xor 
# Pour produire les fichiers exécutables
$(EXE_TEST_CHIFFRAGE_XOR):$(OBJ)
	$(CC) $(CFLAGS) -o $(EXE_TEST_CHIFFRAGE_XOR) $(OBJ) $(LDLIBS)

# $(EXE_CBC):$(OBJ)
# 	$(CC) $(CFLAGS) $(OBJ) -o $(EXE_CBC) $(LDLIBS)

# $(EXE_CHIFFRAGE_XOR):$(OBJ)
# 	$(CC) $(CFLAGS) $(OBJ) -o $(EXE_TEST_CHIFFRAGE_XOR) $(LDLIBS)

# Pour produire les fichiers objets
test_chiffrage_xor.o:test_chiffrage_xor.c chiffrage_xor.h
	$(CC) $(CFLAGS) -c test_chiffrage_xor.c
cbc.o:cbc.c 
	$(CC) $(CFLAGS) -c cbc.c
chiffrage_xor.o:chiffrage_xor.c 
	$(CC) $(CFLAGS) -c chiffrage_xor.c
dh_prime.o:dh_prime.c dh_prime.h
	$(CC) $(CFLAGS) -c dh_prime.c
# Pour effacer les objets et les exécutables
clean:
	$(RM) $(OBJ)   $(EXE_TEST_CHIFFRAGE_XOR)  
#$(EXE_CBC) $(EXE_CHIFFRAGE_XOR)