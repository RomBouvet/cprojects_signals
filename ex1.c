#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int stop=0,count=0;

void handler(int numSig){
    if(numSig==SIGALRM){
        stop=1;
    } 
    if(numSig==SIGINT){
        count++;
    }
}

int main(int argc,char *argv[]){
    int t;
    struct sigaction action;
    action.sa_handler = &handler;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGALRM, &action, NULL);
    alarm(10);
    while(!stop){
        t=sleep(1);
        while((t=sleep(t)));
        printf(".");
        fflush(stdout);
    }
    printf("HAHA YES ! %d SIGINT recus\n",count);
    return 0;    
}   