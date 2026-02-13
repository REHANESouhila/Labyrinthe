#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include "labyrinthe.h"

int main() {
    srand(time(NULL));
    pthread_mutex_init(&verrou, NULL);
    sem_init(&semaphore, 0, MAX_THREADS);

    Coordonnee entree, sortie;
    int choix;

    printf("=== Résolution de Labyrinthe ===\n");
    printf("1. BFS Séquentiel (chemin optimal)\n");
    printf("2. Threads sans limite\n");
    printf("3. Threads avec limite\n");
    printf("Votre choix : ");
    scanf("%d", &choix);

    reinitialiser_labyrinthe(&entree, &sortie);
    printf("\nLabyrinthe généré :\n");
    afficher_labyrinthe();

    switch (choix) {
        case 1:
            bfs_sequentiel(entree, sortie);
            afficher_labyrinthe();
            break;
        case 2:
            chemin_trouve = false;
            printf("\n🔎 Recherche avec threads sans limite...\n");
            Coordonnee* c1 = malloc(sizeof(Coordonnee));
            c1->x = entree.x; c1->y = entree.y;
            pthread_t t1;
            pthread_create(&t1, NULL, threads_sans_limite, c1);
            pthread_join(t1, NULL);
            sleep(1);
            afficher_labyrinthe();
            printf(chemin_trouve ? "\n Chemin trouvé avec threads sans limite !\n" : "\nAucun chemin trouvé.\n");
            break;
        case 3:
            chemin_trouve = false;
            printf("\n Recherche avec threads limités...\n");
            Coordonnee* c2 = malloc(sizeof(Coordonnee));
            c2->x = entree.x; c2->y = entree.y;
            pthread_t t2;
            pthread_create(&t2, NULL, threads_avec_limite, c2);
            pthread_join(t2, NULL);
            sleep(1);
            afficher_labyrinthe();
            printf(chemin_trouve ? "\nChemin trouvé avec threads limités !\n" : "\n Aucun chemin trouvé.\n");
            break;
        default:
            printf("Choix invalide.\n");
            break;
    }

    return 0;
}