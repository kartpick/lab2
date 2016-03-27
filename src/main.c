#include <lab2.h>
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

struct globalArgs_t {
	char *logFile;
	char * execute;
	int multiplex;
} globalArgs;

static const struct option longOpts[] = {
	{"logFile", required_argument, NULL, 0},
	{"multiplex", required_argument, NULL, 0},
	{"execute", required_argument, NULL, 0}
};

int main(int argc, char *argv[]) {
	int opt = 0;
	int longIndex = 0;

	// Инициализация параметров перед использованием
	globalArgs.logFile = NULL;
	globalArgs.execute = NULL;
	globalArgs.multiplex = 1;

	opt = getopt_long( argc, argv, "", longOpts, &longIndex );
	while (opt != -1) {
		if ( strcmp( "logFile", longOpts[longIndex].name ) == 0 ) {
			globalArgs.logFile = optarg;
		}
		else if ( strcmp( "multiplex", longOpts[longIndex].name ) == 0 ) {
			globalArgs.multiplex = atoi(optarg);
		}
		else if ( strcmp( "execute", longOpts[longIndex].name ) == 0 ) {
			globalArgs.execute = optarg;
		}
		opt = getopt_long( argc, argv, "", longOpts, &longIndex);
	}
	printf("------------GetOpt------------------\n");
	printf("Лог-файл: %s\n", globalArgs.logFile );
	printf("Execute: %s\n", globalArgs.execute );
	printf("Multiplex: %d\n", globalArgs.multiplex );
	printf("------------------------------------\n");

	run_with_parameters();
	return EXIT_SUCCESS;
}

void run_with_parameters() {
	run_cmd(globalArgs.execute, globalArgs.logFile);
	exit( EXIT_FAILURE );
}