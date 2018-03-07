#define _POSIX_SOURCE
 
#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <signal.h>    /* Pour sigXXX */
#include <unistd.h>    /* Pour fork */
#include <stdio.h>     /* Pour printf, perror */
#include <sys/types.h> /* Pour pid_t */
#include <sys/wait.h>  /* Pour waitpid */
 
void fils() {
  sigset_t sigs_new;                              /* Signaux a bloquer */
  sigset_t sigs_old;                              /* Ancien masque */
  sigset_t sigs_bloques;                          /* Signaux bloques */
 
  /* Blocage de tous les signaux */
  sigfillset(&sigs_new);                          /* Tous les signaux */
  sigprocmask(SIG_BLOCK, &sigs_new, &sigs_old);   /* Bloque les signaux */
 
  /* Mise en attente de signaux */
  printf("Fils : je suis insensible a tous les signaux !\n");
  sleep(20);
  printf("Fils : je suis sorti de ma pause\n");
 
  /* Recuperation des signaux bloques */
  if(sigpending(&sigs_bloques) == -1) {
    perror("Erreur lors de la recuperation des signaux bloques ");
    exit(EXIT_FAILURE);
  }
 
  /* Recuperation des signaux bloques */
  if(sigismember(&sigs_bloques, SIGUSR1))
    printf("Fils : j'ai bien recu SIGUSR1 pendant ma pause.\n");
  else
    printf("Fils : je n'ai pas recu SIGUSR1 pendant ma pause.\n");
  if(sigismember(&sigs_bloques, SIGUSR2))
    printf("Fils : j'ai bien recu SIGUSR2 pendant ma pause.\n");
  else
    printf("Fils : je n'ai pas recu SIGUSR2 pendant ma pause.\n");
 
  /* Replacer par defaut */
  sigprocmask(SIG_SETMASK, &sigs_old, 0);
 
  printf("Fils : j'ai termine.\n");
 
  exit(EXIT_SUCCESS);
}
 
int main() {
  pid_t pid;
 
  /* Creation du fils */
  if((pid = fork()) == -1) {
    perror("Erreur lors de la creation du premier fils ");
    exit(EXIT_FAILURE);
  }
  if(pid == 0)
    fils();
  printf("Pere : fils lance.\n");
 
  /* Envoi du signal SIGUSR1 */
  sleep(1);
  printf("Pere : j'envoie SIGUSR1 au fils.\n");
  if(kill(pid, SIGUSR1) == -1) {
    perror("Erreur lors de l'envoi de SIGUSR1 ");
    exit(EXIT_FAILURE);
  }
 
  /* Envoi du signal SIGUSR2 */
  sleep(1);
  printf("Pere : j'envoie SIGUSR2 au fils.\n");
  if(kill(pid, SIGUSR2) == -1) {
    perror("Erreur lors de l'envoi de SIGUSR2 ");
    exit(EXIT_FAILURE);
  }
 
  /* Attente de la fin du fils */
  sleep(1);
  if((waitpid(pid, NULL, 0)) == -1) {
    perror("Erreur lors de l'attente de mon fils ");
    exit(EXIT_FAILURE);
  }
  printf("Pere : mon fils est arrete.\n");
 
  return EXIT_SUCCESS;
}
 