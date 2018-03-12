#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "conn_handler.h"

#define AUTHORIZED 1
#define REFUSED 0

int max_consumers,conn_nb,*tube;
pid_t *consumers;

void add_consumer(pid_t pid){
    consumers[conn_nb]=pid;
    ++conn_nb;
}

int find_consumer(pid_t pid){
    int i;
    for(i=0;i<conn_nb;i++){
        if(consumers[i]==pid)
            return i;
    }
    return -1;
}

int remove_consumer(pid_t pid){
    int i,tmp;
    tmp=find_consumer(pid);
    if(tmp>=0){
        for(i=tmp;i<max_consumers-1;i++){
            consumers[i]=consumers[i+1];
        }  
        --conn_nb;
        return 1;
    }
    return 0;
}

void send_msg(char *str){
    if(write(tube[1],str,(strlen(str)+1)*sizeof(char))==-1){
        fprintf(stderr,"__| WRITE ERROR (CONN_HANDLER) |__\n");
        exit(EXIT_FAILURE);
    }
}

void sig_conn_handler(int n, siginfo_t *siginfo, void *context){
    union sigval sv;
    char buffer[254];
    int i;
    if(n==SIGRTMIN){
        sprintf(buffer,"__| A NEW CONSUMER IS TRYING TO CONNECT ( PID : %d ) |__\n",siginfo->si_pid);
        send_msg(buffer);
        if(conn_nb<max_consumers){
            add_consumer(siginfo->si_pid);
            sleep(1);
            sprintf(buffer,"__| CONNECTED (%d/%d) |__\n",conn_nb,max_consumers);
            send_msg(buffer);
            sv.sival_int=AUTHORIZED;
        }else{
            sprintf(buffer,"__| ERROR : NO EMPTY SLOT LEFT (%d/%d) |__\n",conn_nb,max_consumers);
            send_msg(buffer);
            sv.sival_int=REFUSED;
        }
        sigqueue(siginfo->si_pid,SIGRTMIN,sv);
    }else if(n==SIGRTMIN+1){
        if(!remove_consumer(siginfo->si_pid)){
            sprintf(buffer,"__| GIVEN CONSUMER WASN'T CONNECTED ( PID : %d ) |__",siginfo->si_pid);
            send_msg(buffer);
        }
        else{
            sprintf(buffer,"__| A CONSUMER DISCONNECTED ( PID : %d ) |__\n",siginfo->si_pid);
            send_msg(buffer);
        }
    }else if(n==SIGINT){
        for(i=0;i<conn_nb;i++){
            kill(consumers[i],SIGRTMIN+1);
        }
        send_msg("__| CONNEXION HANDLER CLOSED |__\n");
        free(consumers);
        sleep(1);
        exit(EXIT_SUCCESS);
    }
}

void conn_handler(int N,int t[2]){
    struct sigaction action;
    char buffer[255];
    max_consumers=N;
    tube=t;
    consumers=(int*)malloc(max_consumers*sizeof(int));
    conn_nb=0;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction=&sig_conn_handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGRTMIN, &action, NULL);
    sigaction(SIGRTMIN+1, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    sprintf(buffer,"__| CONNEXION HANDLER STARTED ( PID : %d ) |__\n",getpid());
    send_msg(buffer);
    while(1);
}