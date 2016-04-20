/*
 * OSMP_Routinen.c
 *
 *  Created on: 05.04.2016
 *      Author: dominik
 */

#include "OSMP.h"

void* sharedMemory = NULL;

int OSMP_Init(int *argc, char ***argv) {

	printf("OSMP_init");

	//Prüft, ob genug Argumente übergeben wurden
	//So sollte argv gefüllt werden: ./osmprun 5 ./osmpexecutable [param1,…]

	int key =  ftok("/home/tobias/git/betriebsysteme/keyDatei", 5);

	int shmid = shmget(key, 4096, 0);

	sharedMemory = shmat(shmid, NULL, 0);

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
