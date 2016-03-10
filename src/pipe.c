#include "lab1.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void run_pipe_mode() {
	int fd[2], bytesRead;
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = &handle_pipe;

	// Открыть канал
	if (pipe(fd) < 0) {
		fprintf(stderr, "Ошибка PIPE-потока\n" );
		exit( EXIT_FAILURE );
	}


	if(sigaction(SIGPIPE, &sa, NULL) == -1){
		perror("Ошибка: не удается обработать сигнал SIGPIPE");
		exit( EXIT_FAILURE );
	}

	pid_t pid = fork();
	if (pid == 0) {
		// Child-процесс
		close(fd[1]);
		close(fd[0]);
		exit(0);
	} else if(pid > 0) {
		close(fd[0]);
		sleep(1);
		write(fd[1], "hello", 12);

		int status;
		if (wait(&status) > 0) {
			exit( EXIT_SUCCESS );
		} else {
			perror("Failed to handle child-zombie");
			exit( EXIT_FAILURE );
		}
	}

	while(1){}
}

void handle_pipe(int signal, siginfo_t *siginfo, void *context) {
	const char *signal_name;

	switch( signal ){
		case SIGPIPE:
			printf("Пойман SIGPIPE-сигнал( %i )\n", signal);
			break;
		default:
			fprintf( stderr, "Нераспознанный сигнал: %d\n", signal);
			return;	
	}
}