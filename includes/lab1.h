#ifndef __LAB__
#define __LAB__
#include <signal.h>

#define CHILD_SLEEP_TIME 2

void run_with_parameters();

void run_std_mode();
void run_child_mode();
void run_pipe_mode();
void run_posix_mode(int amount_of_signals);
void run_kill_mode(int signal_no, pid_t k_pid);

void handle_std(int signal, siginfo_t *siginfo, void *context);
void handle_child(int signal, siginfo_t *siginfo, void *context);
void handle_queue(int signal, siginfo_t *siginfo, void *context);
void handle_pipe(int signal, siginfo_t *siginfo, void *context);

#endif