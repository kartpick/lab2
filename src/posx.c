#include "lab1.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int resieved_signals_count = 0;

void run_posix_mode(int amount_of_signals) {
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = &handle_queue;

	sigset_t set;
	//sigfillset(&set);
	//sigprocmask(SIG_BLOCK, &set, NULL);
	if(sigaction(SIGRTMIN, &sa, NULL) == -1){
		perror("Ошибка: не удается обработать сигнал SIGRTMIN");
	}

	int j = 0;
	for (j = 0; j < SIGRTMAX; j++) {
		sigaction(SIGRTMIN + j, &sa, NULL);
	}

	pid_t pid = fork();
	if (pid == 0) {
		// Child-процесс
		int i = 0;

		int range = 1 + SIGRTMAX - SIGRTMIN;
		int buckets = RAND_MAX / range;
		int limit = buckets * range;

		for (i = 0; i < amount_of_signals; ++i) {
			union sigval value;

			int r_signal;
			do {
				r_signal = rand();
			} while (r_signal >= limit);

			r_signal = SIGRTMIN + (r_signal / buckets);

			value.sival_int = rand();





			sigqueue(getppid(), r_signal, value);
			fprintf(stderr, "CHILD: N=%i | MYPID=%i | PPID=%i | POSIXSIGNALNO=%i | VALUE=%i\n", 
				i, getpid(), getppid(), r_signal, value.sival_int);
		}
	} else if(pid > 0) {
		printf("Parent: PID=%d, GID=%d\n", getpid(), getpgid(getpid()));
		sleep(10);
		
		int status;
		if (wait(&status) > 0) {
			exit( EXIT_SUCCESS );
		} else {
			perror("Failed to handle child-zombie");
			exit( EXIT_FAILURE );
		}
	}
}

void handle_queue(int signal, siginfo_t *siginfo, void *context) {
	const char *signal_name;

	fprintf(stderr, "PARENT: N=%i | MYPID=%i | PPID=%i | POSIXSIGNALNO=%i | VALUE=%i\n", 
		resieved_signals_count, siginfo->si_pid, getpid(), signal, siginfo->si_value.sival_int);
	resieved_signals_count++;
}