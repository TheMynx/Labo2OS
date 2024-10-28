#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

int kills;

void killsRecus(int signum);
void extendedPause();

void fils(int nbFils);

void main(void) {
    // Initialiser le nombre de signaux recus
    kills = 0;
    // Ecouter pour le signal SIGUSR1 (= kills recus du processus fils)
    signal(SIGUSR1, killsRecus);
    // Recuperer le PID du pere
    int pidPere = getpid();
    // Afficher le message de debut du pere
    printf("Debut du processus pere [PID %d]\n", pidPere);

    // Creer le premier fils
    int pidFils1 = fork();

    // Verification de la creation du fils
    if (pidFils1 < 0) {
        printf("Echec dans la creation du fils.\n");
        exit(EXIT_FAILURE);
    }
    if (pidFils1 == 0) { fils(1); }

    // Se mettre en pause en attendant que le premier fils ait affiche son message
    extendedPause();
    // Une fois que le premier fils a envoye le signal SIGUSR1, l'execution se debloque et on peut continuer
    // Creer le deuxieme fils
    int pidFils2 = fork();

    // Verification de la creation du fils
    if (pidFils2 < 0) {
        printf("Echec dans la creation du fils.\n");
        exit(EXIT_FAILURE);
    }
    if (pidFils2 == 0) { fils(2); }

    // Se mettre en pause en attendant que le deuxieme fils ait affiche son message
    extendedPause();

    // Une fois que le deuxieme fils a envoye le signal SIGUSR1, l'execution se debloque et on peut continuer
    // On affiche le message du pere
    printf("Message du pere\n");

    // Indiquer au fils 1 qu'il peut continuer (ici, il va afficher un message)
    kill(pidFils1, SIGUSR1);

    // Attendre que le fils 1 ait envoye son message
    extendedPause();

    // Indiquer au fils 2 qu'il peut continuer (ici, il va afficher un message)
    kill(pidFils2, SIGUSR1);

    // Attendre que le fils 2 ait envoye son message
    extendedPause();

    // Indiquer au fils 1 qu'il peut continuer (ici, il va afficher son message de fin et terminer son execution)
    kill(pidFils1, SIGUSR1);

    // Attendre la fin de l'execution du fils
    int status;
    int pidTermineFils1 = wait(&status);
    // Verifier que c'est bien le fils qui s'est termine
    if (pidTermineFils1 != pidFils1) {
        printf("On attendait la fin de %d, mais c'est %d qui s'est termine.\n", pidFils1, pidTermineFils1);
        exit(EXIT_FAILURE);
    }

    // Indiquer au fils 2 qu'il peut continuer (ici, il va afficher un message de fin et terminer son execution)
    kill(pidFils2, SIGUSR1);

    // Attendre la fin de l'execution du fils
    int pidTermineFils2 = wait(&status);
    // Verifier que c'est bien le fils qui s'est termine
    if (pidTermineFils2 != pidFils2) {
        printf("On attendait la fin de %d, mais c'est %d qui s'est termine.\n", pidFils2, pidTermineFils2);
        exit(EXIT_FAILURE);
    }

    // Afficher le message de fin du pere et quitter
    printf("Fin du processus pere [PID %d]\n", pidPere);
    exit(EXIT_SUCCESS);

}

void fils(int nbFils) {
    // Initialiser le nombre de signaux recus
    kills = 0;
    // Ecouter pour le signal SIGUSR1 (= kills recus du processus pere)
    signal(SIGUSR1, killsRecus);

    // Recuperer le PID du pere et du fils
    int pidPere = getppid();
    int pidFils = getpid();
    // Afficher le message de debut du fils
    printf("Debut du processus fils %d [PID %d] [PPID %d]\n", nbFils, pidFils, pidPere);

    // Indiquer au pere que le message de debut a ete affiche
    kill(pidPere, SIGUSR1);
    // Attendre que le pere autorise a envoyer le message
    extendedPause();
    // Afficher le message du fils
    printf("Message du fils %d\n", nbFils);

    // Envoyer un signal au pere pour indiquer que le message est envoye
    kill(pidPere, SIGUSR1);

    // Attendre que le pere autorise a continuer
    extendedPause();

    // Une fois que le pere a envoye le signal SIGUSR1, l'execution se debloque et on peut continuer
    // On affiche le message de fin du fils

    // Attendre quelques secondes avant la terminaison du fils
    sleep(3);
    printf("Fin du processus fils %d [PID %d] [PPID %d]\n", nbFils, pidFils, pidPere);

    // On termine le fils
    exit(EXIT_FAILURE);

}

void killsRecus(int signum) {
    kills++;
}

void extendedPause() {
    if (kills == 0) { pause();}
    kills--;
}