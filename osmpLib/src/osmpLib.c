/*
 * OSMP_Routinen.c
 *
 *  Created on: 05.04.2016
 *      Author: dominik
 */

#include "OSMP.h"

int OSMP_Init(int *argc, char ***argv) {

	printf("OSMP_init");

	//Prüft, ob genug Argumente übergeben wurden
	//So sollte argv gefüllt werden: ./osmprun 5 ./osmpexecutable [param1,…]
	if ((*argc) < 1){
		printf("Error zu wenig Argumente/n");
		return OSMP_ERROR;
	}


	printf("Starting Process %s\n", (*argv)[0]);
	int rv = execlp((*argv)[0], "EchoAll", "Argument1", "2", NULL);
	if (rv == -1) {
		printf("Could not start %s\n", (*argv)[0]);
		printf("Reason: %s\n", strerror(errno));
		exit(rv);
	}

	return OSMP_ERROR;
}



int OSMP_Size(int *size){

	printf("OSMP_Size(size: %d) ", (*size));

	return OSMP_SUCCESS;
}

int OSMP_Rank(int *rank){

	printf("OSMP_Rank(rank: %d) ", (*rank));

	return OSMP_SUCCESS;
}

int OSMP_Send(const void *buf, int count, int dest ){

	printf("OSMP_Send(buf: %s, count: %d, dest: %d) ", buf, count, dest);

	return OSMP_SUCCESS;
}

int OSMP_Recv(void *buf, int count, int *source, int *len){

	printf("OSMP_Recv(buf: %s,  count: %d, source %d, len: %d) ", buf, count, (*source), (*len));

	return OSMP_SUCCESS;
}

int OSMP_Finalize(void){

	printf("OSMP_Finalize() ");

	return OSMP_SUCCESS;
}
