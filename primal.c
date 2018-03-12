#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "conn_handler.h"

pid_t ch_pid,th_pid;
int stop=0;

void sig_handler(int s, siginfo_t *siginfo, void *context){
    if(s==SIGINT){
        kill(SIGINT,ch_pid);
        kill(SIGINT,th_pid);
        stop=1;
        printf("__| CLOSING PRODUCER |__\n");
    }
}

int main(int argc,char *argv[]){
    int X,N,tube[2],c_read;
    struct sigaction action;
    char buffer[255];

    sigemptyset(&action.sa_mask);
    action.sa_sigaction=&sig_handler;
    sigaction(SIGINT, &action, NULL);

    if(argc!=5){
        fprintf(stderr,"__| NEED 4 PARAMETERS - SEE README FOR MORE INFORMATIONS |__\n");
        exit(EXIT_FAILURE);
    }
    if(((X=atoi(argv[1]))==0)||((N=atoi(argv[2]))==0)){
        fprintf(stderr,"__| X AND N MUST BE INTEGERS GREATER THAN 0 |__\n");
        exit(EXIT_FAILURE);
    }
    char *tasks_pipe_name=argv[3],*results_pipe_name=argv[4];

    if(pipe(tube)==-1){
        fprintf(stderr,"__| PIPE ERROR |__\n");
        exit(EXIT_FAILURE);
    }

    if((ch_pid=fork())<0){
        fprintf(stderr,"__| FORK ERROR (CONN_HANDLER) |__\n");
        exit(EXIT_FAILURE);
    }else if(ch_pid==0){
        conn_handler(N,tube);
        exit(EXIT_SUCCESS);
    }else{
        if((th_pid=fork())<0){
            fprintf(stderr,"__| FORK ERROR (TASKS_HANDLER) |__\n");
            exit(EXIT_FAILURE);
        }else if(th_pid==0){
            //tasks_handler();
            printf("__| NOT IMPLEMENTED YET |__\n");
            exit(EXIT_SUCCESS);
        }else{
            while(1){
                if(read(tube[0],buffer,255*sizeof(char))==-1 && !stop){
                    fprintf(stderr,"__| READ ERROR |__\n");
                    exit(EXIT_FAILURE);
                }
                printf("%s",buffer);
                fflush(stdout);

                if(stop){
                    exit(EXIT_SUCCESS);
                }
            }
        }
    }
    return EXIT_SUCCESS;
}