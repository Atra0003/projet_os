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
#include <sys/mman.h>



#define READ 0
#define WRITE 1
#define TAB_SIZE 100000



void write_pipe(int pipe, const void *path, size_t sizePath) {
    ssize_t return_Write = write(pipe, path, sizePath);
    if (return_Write < 0) {
        perror("Erreur d'écriture");
        exit(1);
    }
}


char *read_Pipe(int pipe, char *buffer, size_t sizePath) { // ne fonction pas pour le moment
    ssize_t return_read = read(pipe, buffer, sizeof(char)*sizePath);
    if (return_read < 0) {
        perror("Erreur de lecture");
        exit(1);
    }
    buffer[return_read] = '\0';
    //printf("Reçu via le pipe nommé 1: img/%s\n", buffer);
    return buffer;
}


int img_dist(char *image1, char *image2) {
    pid_t childImgDist;
    
    childImgDist = fork();
    if(childImgDist <0) {
        perror("Échec de la création du processus fils");
        return 1;
    }

    if(childImgDist == 0){
        char *args2[] = {"./img-dist/img-dist", image1, image2, NULL};
        execvp(args2[0], args2);
    } else {
        int status;
        wait(&status);
        
        if(WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            return exit_status;
        }
    }
    return 65;
}


void *memoire_partage(size_t size){
    const int protection = PROT_READ | PROT_WRITE;
    const int visibility = MAP_SHARED | MAP_ANONYMOUS;
    void *ptr = mmap(NULL, size, protection, visibility, -1, 0);
    if(ptr == MAP_FAILED){
        perror("MMAP");
        exit(1);
    }
    return ptr;
}


int appel(int argc, char* argv[]) {
    (void)argc;

    pid_t child1, child2;
    
    int *tab = (int*)memoire_partage(sizeof(int) * TAB_SIZE);
    char *mostSimilarImage = (char *)memoire_partage(sizeof(int) * TAB_SIZE);
    *tab = 65;

    

    // pipe
    int mypipe1[2];
    if (pipe(mypipe1) < 0) {
        perror("Échec de la création du pipe");
        return 1;
    }

    int mypipe2[2];
    if (pipe(mypipe2) < 0) {
        perror("Échec de la création du pipe");
        return 1;    
    }
    // Crée le premier processus fils
    child1 = fork();

    if (child1 < 0) {
        perror("Échec de la création du premier processus fils");
        return 1;
    }

    if (child1 == 0) {
        // Code exécuté par le premier fils
        close(mypipe1[WRITE]);
        char buffer[999];
        char size[10];
        while (read(mypipe1[READ], size, sizeof(char)) > 0) {
            
            int intSize = atoi(size);
            if(intSize == 1){
                close(mypipe1[READ]);
                exit(0);
            }
            char *buf = read_Pipe(mypipe1[READ], buffer, intSize);
            char dest[50];
            strcpy(dest, "img/");
            strcat(dest, buf);
            int return_dist = img_dist(argv[1], dest);
            if(return_dist < *tab){
                *tab = return_dist;
                strcpy(mostSimilarImage, dest);
                //*mostSimilarImage = &dest;
            }
        }
    } else {
        child2 = fork();

        if (child2 < 0) {
            perror("Échec de la création du deuxième processus fils");
            return 1;
        }

        if (child2 == 0) {
            // Code exécuté par le deuxième fils
            close(mypipe2[WRITE]);
            char buffer[999];
            char size[10];

            while(read(mypipe2[READ], size, sizeof(char)) > 0) {

                int intSize = atoi(size);
                if(intSize == 1){
                    close(mypipe2[READ]);
                    exit(0);
                }
                char *buf = read_Pipe(mypipe2[READ], buffer, intSize);
                char dest[50];
                strcpy(dest, "img/");
                strcat(dest, buf);
                int return_dist = img_dist(argv[1], dest);
                if(return_dist < *tab){
                    *tab = return_dist;
                    strcpy(mostSimilarImage, dest);
                }
            }
        } else {
            // Code exécuté par le processus parent
            close(mypipe1[READ]);
            close(mypipe2[READ]);

            int cpt=0;
            DIR *directory;
            struct dirent *entry;

            // Spécifiez le chemin du dossier que vous souhaitez lister
            const char *path = argv[2];

            directory = opendir(path);

            if (directory == NULL) {
                perror("Erreur lors de l'ouverture du dossier");
                return 1;
            }

            while ((entry = readdir(directory)) != NULL) {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    int charCount = strlen(entry->d_name);

                    char charCountStr[10];  // Assurez-vous que la taille est suffisante
                    snprintf(charCountStr, sizeof(charCountStr), "%d", charCount);
                    if(cpt % 2 == 0) {
                        write_pipe(mypipe1[WRITE], charCountStr, strlen(charCountStr)); 
                        write_pipe(mypipe1[WRITE], entry->d_name, strlen(entry->d_name));
                    } else {
                        write_pipe(mypipe2[WRITE], charCountStr, strlen(charCountStr)); 
                        write_pipe(mypipe2[WRITE], entry->d_name, strlen(entry->d_name));
                    }
                    cpt++;
                }
            }
            write_pipe(mypipe1[WRITE], "1", strlen("1"));
            write_pipe(mypipe2[WRITE], "1", strlen("1"));

            close(mypipe1[WRITE]);
            close(mypipe2[WRITE]);
            
            int status1, status2;
            waitpid(child1, &status1, 0);
            waitpid(child2, &status2, 0);
            
            printf("Most similar image found: '%s' with a distance of %d.\n", mostSimilarImage, *tab);

            closedir(directory);
        }
    munmap(tab, sizeof(int) * TAB_SIZE);
    }
    return 0;
}
