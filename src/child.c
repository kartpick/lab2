#include "lab1.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void run_child_mode() {
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = &handle_child;

	pid_t pid = fork();
	if (pid == 0) {
		// Child-процесс
		printf("Child: PID=%d, GID=%d\n", getpid(), getpgid(getpid()));
		printf("Child: Засыпаю на %d секунд...\n", CHILD_SLEEP_TIME);
		sleep(CHILD_SLEEP_TIME);
		printf("Child: BRAINS!!!\n");
	} else if(pid > 0) {
		printf("Parent: PID=%d, GID=%d\n", getpid(), getpgid(getpid()));
		sigaction(SIGCHLD, &sa, NULL);
		int status;
		if (wait(&status) > 0) {
			while(1){}
		} else {
			perror("Failed to handle child-zombie");
			exit( EXIT_FAILURE );
		}
	}
}

void handle_child(int signal, siginfo_t *siginfo, void *context) {
	const char *signal_name;

	switch( signal ){
		case SIGCHLD:
			printf("Parent:Пойман сигнал SIGCHLD\n");
			printf("-------------------\n");
			printf("Child PID: %i\n", siginfo->si_pid);
			printf("User of process: %i\n", siginfo->si_uid);
			printf("Status: %i\n", siginfo->si_status);
			printf("Code: %i\n", siginfo->si_code);
			exit( EXIT_SUCCESS );
			break;
		default:
			fprintf( stderr, "Нераспознанный сигнал: %d\n", signal);
			return;	
	}
}