#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t child1, child2;
    int pipe_fd[2];  // Tableau pour les descripteurs du pipe

    if (pipe(pipe_fd) == -1) {
        perror("Erreur lors de la création du pipe");
        return 1;
    }

    // Crée le premier processus fils
    child1 = fork();

    if (child1 < 0) {
        perror("Erreur lors de la création du premier processus fils");
        return 1;
    }
    if (child1 == 0) {
        //printf("Je suis le premier fils : %d et mon pere est %d\n", getpid(), getppid());
        // Code du premier processus fils
        close(pipe_fd[0]);  // Ferme le descripteur de lecture
        int data_to_send = 42;
        write(pipe_fd[1], &data_to_send, sizeof(data_to_send));
        close(pipe_fd[1]);
        return 0;
    }

    // Processus parent

    // Crée le deuxième processus fils
    child2 = fork();

    if (child2 < 0) {
        perror("Erreur lors de la création du deuxième processus fils");
        return 1;
    }
    if (child2 == 0) {
        //printf("Je suis le deuxiem fils : %d et mon pere est %d\n", getpid(), getppid());
        // Code du deuxième processus fils
        close(pipe_fd[1]);  // Ferme le descripteur d'écriture
        int received_data;
        read(pipe_fd[0], &received_data, sizeof(received_data));
        printf("Processus fils 2 a reçu des données : %d\n", received_data);
        close(pipe_fd[0]);
        return 0;
    }

    // Processus parent
    close(pipe_fd[0]);  // Ferme le descripteur de lecture inutilisé
    close(pipe_fd[1]);  // Ferme le descripteur d'écriture inutilisé

    // Attendez que les deux processus fils se terminent
    wait(NULL);
    wait(NULL);

    printf("Le processus parent (PID %d) s'est terminé.\n", getpid());

    return 0;
}
