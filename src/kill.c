#include "lab1.h"
#include <stdio.h>
#include <signal.h>


void run_kill_mode(int signal_no, pid_t k_pid) {
	kill(k_pid, signal_no);
}