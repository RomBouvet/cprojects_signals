#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MAX_CONSUMERS 5


void pipe_stopper(char *pipe_name) {
    /* Suppression du tube */
    if(unlink(pipe_name) == -1) {
        if(errno != ENOENT) {
            fprintf(stderr, "Erreur lors de la suppresion du tube '%s'", pipe_name);
            perror(" ");
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc,char *argv[]){
    int fd, i;

    if(argc!=3){
        fprintf("__| ERROR : NEED 2 PARAMETERS |__\n");
        exit(EXIT_FAILURE);
    }

    char *tasks_pipe_name=argv[1];
    char *results_pipe_name=argv[2];

    /* Enregistrement de la procedure de fin */
    if(atexit(pipe_stopper) == -1) {
        perror("Erreur lors de l'enregistrement d'une procedure ");
        exit(EXIT_FAILURE);
    }

    /* Creation du tube */
    if(mkfifo(tasks_pipe_name, S_IRUSR | S_IWUSR) == -1) {
        if(errno != EEXIST) {
            fprintf(stderr, "Erreur lors de la creation du tube '%s'", tasks_pipe_name);
            perror(" ");
            exit(EXIT_FAILURE);
        }
        else
            fprintf(stderr, "Le tube '%s' existe deja.\n", tasks_pipe_name);
    }

    /* Ouverture du tube */
    if((fd = open(tasks_pipe_name, O_WRONLY)) == -1) {
        fprintf(stderr, "Erreur lors de l'ouverture du tube '%s'", tasks_pipe_name);
        perror(" ");
        exit(EXIT_FAILURE);
    }
    printf("Serveur pret.\n");
    sleep(1);

        /* Ecriture de 5 entiers */
        for(i = 0; i < 5; i++) {
            if(write(fd, &i, sizeof(int)) == -1) {
                perror("Erreur lors de l'ecriture d'un entier dans le tube ");
                exit(EXIT_FAILURE);
            }
        } 

     

    /* Fermeture du tube */
    if(close(fd) == -1) {
        perror("Erreur lors de la fermeture du tube ");
        exit(EXIT_FAILURE);
    }

    printf("Serveur termine.\n");

    return EXIT_SUCCESS;
}