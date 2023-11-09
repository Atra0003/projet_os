#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>

int shared_data;  // Variable partagée
int shm_fd;        // Descripteur de la mémoire partagée
pid_t child1, child2;
int pipe_fd1[2];  // Tableau pour les descripteurs du pipe
int pipe_fd2[2];

void sigpipe_handler(int signo) {
    // Gestionnaire du signal SIGPIPE
    fprintf(stderr, "Signal SIGPIPE reçu. Arrêt propre de tous les processus.\n");
    close(pipe_fd1[0]);
    close(pipe_fd1[1]);
    close(pipe_fd2[0]);
    close(pipe_fd2[1]);
    if (child1 > 0) {
        kill(child1, SIGTERM);
    }
    if (child2 > 0) {
        kill(child2, SIGTERM);
    }
    shm_unlink("/my_shared_memory"); // Supprime la mémoire partagée
    exit(1);
}

void sigint_handler(int signo) {
    // Gestionnaire du signal SIGINT
    fprintf(stderr, "Signal SIGINT reçu. Arrêt propre de tous les processus.\n");
    if (child1 > 0) {
        kill(child1, SIGTERM);
    }
    if (child2 > 0) {
        kill(child2, SIGTERM);
    }
    wait(NULL); // Attendez que les deux processus fils se terminent
    wait(NULL);
    shm_unlink("/my_shared_memory"); // Supprime la mémoire partagée
    exit(1);
}


int main() {
    // Installation du gestionnaire de signal pour SIGPIPE
    signal(SIGPIPE, sigpipe_handler);

    // Installation du gestionnaire de signal pour SIGINT
    signal(SIGINT, sigint_handler);

    // Crée la mémoire partagée
    shm_fd = shm_open("/my_shared_memory", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Erreur lors de la création de la mémoire partagée");
        return 1;
    }
    ftruncate(shm_fd, sizeof(int));

    // Mappe la mémoire partagée dans l'espace d'adressage de ce processus
    int* shared_data = (int*)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    printf("La valeur partagée est : %d\n", *shared_data);

    if (pipe(pipe_fd1) == -1 || pipe(pipe_fd2) == -1) { // Créez deux pipes
        perror("Erreur lors de la création des pipes");
        return 1;
    }

    // Crée le premier processus fils
    child1 = fork();

    if (child1 < 0) {
        perror("Erreur lors de la création du premier processus fils");
        return 1;
    }
    if (child1 == 0) {
        // Code du premier processus fils
        close(pipe_fd1[0]);  // Ferme le descripteur de lecture inutilisé
        int data_to_send = 42;
        write(pipe_fd1[1], &data_to_send, sizeof(data_to_send));
        close(pipe_fd1[1]);

        int received_data1;
        close(pipe_fd2[1]); // Ferme le descripteur d'écriture inutilisé
        read(pipe_fd2[0], &received_data1, sizeof(received_data1));
        printf("La valeur reçue dans le pipe 2 depuis le fils 1 est : %d\n", received_data1);
        close(pipe_fd2[0]);

        if (*shared_data > received_data1) {
            *shared_data = received_data1;
        }
        return 0;
    }

    // Crée le deuxième processus fils
    child2 = fork();

    if (child2 < 0) {
        perror("Erreur lors de la création du deuxième processus fils");
        return 1;
    }
    if (child2 == 0) {
        // Code du deuxième processus fils
        close(pipe_fd2[0]);  // Ferme le descripteur de lecture inutilisé
        int data_to_send = 25;
        write(pipe_fd2[1], &data_to_send, sizeof(data_to_send));
        close(pipe_fd2[1]);

        int received_data2;
        close(pipe_fd1[1]); // Ferme le descripteur d'écriture inutilisé
        read(pipe_fd1[0], &received_data2, sizeof(received_data2));
        printf("La valeur reçue dans le pipe 1 depuis le fils 2 est : %d\n", received_data2);
        close(pipe_fd1[0]);

        if (*shared_data > received_data2) {
            *shared_data = received_data2;
        }

        return 0;
    }

    // Processus parent
    close(pipe_fd1[0]);  // Ferme le descripteur de lecture inutilisé dans le parent
    close(pipe_fd1[1]);  // Ferme le descripteur d'écriture inutilisé dans le parent
    close(pipe_fd2[0]);  // Ferme le descripteur de lecture inutilisé dans le parent
    close(pipe_fd2[1]);  // Ferme le descripteur d'écriture inutilisé dans le parent

    // Attendez que les deux processus fils se terminent
    wait(NULL);
    wait(NULL);

    printf("####### La valeur partagée est : %d\n", *shared_data);
    printf("Le processus parent (PID %d) s'est terminé.\n", getpid());

    return 0;
}



 int* shared_data;  // Pointeur vers la mémoire partagée
    int shm_fd;        // Descripteur de la mémoire partagée

    // Crée la mémoire partagée
    shm_fd = shm_open("/my_shared_memory", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Erreur lors de la création de la mémoire partagée");
        return 1;
    }
    ftruncate(shm_fd, sizeof(int));

    // Mappe la mémoire partagée dans l'espace d'adressage de ce processus
    shared_data = (int*)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);





DIR *directory;
struct dirent *entry;

const char *path = argv[2];
directory = opendir(path);

if(directory == NULL){
    perror("Erreur lors de l'ouverture du dossier");
    return 1;
} 
    

    int cpt = 0;
    while ((entry = readdir(directory)) != NULL){
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
            //Ecriture dans le premier pipe
            close(pipe_fd1[0]);  // Ferme le descripteur de lecture inutilisé
            ///int data_to_send = 42;
            write(pipe_fd1[1], entry->d_name, strlen(entry->d_name));
            close(pipe_fd1[1]);

            //Ecriture dans le deuxieme pipe
            close(pipe_fd2[0]);  // Ferme le descripteur de lecture inutilisé
            //int data_to_send = 1;
            write(pipe_fd2[1], entry->d_name, strlen(entry->d_name));
            close(pipe_fd2[1]);


            if((cpt % 2) == 0){
                if (child1 == 0) {
                    //Lecture du contenu du premier pipe 1 par le fils 1
                    char received_data1[strlen(entry->d_name)];
                    close(pipe_fd1[1]); // Ferme le descripteur d'écriture inutilisé
                    read(pipe_fd1[0], &received_data1, strlen(received_data1));
                    printf("La valeur reçue dans le pipe 1 depuis le père est : %s\n", received_data1);
                    close(pipe_fd1[0]);
                }
            }else{
                if (child2 == 0) {
                    //Lecture du contenu du deuxieme pipe  par le fils 2
                    char received_data2[strlen(entry->d_name)];
                    close(pipe_fd2[1]); // Ferme le descripteur d'écriture inutilisé
                    read(pipe_fd2[0], &received_data2, strlen(received_data2));
                    printf("La valeur reçue dans le pipe 2 depuis le père est : %s\n", received_data2);
                    close(pipe_fd2[0]);
                }
            }
        }
        cpt += 1;
    }