#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "labyrinthe.h"
#include <string.h>




sem_t semaphore;
bool chemin_trouve = false;


pthread_mutex_t verrou;

char labyrinthe[TAILLE][TAILLE];



int directions[4][2] = {
    {0, 1}, {1, 0}, {0, -1}, {-1, 0}
};

// Initialisation du labyrinthe
void initialiser_labyrinthe() {
    for (int i = 0; i < TAILLE; i++)
        for (int j = 0; j < TAILLE; j++)
            labyrinthe[i][j] = '#';
}

// Génération du labyrinthe avec des chemins
void creuser_chemin(int x, int y) {
    int ordre[4] = {0, 1, 2, 3};
    for (int i = 0; i < 4; i++) {
        int r = rand() % 4;
        int temp = ordre[i];
        ordre[i] = ordre[r];
        ordre[r] = temp;
    }
    for (int i = 0; i < 4; i++) {
        int dx = directions[ordre[i]][0] * 2;
        int dy = directions[ordre[i]][1] * 2;
        int nx = x + dx;
        int ny = y + dy;
        if (nx > 0 && nx < TAILLE - 1 && ny > 0 && ny < TAILLE - 1 && labyrinthe[ny][nx] == '#') {
            labyrinthe[y + dy / 2][x + dx / 2] = ' ';
            labyrinthe[ny][nx] = ' ';
            creuser_chemin(nx, ny);
        }
    }
}

void definir_entree_sortie(Coordonnee* entree, Coordonnee* sortie) {
    // Entrée en haut
    for (int x = 1; x < TAILLE - 1; x++) {
        if (labyrinthe[1][x] == ' ') {
            *entree = (Coordonnee){x, 0}; // E en haut
            break;
        }
    }
    // Sortie en bas
    for (int x = TAILLE - 2; x > 0; x--) {
        if (labyrinthe[TAILLE - 2][x] == ' ') {
            *sortie = (Coordonnee){x, TAILLE - 1}; // S en bas
            break;
        }
    }
    labyrinthe[entree->y][entree->x] = 'E';
    labyrinthe[sortie->y][sortie->x] = 'S';
}


void afficher_labyrinthe() {
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            if (labyrinthe[i][j] == '.')
                printf(ROUGE "%c " RESET, labyrinthe[i][j]);
            else
                printf("%c ", labyrinthe[i][j]);
        }
        printf("\n");
    }
}

// ______ BFS Séquentiel 
Coordonnee file[TAILLE * TAILLE];
Coordonnee parent[TAILLE][TAILLE];
bool visite[TAILLE][TAILLE];
int debut = 0, fin = 0;

void bfs_sequentiel(Coordonnee entree, Coordonnee sortie) {
    memset(visite, 0, sizeof(visite));
    debut = fin = 0;
    file[fin++] = entree;
    visite[entree.y][entree.x] = true;
    parent[entree.y][entree.x] = (Coordonnee){-1, -1};

    while (debut < fin) {
        Coordonnee courant = file[debut++];
        if (courant.x == sortie.x && courant.y == sortie.y) break;

        for (int i = 0; i < 4; i++) {
            int nx = courant.x + directions[i][0];
            int ny = courant.y + directions[i][1];
            if (nx >= 0 && ny >= 0 && nx < TAILLE && ny < TAILLE &&
                labyrinthe[ny][nx] != '#' && !visite[ny][nx]) {
                visite[ny][nx] = true;
                parent[ny][nx] = courant;
                file[fin++] = (Coordonnee){nx, ny};
            }
        }
    }

    Coordonnee p = sortie;
    if (parent[p.y][p.x].x == -1) {
        printf("\n Aucun chemin trouvé avec BFS.\n");
        return;
    }

    while (parent[p.y][p.x].x != -1) {
        if (labyrinthe[p.y][p.x] != 'S') labyrinthe[p.y][p.x] = '.';
        p = parent[p.y][p.x];
    }
    printf("\nChemin trouvé avec BFS !\n");
}

// ____ Threads sans limite 
void* threads_sans_limite(void* arg) {
    Coordonnee* position = (Coordonnee*)arg;
    int x = position->x, y = position->y;
    free(position);
    if (x < 0 || y < 0 || x >= TAILLE || y >= TAILLE || labyrinthe[y][x] == '#' || labyrinthe[y][x] == '.' || chemin_trouve) return NULL;
    if (labyrinthe[y][x] == 'S') {
        pthread_mutex_lock(&verrou);
        chemin_trouve = true;
        pthread_mutex_unlock(&verrou);
        return NULL;
    }
    if (labyrinthe[y][x] != 'E') labyrinthe[y][x] = '.';
    pthread_t fils[4];
    for (int i = 0; i < 4; i++) {
        Coordonnee* suivant = malloc(sizeof(Coordonnee));
        suivant->x = x + directions[i][0];
        suivant->y = y + directions[i][1];
        pthread_create(&fils[i], NULL, threads_sans_limite, suivant);
        pthread_detach(fils[i]);
    }
    return NULL;
}

// ___Threads avec limite ___
void* threads_avec_limite(void* arg) {
    sem_wait(&semaphore);
    Coordonnee* position = (Coordonnee*)arg;
    int x = position->x, y = position->y;
    free(position);
    if (x < 0 || y < 0 || x >= TAILLE || y >= TAILLE || labyrinthe[y][x] == '#' || labyrinthe[y][x] == '.' || chemin_trouve) {
        sem_post(&semaphore);
        return NULL;
    }
    if (labyrinthe[y][x] == 'S') {
        pthread_mutex_lock(&verrou);
        chemin_trouve = true;
        pthread_mutex_unlock(&verrou);
        sem_post(&semaphore);
        return NULL;
    }
    if (labyrinthe[y][x] != 'E') labyrinthe[y][x] = '.';
    pthread_t fils[4];
    for (int i = 0; i < 4; i++) {
        Coordonnee* suivant = malloc(sizeof(Coordonnee));
        suivant->x = x + directions[i][0];
        suivant->y = y + directions[i][1];
        pthread_create(&fils[i], NULL, threads_avec_limite, suivant);
        pthread_detach(fils[i]);
    }
    sem_post(&semaphore);
    return NULL;
}

// Réinitialisation du labyrinthe
void reinitialiser_labyrinthe(Coordonnee* entree, Coordonnee* sortie) {
    initialiser_labyrinthe();
    creuser_chemin(1, 1);
    definir_entree_sortie(entree, sortie);
}


