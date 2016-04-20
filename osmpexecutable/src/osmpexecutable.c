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
	printf("Argc: %d; ", argc);
	printf("Arguments: ");

	int i;

	for(i = 0; i < argc; i++) {
		printf("%s; ", argv[i]);
	}
	printf("\n");

	int rv = OSMP_Init(&argc, &argv);

	if(rv == OSMP_ERROR){
		printf("Fehler INIT. Grund: %s\n", strerror(errno));
		exit(-1);
	}

	printf("Teste size\n");

	int size = -1;

	rv = OSMP_Size(&size);

	if(rv == OSMP_ERROR){
		printf("Fehler Size. Grund: %s\n", strerror(errno));
		exit(-1);
	}

	printf("Dies ist die Size: %d\n", size);
	printf("Teste Rank\n");

	int rank = -1;

	rv = OSMP_Rank(&rank);

	if(rv == OSMP_ERROR){
		printf("Fehler Rank. GRund: %s\n", strerror(errno));
		exit(-1);
	}

	printf("Dies ist der Rank: %d\n", rank);
	return 0;
}


