#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MAX_CONSUMERS 5
#define AUTHORIZED 1
#define REFUSED 0

pid_t consumers[MAX_CONSUMERS];
int conn_nb;
int stop;

void add_consumer(pid_t pid){
    consumers[conn_nb]=pid;
    ++conn_nb;
}

int find_consumer(pid_t pid){
    int i;
    for(i=0;i<conn_nb;i++){
        fflush(stdout);
        if(consumers[i]==pid)
            return i;
    }
    return -1;
}

int remove_consumer(pid_t pid){
    int i,tmp;
    tmp=find_consumer(pid);
    if(tmp>=0){
        for(i=tmp;i<MAX_CONSUMERS-1;i++){
            consumers[i]=consumers[i+1];
        }  
        --conn_nb;
        return 1;
    }
    return 0;
}

void sig_handler(int n, siginfo_t *siginfo, void *context){
    union sigval sv;
    int i;
    if(n==SIGRTMIN){
        printf("__| A NEW CONSUMER IS TRYING TO CONNECT ( PID : %d ) |__\n",siginfo->si_pid);
        fflush(stdout);
        if(conn_nb<MAX_CONSUMERS){
            add_consumer(siginfo->si_pid);
            printf("__| CONNECTED |__\n");
            fflush(stdout);
            sv.sival_int=AUTHORIZED;
        }else{
            printf("__| ERROR : NO EMPTY SLOT LEFT |__\n");
            fflush(stdout);
            sv.sival_int=REFUSED;
        }
        sigqueue(siginfo->si_pid,SIGRTMIN,sv);
    }else if(n==SIGRTMIN+1){
        if(!remove_consumer(siginfo->si_pid)){
            printf("__| GIVEN CONSUMER WASN'T CONNECTED ( PID : %d ) |__",siginfo->si_pid);
            fflush(stdout);
        }
        else{
            printf("__| A CONSUMER DISCONNECTED ( PID : %d ) |__\n",siginfo->si_pid);
            fflush(stdout);
        }
    }else if(n==SIGINT){
        stop=1;
        for(i=0;i<conn_nb;i++){
            kill(consumers[i],SIGRTMIN+1);
        }
        printf("__| CONNEXION HANDLER CLOSED |__\n");
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[]){
    struct sigaction action;
    conn_nb=0;
    stop=0;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction=&sig_handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGRTMIN, &action, NULL);
    sigaction(SIGRTMIN+1, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    printf("__| CONNEXION HANDLER STARTED ( PID : %d ) |__\n",getpid());
    while(!stop);
    return EXIT_SUCCESS;
}