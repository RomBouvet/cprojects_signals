#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

pid_t conn_handler;
int stop=0;

void sig_handler(int n, siginfo_t *siginfo, void *context){
    int accepted=(int)siginfo->si_value.sival_int;
    if(n==SIGRTMIN){
        if(!accepted){
            fprintf(stderr,"__| NO EMPTY SLOT LEFT |__\n");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }else{
            printf("__| CONNECTED |__\n");
            fflush(stdout);
        }
    }else if(n==SIGRTMIN+1){
        stop=1;
        printf("__| HANDLER LEFT |__\n");
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }else if(n==SIGINT){
        stop=1;
        kill(conn_handler,SIGRTMIN+1);
        printf("__| GOODBYE |__\n");
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char* argv[]){
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction=&sig_handler;
    action.sa_flags = SA_SIGINFO;

    sigaction(SIGRTMIN, &action, NULL);
    sigaction(SIGRTMIN+1, &action, NULL);
    sigaction(SIGINT, &action, NULL); 
    conn_handler=(pid_t) atoi(argv[1]);
    if(kill(conn_handler, SIGRTMIN) == -1) {
        perror("Erreur lors de l'envoi de SIGUSR2 ");
        exit(EXIT_FAILURE);
    }
    while(!stop);
    return EXIT_SUCCESS;    
}