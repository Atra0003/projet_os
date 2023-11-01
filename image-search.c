#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>



int image_search(){

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



    pid_t child1, child2;
    int pipe_fd1[2];  // Tableau pour les descripteurs du pipe
    int pipe_fd2[2];

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

        if(*shared_data > received_data1){
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
        int data_to_send = 1;
        write(pipe_fd2[1], &data_to_send, sizeof(data_to_send));
        close(pipe_fd2[1]);

        int received_data2;
        close(pipe_fd1[1]); // Ferme le descripteur d'écriture inutilisé
        read(pipe_fd1[0], &received_data2, sizeof(received_data2));
        printf("La valeur reçue dans le pipe 1 depuis le fils 2 est : %d\n", received_data2);
        close(pipe_fd1[0]);


        if(*shared_data > received_data2){
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
