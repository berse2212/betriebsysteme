/*
 * osmpexecutable.c
 *
 *  Created on: 19.04.2016
 *      Author: tobias
 */

#include <stdio.h>
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
		printf("Fehler INIT. GRund: %s\n", strerror(errno));
		exit(-1);
	}

	printf("Teste size\n");

	int size = -1;

	rv = OSMP_Size(&size);

	if(rv == OSMP_ERROR){
		printf("Fehler Size. GRund: %s\n", strerror(errno));
		exit(-1);
	}

	printf("DIes ist die SIze: %d\n", size);

	printf("Teste Rank\n");

	int rank = -1;

	rv = OSMP_Rank(&rank);

	if(rv == OSMP_ERROR){
		printf("Fehler Rank. GRund: %s\n", strerror(errno));
		exit(-1);
	}

	printf("DIes ist der Rank: %d\n", rank);


	return 0;
}


