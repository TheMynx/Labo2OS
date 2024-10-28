#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

// Nombre maximum de processus
#define MAX_PROCESSUS 10
// Nombre de messages a envoyer
#define NB_MESSAGES 10

// Compteur de signaux recus
int kills;
// Tableau des processus
pid_t processus[MAX_PROCESSUS];


void pere(int nbProcessus);
void fils(pid_t processus[MAX_PROCESSUS], int numProcessus);

void creerFils(int nbProcessus);
void quitterFils(int nbProcessus);
void afficherMessageFils(int numProcessus, int nbFois, pid_t pidPere);

void boucleMessagesPere(int nbProcessus, int nbFois, int estOrdreCroissant);

void extendedPause();
void signalHandler(int signum);


void main(int argc, char** argv) {
    // Verifier le nombre d'arguments recus
    if (argc != 2) {
        printf("Nombre d'arguments incorrects!\nEntrez un nombre entre 1 et %d en argument.\n", MAX_PROCESSUS);
        printf("Exemple d'utilisation:\n%s %d\n", argv[0], MAX_PROCESSUS);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    // Recuperer le nombre de processus demandes
    int nbProcessus = atoi(argv[1]);
    if (nbProcessus < 1 || nbProcessus > MAX_PROCESSUS) {
        printf("Nombre de processus incorrects!\nEntrez un nombre entre 1 et %d en argument.\n", MAX_PROCESSUS);
        printf("Exemple d'utilisation:\n%s %d\n", argv[0], MAX_PROCESSUS);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    printf("Nombre de processus demandes: %d\n\n", nbProcessus);
    fflush(stdout);
    pere(nbProcessus);
}

void pere(int nbProcessus) {
    // Recuperer le PID du pere
    pid_t pidPere = getpid();
    printf("Debut du pere - PID %d\n\n", pidPere);
    fflush(stdout);
    // Creer un ecouteur de SIGUSR1
    signal(SIGUSR1, signalHandler);
    // Initier kills a 0
    kills = 0;
    // Creer les processus
    printf("\t[Creation des fils]\n\n");
    fflush(stdout);
    creerFils(nbProcessus);

    // Afficher les messages dans un ordre croissant
    printf("\t[ORDRE CROISSANT]\n\n");
    fflush(stdout);
    boucleMessagesPere(nbProcessus, NB_MESSAGES, 1);

    // Afficher les messages dans un ordre decroissant
    printf("\t[ORDRE DECROISSANT]\n\n");
    fflush(stdout);
    boucleMessagesPere(nbProcessus, NB_MESSAGES, 0);

    // Quitter les processus
    printf("\t[Terminaison des fils]\n\n");
    fflush(stdout);
    quitterFils(nbProcessus);

    // Quitter le pere
    printf("\nFin du pere - PID %d\n", pidPere);
    exit(EXIT_SUCCESS);
    
}

void fils(pid_t processus[MAX_PROCESSUS], int numProcessus) {
    // Recuperer le PID du fils et du pere
    processus[numProcessus] = getpid();
    pid_t pidPere = getppid();
    // Creer un ecouteur de SIGUSR1
    signal(SIGUSR1, signalHandler);
    // Initier kills a 0
    kills = 0;

    // Envoyer le message de debut du fils
    printf("Debut du fils %d - PID %d - PPID %d\n", numProcessus+1, processus[numProcessus], pidPere);
    fflush(stdout);
    // Dire au pere que le message est envoye
    kill(pidPere, SIGUSR1);

    // Attendre de pouvoir continuer et envoyer les messages (croissant)
    afficherMessageFils(numProcessus, NB_MESSAGES, pidPere);
    
    // Attendre de pouvoir continuer et envoyer les messages (decroissant)
    afficherMessageFils(numProcessus, NB_MESSAGES, pidPere);

    // Attendre de pouvoir se terminer et envoyer le message de fin
    extendedPause();
    printf("Fin du fils %d - PID %d - PPID %d\n", numProcessus+1, processus[numProcessus], pidPere);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}


// Cree nbProcessus fils 
void creerFils(int nbProcessus) {
    for (int iProcessus = 0; iProcessus < nbProcessus; iProcessus++) {
        processus[iProcessus] = fork();
        if (processus[iProcessus] < 0) {
            perror("Erreur dans la creation d'un fils!");
            exit(EXIT_FAILURE);
        }
        if (processus[iProcessus] == 0) {
            // Lance le processus fils
            fils(processus, iProcessus);
        }
        // Attend la creation du fils avant de continuer
        extendedPause();
    }
    printf("\n");
    fflush(stdout);
}

// Attend que les nbProcessus fils aient tous quittes
// et traite les erreurs
void quitterFils(int nbProcessus) {
    int finishedPID, status;
    for (int iProcessus = 0; iProcessus < nbProcessus; iProcessus++) {
        // Envoyer le signal de terminaison
        kill(processus[iProcessus], SIGUSR1);
        finishedPID = wait(&status);
        // Verifier que le bon processus a quitte
        if (finishedPID != processus[iProcessus]) {
            printf("Le processus %d est mort alors qu'on attendait la mort de %d\n", finishedPID, processus[iProcessus]);
            fflush(stdout);
        }
        if (status != 0) {
            printf("Le processus %d est mort par accident - status %04x\n", finishedPID, status);
            fflush(stdout);
        }
    }
}

// Affiche nbFois un message a l'ecran, puis dit a pidPere
// de continuer
void afficherMessageFils(int numProcessus, int nbFois, pid_t pidPere) {
    for (int iMessage = 0; iMessage < nbFois; iMessage++) {
        // Attend d'avoir l'autorisation d'envoyer le message
        extendedPause();
        // Envoyer le message
        printf("Message du fils %d - PID %d - PPID %d\n", numProcessus+1, processus[numProcessus], pidPere);
        fflush(stdout);
        // Le signaler au pere
        kill(pidPere, SIGUSR1);
    }
}

// Demande a nbProcessus d'envoyer nbFois series de messages
// Si estOrdreCroissant == 0, envoie les messages dans l'ordre decroissant
// Sinon, envoie les messages dans l'ordre croissant
void boucleMessagesPere(int nbProcessus, int nbFois, int estOrdreCroissant) {
    for (int iMessage = 0; iMessage < nbFois; iMessage++) {
        printf("\t[Serie de messages %d]\n\n", iMessage+1);
        fflush(stdout);
        if (estOrdreCroissant) {
            for (int iProcessus = 0; iProcessus < nbProcessus; iProcessus++) {
                // Indique au processus iProcessus d'envoyer son message
                kill(processus[iProcessus], SIGUSR1);
                // Attend que le message ait ete envoye
                extendedPause();
            }
        } else {
            for (int iProcessus = nbProcessus-1; iProcessus >= 0; iProcessus--) {
                // Indique au processus iProcessus d'envoyer son message
                kill(processus[iProcessus], SIGUSR1);
                // Attend que le message ait ete envoye
                extendedPause();
            }
        }
        printf("\n");
        fflush(stdout);
    }
}

void signalHandler(int signum) {
    kills++;
}

void extendedPause() {
    if (kills == 0) {
        pause();
    }
    kills--;
}