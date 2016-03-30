#include "lab2.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

int childIsZombie;
FILE *f;

void run_cmd(char * comandStr, char * logFile) {
	// SIGACTION
	struct sigaction sa;
	struct sigaction sa2;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = &handle_child;
	sa2.sa_flags = SA_SIGINFO;
	sa2.sa_sigaction = &handle_int;
	childIsZombie = 0;


	// PIPE & FILE
	int stdInPipeFd[2];
	int stdOutPipeFd[2];
	int stdErrPipeFd[2];
	if (logFile != NULL) {
		f = fopen(logFile, "w");

		if (f == NULL) {
			fprintf(stderr, "Error opening logger file. Continue without file-logging.\n");
		}
	}
	

	
	if (pipe(stdInPipeFd) < 0) {
		fprintf(stderr, "pipe for child-input error\n");
		exit( EXIT_FAILURE );
	}
	if (pipe(stdOutPipeFd) < 0) {
		close(stdInPipeFd[PIPE_READ]);
		close(stdInPipeFd[PIPE_WRITE]);

		fprintf(stderr, "pipe for child-output error");
		exit( EXIT_FAILURE );
	}
	if (pipe(stdErrPipeFd) < 0) {
		close(stdInPipeFd[PIPE_READ]);
		close(stdInPipeFd[PIPE_WRITE]);
		close(stdOutPipeFd[PIPE_READ]);
		close(stdOutPipeFd[PIPE_WRITE]);

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

		close(stdInPipeFd[PIPE_READ]);
		close(stdOutPipeFd[PIPE_WRITE]);

		struct timeval tv;
		int retval;
		fd_set set;

		time_t t;
		struct tm *timeinfo;
		char buffer[80];
		

		while(1) {
			FD_ZERO(&set);
			FD_SET(stdOutPipeFd[PIPE_READ], &set);
			FD_SET(stdErrPipeFd[PIPE_READ], &set);
			FD_SET(STDIN_FILENO, &set);
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			retval = select(FD_SETSIZE, &set, NULL, NULL, &tv);

			if (retval == 0) {
				time(&t);
				timeinfo = localtime(&t);
				strftime(buffer, 80, "%d-%m-%y %I:%m:%S", timeinfo);
				if (f != NULL) {
					fprintf(f, "%s, NOIO\n", buffer);
				}
			}
			else if (retval < 0) {
				fprintf(stderr, "Select error");
			}
			else {
				int bytes;
				int bytes2;
				int bytes3;
				char data_buffer[1024] = "";
				char data_buffer2[1024] = "";
				char data_buffer3[1024] = "";

				if (FD_ISSET(STDIN_FILENO, &set)) {
					bytes3 = read(STDIN_FILENO, data_buffer3, sizeof(data_buffer3));
					if (strcmp(data_buffer3, "exit") == 0) {
						exit(EXIT_SUCCESS);
					}
					write(stdInPipeFd[PIPE_WRITE], data_buffer3, strlen(data_buffer3) + 1);
				}
				if (FD_ISSET(stdOutPipeFd[PIPE_READ], &set)) {
					bytes = read(stdOutPipeFd[PIPE_READ], data_buffer, sizeof(data_buffer) - 1);
				}
				if (FD_ISSET(stdErrPipeFd[PIPE_READ], &set)) {
					bytes2 = read(stdErrPipeFd[PIPE_READ], data_buffer2, sizeof(data_buffer2));
				}

				time(&t);
				timeinfo = localtime(&t);
				strftime(buffer, 80, "%d-%m-%y %I:%m:%S", timeinfo);

				fprintf(stderr, "<%d> 1< %s\n 2< %s\n >0 %s\n", getpid(), data_buffer, data_buffer2, data_buffer3);
				if (f != NULL) {
					fprintf(f, "PID:%d TIME: %s\n--------------------\n 1< %s\n 2< %s\n >0 %s\n", getpid(), buffer, data_buffer, data_buffer2, data_buffer3);
				}				
				sleep(1);
			}

			if (childIsZombie) {
				if (f != NULL) {
					fclose(f);
				}				
				exit(1);
			}
		}
		close(stdInPipeFd[PIPE_WRITE]);
		close(stdOutPipeFd[PIPE_READ]);
	}
}

void handle_child(int signal, siginfo_t *siginfo, void *context) {
	switch( signal ){
		case SIGCHLD:
			fprintf(stderr, "%i TERMINATED WITH EXIT CODE: %i\n", siginfo->si_pid, siginfo->si_code);
			childIsZombie = 1;
			break;
		default:
			fprintf( stderr, "Нераспознанный сигнал: %d\n", signal);
			return;	
	}
}

void handle_int(int signal, siginfo_t *siginfo, void *context) {
	fprintf(stderr, "%s\n", "WOW");
	switch( signal ){
		case SIGINT:
			if (f != NULL) {
				fclose(f);
			}
			exit(0);
			break;
		default:
			fprintf( stderr, "Нераспознанный сигнал: %d\n", signal);
			return;	
	}
}