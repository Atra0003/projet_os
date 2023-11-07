#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdint.h>



int image_search(int argc, char* argv[]){
    (void)argc;

    DIR *directory;
    struct dirent *entry;
    // Spécifiez le chemin du dossier que vous souhaitez lister
    const char *path = argv[2];
    directory = opendir(path);
    if (directory == NULL) {
        perror("Erreur lors de l'ouverture du dossier");
        return 1;
    }

    char chaine[] = "BONJOUR";
    pid_t child1, child2;
    int pipe_fd1[2];
    int pipe_fd2[2];

    // Crée les deux pipes anonymes
    if (pipe(pipe_fd1) == -1 || pipe(pipe_fd2) == -1) {
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
        close(pipe_fd1[1]);  // Ferme le descripteur d'écriture inutilisé
        
        char received_data[999];
        while (1) {
            read(pipe_fd1[0], &received_data, sizeof(received_data));
            if (strcmp(received_data, "Stop")){
                break; // Sort de la boucle si la valeur reçue est -1
            }
            printf("Je suis le premier processus fils (PID : %d) et j'ai reçu : %s\n", getpid(), received_data);
        }
        close(pipe_fd1[0]);
    } else {
        // Crée le deuxième processus fils
        child2 = fork();

        if (child2 < 0) {
            perror("Erreur lors de la création du deuxième processus fils");
            return 1;
        }

        if (child2 == 0) {
            // Code du deuxième processus fils
            close(pipe_fd2[1]);  // Ferme le descripteur d'écriture inutilisé
            
            char received_data[999];
            while (1) {
                read(pipe_fd2[0], &received_data, sizeof(received_data));
                if (strcmp(received_data, "Stop")) {
                    break; // Sort de la boucle si la valeur reçue est -1
                }
                printf("Je suis le deuxième processus fils (PID : %d) et j'ai reçu : %s\n", getpid(), received_data);
            }
            close(pipe_fd2[0]);
        } else {
            // Code du processus parent
           

            close(pipe_fd1[0]);  // Ferme le descripteur de lecture inutilisé
            close(pipe_fd2[0]);

            
            int cpt=0;
            while((entry = readdir(directory)) != NULL) {
                if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    
                    if (cpt % 2 == 0) {
                        write(pipe_fd1[1], entry->d_name, strlen(entry->d_name));
                        //printf("Le nom est : %s\n", entry->d_name);
                    } else {
                        write(pipe_fd2[1], entry->d_name, strlen(entry->d_name));
                        //printf("Le nom est : %s\n", entry->d_name);
                    }
                }
                printf("Le compteur est : %d\n", cpt);
                cpt += 1;
            }


            //for (int i = 0; i < 7; i++) {
                //if (i % 2 == 0) {
                //    write(pipe_fd1[1], &chaine[i], sizeof(chaine[i]));
                //} else {
                //    write(pipe_fd2[1], &chaine[i], sizeof(chaine[i]));
                //}
            //}

            // Envoyer -1 pour signaler aux fils que la communication est terminée
            char data_to_send = "stop";
            write(pipe_fd1[1], &data_to_send, sizeof(data_to_send));
            write(pipe_fd2[1], &data_to_send, sizeof(data_to_send));
            printf("\nFin de lecriture\n");

            close(pipe_fd1[1]);
            close(pipe_fd2[1]);

            // Attendez que les deux processus fils se terminent
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}

