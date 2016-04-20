/******************************************************************************
* FILE: osmpLib.c
* DESCRIPTION:
*
* Hier stehen die Sourcen zu den OSMP-Routinen.
*
* LAST MODIFICATION: Dominik und Tobias, 19.04.2016*****************************************************************************/

#include "OSMP.h"

#define FAIL ((void*) -1)

/**
 * Pointer, der auf den Shared Memory zeigt.
 */
void* sharedMemory = NULL;

/**
 * Flag, ob der Prozséss initialisiert wurde.
 */
int initialised = 0;

int OSMP_Init(int *argc, char ***argv) {
	printf("OSMP_init\n");

	int key =  ftok("/home/dominik/git/betriebsysteme/keyDatei", 5);
	//int key =  ftok("/home/tobias/git/betriebsysteme/keyDatei", 5);

	if(key == -1) {
		return OSMP_ERROR;
	}

	int shmid = shmget(key, 4096, 0);

	if(shmid == -1) {
		return OSMP_ERROR;
	}

	sharedMemory = shmat(shmid, NULL, 0);

	if(sharedMemory == FAIL) {
		return OSMP_ERROR;
	}

	initialised = 1;
	return OSMP_SUCCESS;
}



int OSMP_Size(int *size){

	if(initialised) {
		(*size) = (*((int*)sharedMemory));
		return OSMP_SUCCESS;

	} else {
		return OSMP_ERROR;
	}
}

int OSMP_Rank(int *rank){

	printf("OSMP_Rank(rank: %d)\n", (*rank));

	if(initialised) {
		int * memoryAsInt = (int*) sharedMemory;

		int size = (*memoryAsInt);
		memoryAsInt++;

		for(int i = 0; i < size; i++, memoryAsInt++) {
			printf("Pid: %d\n", (*memoryAsInt));

			if((*memoryAsInt) == getpid()) {
				(*rank) = i;
				return OSMP_SUCCESS;
			}
		}
	}
	return OSMP_ERROR;
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
