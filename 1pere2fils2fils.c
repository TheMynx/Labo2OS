#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

pid_t pidPere, pidFils1, pidFils2, pidFils3, pidFils4;
int kills;

void pere(void);
void fils1(void);
void fils2(void);
void fils3(void);
void fils4(void);


void extendedPause(void);
void killReceived(void);

void signalHandler(int signum);

void main(void) {
    pidPere = getpid();
    printf("Pere [PID %d]\n", pidPere);
    fflush(stdout);
    pere();
}


void pere(void) {

    // Ecouter pour les signals SIGUSR1 
    signal(SIGUSR1, signalHandler);
    kills = 0;

    // Creer le processus fils 1
    pidFils1 = fork();
    if (pidFils1 < 0) {
        perror("Echec de la creation du fils\n");
        exit(EXIT_FAILURE);
    }
    if (pidFils1 == 0) {
        fils1();
    }

    // Attendre que le fils 1 soit cree
    extendedPause();
    
    // Creer le processus fils 3
    pidFils3 = fork();
    if (pidFils3 < 0) {
        perror("Echec de la creation du fils\n");
        exit(EXIT_FAILURE);
    }
    if (pidFils3 == 0) {
        fils3();
    } 
    
    // Attendre que le fils 3 soit cree
    extendedPause();

    // Envoyer les messages dans l'ordre croissant (1 -> 2 -> 3 -> 4)
    printf("\nMessages dans l'ordre croissant\n\n");
    fflush(stdout);
    for (int i = 0; i < 10; i++) {
        // Indiquer au fils 1 d'envoyer son message
        kill(pidFils1, SIGUSR1);
        // Attendre que le fils 1 ait envoye son message
        extendedPause();
        // Indiquer au fils 3 d'envoyer son message
        kill(pidFils3, SIGUSR1);
        // Attendre que le fils 3 ait envoye son message
        extendedPause();
    }

    // Envoyer les messages dans l'ordre decroissant (4 -> 3 -> 2 -> 1)
    printf("\nMessages dans l'ordre decroissant\n\n");
    fflush(stdout);
    for (int i = 0; i < 10; i++) {
        // Indiquer au fils 3 d'envoyer son message
        kill(pidFils3, SIGUSR1);
        // Attendre que le fils 3 ait envoye son message
        extendedPause();
        // Indiquer au fils 1 d'envoyer son message
        kill(pidFils1, SIGUSR1);
        // Attendre que le fils 1 ait envoye son message
        extendedPause();
    }



    // Quitter le fils 3
    kill(pidFils3, SIGUSR1);
    int finishedPID = wait(NULL);
    if (finishedPID != pidFils3) {
        printf("Erreur dans la fin du fils 3 - PID termine %d\n", finishedPID);
    }

    // Quitter le fils 1 et 2
    kill(pidFils1, SIGUSR1);
    finishedPID = wait(NULL);
    if (finishedPID != pidFils1) {
        printf("Erreur dans la fin du fils 1 - PID termine %d\n", finishedPID);
    } 
    
    // Quitter le pere
    printf("Fin du pere [PID %d]\n", pidPere);
    exit(EXIT_SUCCESS);
}


void fils1(void) {
    // Ecouteur SIGUSR1 + indiquer le debut du fils 1
    signal(SIGUSR1, signalHandler);
    pidFils1 = getpid();
    kills = 0;
    printf("Debut du fils 1 - PID %d - PPID %d\n", pidFils1, getppid());

    // Creer le fils 2
    pidFils2 = fork();
    if (pidFils2 < 0) {
        perror("Echec de la creatoin du fils 2");
        exit(EXIT_FAILURE);
    }
    if (pidFils2 == 0) {
        fils2();
    }

    // Attendre le debut du fils 2
    extendedPause();

    // Indiquer au pere que fils 1 + fils 2 ont demarre
    kill(pidPere, SIGUSR1);
    
    // Boucle des messages ordre croissant
    for (int i = 0; i < 10; i++) {
        // Attendre que le pere autorise a envoyer le message
        extendedPause();
        // Envoyer le message
        printf("[ASC] Message du fils 1 - PID %d - PPID %d\n", pidFils1, getppid());
        // Dire au fils d'envoyer son message
        kill(pidFils2, SIGUSR1);
        // Attendre que le fils ait envoye son message
        extendedPause();
        // Indiquer au pere que fils 1 et fils 2 ont envoye leurs messages
        kill(pidPere, SIGUSR1);
    }
    
    
    // Boucle des messages ordre decroissant
    for (int i = 0; i < 10; i++) {
        // Attendre que le pere autorise a envoyer le message
        extendedPause();
        // Indiquer au fils d'envoyer son message
        kill(pidFils2, SIGUSR1);
        // Attendre que le fils ait envoye son message
        extendedPause();
        // Envoyer le message
        printf("[DSC] Message du fils 1 - PID %d - PPID %d\n\n", pidFils1, getppid());
        // Indiquer au pere que le fils 1 et fils 2 ont envoye leurs messages
        kill(pidPere, SIGUSR1);

    }

    // Attendre que le pere autorise a quitter
    extendedPause();

    // Dire au fils 2 de se terminer
    kill(pidFils2, SIGUSR1);
    int finishedPID = wait(NULL);
    if (finishedPID != pidFils2) {
        printf("Erreur dans la fin du fils 2 - PID termine %d\n", finishedPID);
    }
    // Indiquer la fin du fils 1
    printf("Fin du fils 1 - PID %d - PPID %d\n", pidFils1, getppid());
    exit(EXIT_SUCCESS);
}

void fils2(void) {
    // Ecouteur SIGUSR1 + indiquer le debut du fils 2
    signal(SIGUSR1, signalHandler);
    pidFils2 = getpid();
    kills = 0;
    printf("Debut du fils 2 - PID %d - PPID %d\n", pidFils2, getppid());
    fflush(stdout);
    kill(getppid(), SIGUSR1);

    // Boucle des messages
    for (int i = 0; i < 10; i++) {
        // Attendre que le fils 1 autorise a envoyer le message
        extendedPause();
        // Envoyer le message
        printf("[ASC] Message du fils 2 - PID %d - PPID %d\n", pidFils2, getppid());
        // Indiquer au fils 1 que le message a ete envoye
        kill(pidFils1, SIGUSR1);
    }

    for (int i = 0; i < 10; i++) {
        // Attendre que le fils 1 autorise a envoyer le message
        extendedPause();
        // Envoyer le message
        printf("[DSC] Message du fils 2 - PID %d - PPID %d\n", pidFils2, getppid());
        // Indiquer au fils 1 que le message a ete envoye
        kill(pidFils1, SIGUSR1);
    }

    // Attendre l'autorisation du fils 1 de se terminer
    extendedPause();

    printf("Fin du fils 2 - PID %d - PPID %d\n", pidFils2, getppid());
    exit(EXIT_SUCCESS);
}

void fils3(void) {
    // Ecouteur SIGUSR1 + indiquer le debut du fils 3
    signal(SIGUSR1, signalHandler);
    pidFils3 = getpid();
    kills = 0;
    printf("Debut du fils 3 - PID %d - PPID %d\n", pidFils3, getppid());

    // Creer le fils 4
    pidFils4 = fork();
    if (pidFils4 < 0) {
        perror("Echec de la creatoin du fils 4");
        exit(EXIT_FAILURE);
    }
    if (pidFils4 == 0) {
        fils4();
    }

    // Attendre le debut du fils 4
    extendedPause();

    // Indiquer au pere que fils 3 + fils 4 ont demarre
    kill(pidPere, SIGUSR1);
    
    // Boucle des messages
    for (int i = 0; i < 10; i++) {
        // Attendre que le pere autorise a envoyer le message
        extendedPause();
        // Envoyer le message
        printf("[ASC] Message du fils 3 - PID %d - PPID %d\n", pidFils3, getppid());
        // Dire au fils d'envoyer son message
        kill(pidFils4, SIGUSR1);
        // Attendre que le fils ait envoye son message
        extendedPause();
        // Indiquer au pere que fils 3 et fils 4 ont envoye leurs messages
        kill(pidPere, SIGUSR1);
    }
    
    for (int i = 0; i < 10; i++) {
        // Attendre que le pere autorise a envoyer le message
        extendedPause();
        // Indiquer au fils d'envoyer son message
        kill(pidFils4, SIGUSR1);
        // Attendre que le fils ait envoye son message
        extendedPause();
        // Envoyer le message
        printf("[DSC] Message du fils 3 - PID %d - PPID %d\n", pidFils3, getppid());
        // Indiquer au pere que le fils 1 et fils 2 ont envoye leurs messages
        kill(pidPere, SIGUSR1);

    }

    // Attendre l'autorisation du pere de quitter
    extendedPause();

    // Dire au fils 4 de se terminer
    kill(pidFils4, SIGUSR1);
    int finishedPID = wait(NULL);
    if (finishedPID != pidFils4) {
        printf("Erreur dans la fin du fils 4 - PID termine %d\n", finishedPID);
    }
    // Indiquer la fin du fils 3
    printf("Fin du fils 3 - PID %d - PPID %d\n", pidFils3, getppid());
    exit(EXIT_SUCCESS);
}

void fils4(void) {
    // Ecouteur SIGUSR1 + indiquer le debut du fils 4
    signal(SIGUSR1, signalHandler);
    pidFils4 = getpid();
    kills = 0;
    printf("Debut du fils 4 - PID %d - PPID %d\n\n", pidFils4, getppid());
    fflush(stdout);
    kill(getppid(), SIGUSR1);

    // Boucle des messages
    for (int i = 0; i < 10; i++) {
        // Attendre que le fils 3 autorise a envoyer le message
        extendedPause();
        // Envoyer le message
        printf("[ASC] Message du fils 4 - PID %d - PPID %d\n\n", pidFils4, getppid());
        // Indiquer au fils 1 que le message a ete envoye
        kill(pidFils3, SIGUSR1);
    }

    for (int i = 0; i < 10; i++) {
        // Attendre que le fils 3 autorise a envoyer le message
        extendedPause();
        // Envoyer le message
        printf("[DSC] Message du fils 4 - PID %d - PPID %d\n", pidFils4, getppid());
        // Indiquer au fils 1 que le message a ete envoye
        kill(pidFils3, SIGUSR1);
    }

    // Attendre l'autorisation du fils 3 de se terminer
    extendedPause();

    printf("Fin du fils 4 - PID %d - PPID %d\n", pidFils4, getppid());
    exit(EXIT_SUCCESS);
}



void signalHandler(int signum) {
    kills++;
}

void extendedPause(void) {
    if (kills == 0) {
        pause();
    }
    kills--;
}