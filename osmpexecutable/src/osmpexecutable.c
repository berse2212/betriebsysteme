/******************************************************************************
* FILE: osmpexecutable.c
* DESCRIPTION:
*
* Programm, welches von den Kindprozessen geladen werden soll.
* Hier werden erstmal nur die OSMP-Routinen getestet.
*
* LAST MODIFICATION: Dominik und Tobias, 19.04.2016*****************************************************************************/

#include "OSMP.h"

int main(int argc, char **argv) {
    pid_t pid = getpid();

	int rv = OSMP_Init(&argc, &argv);

	if(rv == OSMP_ERROR){
		printf("Pid: %d: Fehler INIT. Grund: %s\n", pid, strerror(errno));
		exit(-1);
	}

	printf("Pid: %d: Teste size\n", pid);

	int size = -1;

	rv = OSMP_Size(&size);

	if(rv == OSMP_ERROR){
		printf("Pid: %d: Fehler Size. Grund: %s\n", pid, strerror(errno));
		exit(-1);
	}

	printf("Pid: %d: Dies ist die Size: %d\n", pid,size);
	printf("Pid: %d: Teste Rank\n", pid);

	int rank = -1;

	rv = OSMP_Rank(&rank);

	if(rv == OSMP_ERROR){
		printf("Pid: %d: Fehler Rank. Grund: %s\n", pid, strerror(errno));
		exit(-1);
	}

	printf("Pid: %d: Dies ist der Rank: %d\n", pid, rank);
	return 0;
}


