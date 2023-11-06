#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#define READ 0
#define WRITE 1


int appel(int argc, char* argv[]) {
    (void)argc;

    pid_t child1, child2;
    


    // pipe
    // Crée le premier processus fils
    child1 = fork();

    // Vérifie si la création du premier fils a réussi
    if (child1 < 0) {
        perror("Échec de la création du premier processus fils");
        return 1;
    }

    if (child1 == 0) {
        // Code exécuté par le premier fils
        printf("Je suis le premier processus fils (PID %d).\n", getpid());
        char myfifo[] = "/tmp/myfifo";
        int pipe = open(myfifo, O_RDONLY);
        if(pipe < 0) {
            perror(" Erreur lors de l'ouverture du pipe nommé :");
            exit(1);
        }

        char buffer[999];
        while(read(pipe, buffer, sizeof(char)*6) > 0) {
            printf("Reçu via le pipe nommé 1: img/%s\n", buffer);
        }
        close (pipe);

    } else {

        child2 = fork();

        // Vérifie si la création du deuxième fils a réussi
        if (child2 < 0) {
            perror("Échec de la création du deuxième processus fils");
            return 1;
        }

        if (child2 == 0) {
            // Code exécuté par le deuxième fils
            printf("Je suis le deuxième processus fils (PID %d).\n", getpid());
            char myfifo[] = "/tmp/myfifo";
            int pipe = open(myfifo, O_RDONLY);
            if(pipe < 0) {
                perror(" Erreur lors de l'ouverture du pipe nommé :");
                exit(1);
            }

            char buffer[999];
            while(read(pipe, buffer, sizeof(char)*6) > 0) {
                printf("Reçu via le pipe nommé 2: img/%s\n", buffer);
            }
            close (pipe);


        } else {
            // Code exécuté par le processus parent
            printf("Je suis le processus parent (PID %d).\n", getpid());
            
            DIR *directory;
            struct dirent *entry;

            // Spécifiez le chemin du dossier que vous souhaitez lister
            const char *path = argv[2];

            directory = opendir(path);

            if (directory == NULL) {
                perror("Erreur lors de l'ouverture du dossier");
                return 1;
            }
            
            // Création du pipe nommé
            const char myfifo[] = "/tmp/myfifo";
            mkfifo(myfifo, 0666);
            int pipe = open(myfifo, O_WRONLY);
            //int pipe2 = open(myfifo, O_WRONLY);

            int cpt=0;

            while((entry = readdir(directory)) != NULL) {
                if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    //printf("taille = %ld", sizeof(entry->d_name));
                    write(pipe, entry->d_name, strlen(entry->d_name));
                    write(pipe, "\n", strlen("\n"));
                }
            }
            
            closedir(directory);
            close(pipe);
            unlink(myfifo);

        }
    }

    return 0;
}


