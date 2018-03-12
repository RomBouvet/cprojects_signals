#ifndef __CONN_HANDLER_H__
#define __CONN_HANDLER_H__ 1

#include <unistd.h>

void add_consumer(pid_t);
int find_consumer(pid_t);
int remove_consumer(pid_t);

void sig_conn_handler(int, siginfo_t *, void *);

void conn_handler(int,int[2]);

#endif