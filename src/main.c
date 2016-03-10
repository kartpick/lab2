#include <lab1.h>
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

struct globalArgs_t {
	char *mode; // mode=Режим
	int amount; // Работает вместе с mode=posix
	int signalName;
	pid_t pidVal;
} globalArgs;

static const struct option longOpts[] = {
	{"mode", required_argument, NULL, 0},
	{"amount", required_argument, NULL, 0},
	{"signal", required_argument, NULL, 0},
	{"pid", required_argument, NULL, 0}
};

int main(int argc, char *argv[]) {
	int opt = 0;
	int longIndex = 0;

	// Инициализация параметров перед использованием
	globalArgs.mode = NULL;
	globalArgs.amount = 0;
	globalArgs.signalName = 0;
	globalArgs.pidVal = 0;

	opt = getopt_long( argc, argv, "", longOpts, &longIndex );
	while (opt != -1) {
		if ( strcmp( "mode", longOpts[longIndex].name ) == 0 ) {
			globalArgs.mode = optarg;
		}
		else if ( strcmp( "signal", longOpts[longIndex].name ) == 0 ) {
			globalArgs.signalName = atoi(optarg);
		}
		else if ( strcmp( "amount", longOpts[longIndex].name ) == 0 ) {
			globalArgs.amount = atoi(optarg);
		}
		else if ( strcmp( "pid", longOpts[longIndex].name ) == 0 ) {
			globalArgs.pidVal = atoi(optarg);
		}
		opt = getopt_long( argc, argv, "", longOpts, &longIndex);
	}
	printf("------------GetOpt------------------\n");
	printf("Режим программы: %s\n", globalArgs.mode );
	printf("Заданный сигнал(для режима 'kill'): %i\n", globalArgs.signalName );
	printf("Количество случайных сигналов(для режима 'posix'): %d\n", globalArgs.amount );
	printf("PID(для режима 'kill'): %i\n", globalArgs.pidVal );
	printf("------------------------------------\n");

	run_with_parameters();
	return EXIT_SUCCESS;
}

void run_with_parameters() {
	if ( strcmp( globalArgs.mode, "std" ) == 0){
		// Запуск программы для обработки сигналов SIGUSR1, SIGUSR2, SIGHUP
		run_std_mode();
	}
	else if ( strcmp( globalArgs.mode, "child" ) == 0 ) {
		// Запуск программы для обработки сигнала SIGCHLD
		run_child_mode();
	}
	else if ( strcmp( globalArgs.mode, "posix" ) == 0 ) {
		// Запуск программы для обработки POSIX-сигналов
		if (globalArgs.amount != 0) {
			run_posix_mode(globalArgs.amount);
		}
		else {
			printf("Количество POSIX-сигналов не может быть 0\n");
		}
	}
	else if ( strcmp( globalArgs.mode, "kill" ) == 0 ) {
		// Запуск программы для посыла сигналов процессу, группам процессов с указанием сигнала
		if (!globalArgs.pidVal) {
			fprintf(stderr, "Не указан процесс, используйте --pid=value\n");
			exit( EXIT_FAILURE );
		}

		if (!globalArgs.signalName) {
			fprintf(stderr, "Не указан вид сигнала, используйте --signal=value\n");
			exit( EXIT_FAILURE );
		}

		run_kill_mode(globalArgs.signalName, globalArgs.pidVal);
	}
	else if ( strcmp( globalArgs.mode, "pipe" ) == 0 ) {
		// Запуск программы для обработки сигнала SIGCHLD
		run_pipe_mode();
	}
	else {
		fprintf(stderr, "Неверный аргумент в параметре 'mode'\n");
		exit( EXIT_FAILURE );
	}

}