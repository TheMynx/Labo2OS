#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_RECURSIONS 9

void main(int argc, char** argv) {
    // Recuperer le nom du fichier
    char* filename = argv[0];
    // S'assurer qu'on a bien recu deux parametres
    if (argc != 2) {
        printf("Usage incorrect! Exemple d'utilisation:\n%s %d\n", filename, MAX_RECURSIONS);
        exit(EXIT_FAILURE);
    }
    // Recuperer le deuxieme argument
    int nbRecursions = atoi(argv[1]);
    // S'assurer qu'il est compris dans l'intervalle [0 ; MAX_RECURSIONS]
    if (nbRecursions > MAX_RECURSIONS || nbRecursions < 0) {
        printf("Usage incorrect! Entrez un nombre de recursions compris entre 1 et %d\n", MAX_RECURSIONS);
        exit(EXIT_FAILURE);
    }
    // Cas de base
    if (nbRecursions > 0) {
        // Afficher un message a l'ecran
        printf("Recursion num. %d\n", nbRecursions);
        // Decrementer le nombre de recursions
        nbRecursions--;

        // Preparer les nouveaux arguments d'appel du programme (c'est-a-dire l'equivalent de char** argv)
        char nbRecursionsChar[2];
        // Convertir nbRecursions en char
        sprintf(nbRecursionsChar, "%d", nbRecursions);
        char* args[3] = {filename, nbRecursionsChar, NULL};
        execv(filename, args);
    }

    exit(EXIT_SUCCESS);
    
}
