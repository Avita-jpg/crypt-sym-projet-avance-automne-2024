# Nom de l'exécutable
EXEC = sym_crypt

# Options de compilation
CFLAGS = -Wall -Wextra -pedantic

# Les fichiers sources et objets
SRC = sym_crypt.c chiffrage_xor.c masque_jetable.c cbc.c 
OBJ = $(SRC:.c=.o)

# La règle principale pour générer l'exécutable
$(EXEC): $(OBJ)
	@gcc $(CFLAGS) -o $(EXEC) $(OBJ)
	@echo "Compilation de $(EXEC) terminée."

# Compilation des fichiers .o
%.o: %.c
	@gcc $(CFLAGS) -c $< -o $@
	@echo "Compilation de $< terminée."

# Commande pour nettoyer les fichiers objets et l'exécutable
clean:
	@rm -f $(OBJ) $(EXEC)
	@echo "Nettoyage terminé."

# Commande pour forcer la recompilation complète
rebuild: clean $(EXEC)
