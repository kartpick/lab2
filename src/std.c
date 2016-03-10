#include "lab1.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Файл для работы с режимом STD
void handle_std(int signal, siginfo_t *siginfo, void *context) {
	const char *signal_name;

	// Определение сигнала
	switch( signal ){
		case SIGHUP:
			signal_name = "SIGHUP";
			break;
		case SIGUSR1:
			signal_name = "SIGUSR1";
			break;
		case SIGUSR2:
			signal_name = "SIGUSR2";
			break;
		default:
			fprintf( stderr, "Нераспознанный сигнал: %d\n", signal);
			return;	
	}
	pid_t signal_id = siginfo->si_pid;
	fprintf( stderr, "Пойман сигнал %s(%d) от процесса с PID=%i( GID=%i )\n", signal_name, signal, signal_id, getpgid(signal_id));
	exit( EXIT_SUCCESS );
}

void run_std_mode() {
	struct sigaction sa;
	sa.sa_sigaction = &handle_std;

	printf("Pid программы %i\n", getpid());

	sa.sa_flags = SA_SIGINFO;

	// Блокируем все сигналы во время обработки
	sigfillset(&sa.sa_mask);

	if (sigaction(SIGHUP, &sa, NULL) == -1) {
		perror("Ошибка: не удается обработать сигнал SIGHUP");
	}
	if (sigaction(SIGUSR1, &sa, NULL) == -1) {
		perror("Ошибка: не удается обработать сигнал SIGUSR1");
	}
	if (sigaction(SIGUSR2, &sa, NULL) == -1) {
		perror("Ошибка: не удается обработать сигнал SIGUSR2");
	}

	printf("Ожидание...\n");
	while(1) sleep(1);
} 