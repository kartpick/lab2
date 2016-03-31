#include "lab2.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>

FILE *f;
// PIPE & FILE
int stdInPipeFd[2];
int stdOutPipeFd[2];
int stdErrPipeFd[2];
int isIO;
int childIsZombie;

char data_buffer[1024] = "";

void run_cmd_async(char * comandStr, char * logFile) {
	

	// SIGACTION
	struct sigaction sa;
	struct sigaction sa2;
	struct sigaction sa3;
	struct sigaction sa4;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = &handle_child;
	sa2.sa_flags = SA_SIGINFO;
	sa2.sa_sigaction = &handle_int;
	sa3.sa_flags = SA_SIGINFO;
	sa3.sa_sigaction = &handle_async_sig;
	sa4.sa_flags = SA_SIGINFO;
	sa4.sa_sigaction = &handle_alrm;
	isIO = 0;
	childIsZombie = 0;

	// FILE
	if (logFile != NULL) {
		f = fopen(logFile, "w");

		if (f == NULL) {
			fprintf(stderr, "Error opening logger file. Continue without file-logging.\n");
		}
	}
	
	// PIPES OPEN
	if (pipe(stdInPipeFd) < 0) {
		fprintf(stderr, "pipe for child-input error\n");
		exit( EXIT_FAILURE );
	}
	if (pipe(stdOutPipeFd) < 0) {
		fprintf(stderr, "pipe for child-output error");
		exit( EXIT_FAILURE );
	}
	if (pipe(stdErrPipeFd) < 0) {
		fprintf(stderr, "pipe for child-errput error");
		exit( EXIT_FAILURE );
	}

	pid_t pid = fork();
	if (pid == 0) {
		// Child-процесс
		if (dup2(stdInPipeFd[PIPE_READ], STDIN_FILENO) == -1) {
			fprintf(stderr, "Redirect stdin error");
			exit( EXIT_FAILURE );
		}

		if (dup2(stdOutPipeFd[PIPE_WRITE], STDOUT_FILENO) == -1) {
			fprintf(stderr, "Redirect stdout error");
			exit( EXIT_FAILURE );
		}

		if (dup2(stdErrPipeFd[PIPE_WRITE], STDERR_FILENO) == -1) {
			fprintf(stderr, "Redirect stderr error");
			exit( EXIT_FAILURE );
		}

		close(stdInPipeFd[PIPE_READ]);
		close(stdInPipeFd[PIPE_WRITE]);
		close(stdOutPipeFd[PIPE_READ]);
		close(stdOutPipeFd[PIPE_WRITE]);
		close(stdErrPipeFd[PIPE_READ]);
		close(stdErrPipeFd[PIPE_WRITE]);


		// Заотовка параметров для подмены процесса
		char * program = strtok(comandStr, " ");
		char * p_args = strtok(NULL, "");
		char * argv[] = {program, p_args, NULL};
		char * envp[] = {NULL};
		int ret = execve(program, argv, envp);

		if (ret == -1) {
			fprintf(stderr, "Exec error in child proccess");
			exit( EXIT_FAILURE );
		}
	} else if(pid > 0) {
		sigaction(SIGCHLD, &sa, NULL);
		sigaction(SIGINT, &sa2, NULL);
		sigaction(SIGRTMIN + 1, &sa3, NULL);
		sigaction(SIGRTMIN + 2, &sa3, NULL);
		sigaction(SIGRTMIN + 3, &sa3, NULL);
		sigaction(SIGALRM, &sa4, NULL);

		fcntl(stdOutPipeFd[PIPE_READ], F_SETSIG, SIGRTMIN + 1);
		fcntl(stdOutPipeFd[PIPE_READ], F_SETOWN, getpid());
		fcntl(stdOutPipeFd[PIPE_READ], F_SETFL, O_ASYNC | O_NONBLOCK);


		fcntl(stdErrPipeFd[PIPE_READ], F_SETSIG, SIGRTMIN + 2);
		fcntl(stdErrPipeFd[PIPE_READ], F_SETOWN, getpid());
		fcntl(stdErrPipeFd[PIPE_READ], F_SETFL, O_ASYNC);


		fcntl(STDIN_FILENO, F_SETSIG, SIGRTMIN + 3);
		fcntl(STDIN_FILENO, F_SETOWN, getpid());
		fcntl(STDIN_FILENO, F_SETFL, O_ASYNC | O_NONBLOCK);

		alarm(1);

		while(1) {
			if (childIsZombie) {
				close(stdInPipeFd[PIPE_WRITE]);
				close(stdOutPipeFd[PIPE_READ]);
				exit(EXIT_SUCCESS);
			}
		}
		
	}
}

void handle_async_sig(int signal, siginfo_t *siginfo, void *context) {
	int bytes;
	data_buffer[0] = 0;
	if(signal == SIGRTMIN + 1) { // FD OUTPUT READ
		isIO = 1;
		bytes = read(stdOutPipeFd[PIPE_READ], data_buffer, sizeof(data_buffer) - 1);
		data_buffer[bytes] = '\0';
		print_log("1<", data_buffer, getpid());
	}
	else if (signal == SIGRTMIN + 2) {// FD ERRPUT READ
		isIO = 1;
		bytes = read(stdErrPipeFd[PIPE_READ], data_buffer, sizeof(data_buffer) - 1);
		data_buffer[bytes] = '\0';
		print_log("2<", data_buffer, getpid());
	}
	else if (signal == SIGRTMIN + 3) {// FD INPUT WRITE
		bytes = read(STDIN_FILENO, data_buffer, sizeof(data_buffer) - 1);
		data_buffer[bytes] = '\0';
		if (bytes > 0) {
			isIO = 1;
			print_log(">0", data_buffer, getpid());

			// SEND TO CHILD
			if (strcmp(data_buffer, "exit\n") == 0) {
				childIsZombie = 1;
			}
			write(stdInPipeFd[PIPE_WRITE], data_buffer, strlen(data_buffer));
		}
	}
	else {
		fprintf( stderr, "Нераспознанный сигнал: %d\n", signal);
		return;
	}
}

void print_log(char * route, char * msg, int pid) {
	// FORM TIME
	time_t t;
	struct tm *timeinfo;
	char buffer[80];

	time(&t);
	timeinfo = localtime(&t);
	strftime(buffer, 80, "%d-%m-%y %I:%m:%S", timeinfo);


	fprintf(stderr, "<%d> %s %s", pid, route, msg);
	if (f != NULL) {
		fprintf(f, "PID:%d TIME: %s\n--------------------\n%s %s\n", pid, buffer, route, msg);
	}
				
}

void handle_alrm(int signal, siginfo_t *siginfo, void *context) {
	// FORM TIME
	time_t t;
	struct tm *timeinfo;
	char buffer[80];

	if (isIO == 0) {
		time(&t);
		timeinfo = localtime(&t);
		strftime(buffer, 80, "%d-%m-%y %I:%m:%S", timeinfo);

		if (f != NULL) {
			fprintf(f, "%s, NOIO\n", buffer);
		}
	}
	else { isIO = 0; }

	alarm(1);
}