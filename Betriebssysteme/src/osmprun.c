/*
 * osmprun.c
 *
 *  Created on: 19.04.2016
 *      Author: tobias
 */

#include "OSMP.h"

void create(char*);

void* allocate(int);

void delete(void*);


void create(char* path) {
	pid_t pid = fork();

	printf("Pid ist: %d\n", pid);

	if(pid == -1) {
		printf("Fehler beim Erzeugen\n");
		exit(-1);
	}

	if(pid == 0) {

		printf("Ich bin der Kindprozess\n");

		int count = 1;
		char* argv[] = {path, "hallo"};

		printf("argv %s", argv[0]);

		OSMP_Init(&count, &argv);
		printf("Fehler bei der AUsführung\n");
		exit(-1);
	}

	waitpid(pid, NULL, 0);

	printf("Ich bin der ELternprozess\n");
}

int main(int argc, char **argv) {
	create("/home/tobias/workspace/betriebsysteme/osmpexecutable/Debug/osmpexecutable");
}

