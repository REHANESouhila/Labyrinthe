#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

#define TAILLE 21
#define MAX_THREADS 10

#define ROUGE "\033[1;31m"
#define RESET "\033[0m"

typedef struct {
    int x, y;
} Coordonnee;

// Variables globales
extern char labyrinthe[TAILLE][TAILLE];
extern sem_t semaphore;
extern bool chemin_trouve;
extern pthread_mutex_t verrou;

// Directions pour le déplacement
extern int directions[4][2];

// Fonctions de génération du labyrinthe
void initialiser_labyrinthe();
void creuser_chemin(int x, int y);
void definir_entree_sortie(Coordonnee* entree, Coordonnee* sortie);
void reinitialiser_labyrinthe(Coordonnee* entree, Coordonnee* sortie);

// Fonctions d'affichage
void afficher_labyrinthe();

// BFS séquentiel
void bfs_sequentiel(Coordonnee entree, Coordonnee sortie);

// Résolution avec threads
void* threads_sans_limite(void* arg);
void* threads_avec_limite(void* arg);

#endif // LABYRINTHE_H
