# Nom de l'exécutable
EXEC = labyrinthe

# Fichiers sources
SRC = main.c labyrinthe.c

# Fichiers objets (générés automatiquement)
OBJ = $(SRC:.c=.o)

# Compilateur
CC = gcc

# Options de compilation (ajoute -g pour debug ou -O2 pour optimisation)
CFLAGS = -Wall -pthread

# Cible par défaut
all: $(EXEC)

# Règle de création de l'exécutable
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

# Règle pour nettoyer les fichiers objets et l'exécutable
clean:
	rm -f $(OBJ) $(EXEC)

# Optionnel : pour recompiler tout depuis zéro
rebuild: clean all
