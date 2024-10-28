#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

int kills;

void killsRecus(int signum);
void extendedPause();

void fils();

void main(void) {
    // Initialiser le nombre de signaux recus
    kills = 0;
    // Ecouter pour le signal SIGUSR1 (= kills recus du processus fils)
    signal(SIGUSR1, killsRecus);
    // Recuperer le PID du pere
    int pidPere = getpid();
    // Afficher le message de debut du pere
    printf("Debut du processus pere [PID %d]\n", pidPere);

    // Creer le fils
    int pidFils = fork();

    // Verification de la creation du fils
    if (pidFils < 0) {
        printf("Echec dans la creation du fils.\n");
        exit(EXIT_FAILURE);
    }
    if (pidFils == 0) { fils(); }

    // Se mettre en pause en attendant que le fils ait affiche son message
    extendedPause();

    // Une fois que le fils a envoye le signal SIGUSR1, l'execution se debloque et on peut continuer
    // On affiche le message du pere
    printf("Message du pere\n");

    // Indiquer au fils qu'il peut continuer (ici, il va afficher un message de fin et terminer son execution)
    kill(pidFils, SIGUSR1);

    // Attendre la fin de l'execution du fils
    int status;
    int pidTermine = wait(&status);
    // Verifier que c'est bien le fils qui s'est termine
    if (pidTermine != pidFils) {
        printf("On attendait la fin de %d, mais c'est %d qui s'est termine.\n", pidFils, pidTermine);
        exit(EXIT_FAILURE);
    }

    // Afficher le message de fin du pere et quitter
    printf("Fin du processus pere [PID %d]\n", pidPere);
    exit(EXIT_SUCCESS);

}

void fils() {
    // Initialiser le nombre de signaux recus
    kills = 0;
    // Ecouter pour le signal SIGUSR1 (= kills recus du processus pere)
    signal(SIGUSR1, killsRecus);

    // Recuperer le PID du pere et du fils
    int pidPere = getppid();
    int pidFils = getpid();
    // Afficher le message de debut du fils
    printf("Debut du processus fils [PID %d] [PPID %d]\n", pidFils, pidPere);

    // Afficher le message du fils
    printf("Message du fils\n");

    // Envoyer un signal au pere pour indiquer que le message est envoye
    kill(pidPere, SIGUSR1);

    // Attendre que le pere autorise a continuer
    extendedPause();

    // Une fois que le pere a envoye le signal SIGUSR1, l'execution se debloque et on peut continuer
    // On affiche le message de fin du fils
    printf("Fin du processus fils [PID %d] [PPID %d]\n", pidFils, pidPere);

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