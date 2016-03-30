#ifndef __LAB__
#define __LAB__
#include <signal.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

void run_with_parameters();
void run_cmd(char * comandStr, char * logFile);
void run_cmd_async(char * comandStr, char * logFile);

void handle_child(int signal, siginfo_t *siginfo, void *context);
void handle_int(int signal, siginfo_t *siginfo, void *context);
void handle_async_sig(int signal, siginfo_t *siginfo, void *context);
void print_log(char * route, char * msg, int pid);

#endif