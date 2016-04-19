/*
 * OSMP_Routinen.c
 *
 *  Created on: 05.04.2016
 *      Author: dominik
 */

#include "OSMP.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int OSMP_Init(int *argc, char ***argv){
	int i , anz;
	pid_t pid;
	//Prüft, ob genug Argumente übergeben wurden
	//So sollte argv gefüllt werden: ./osmprun 5 ./osmpexecutable [param1,…]
	if (*argc < 2){
		printf("Error zu wenig Argumente/n");
		return OSMP_ERROR;
	}


	//liest das zweite argument aus, um die Anzahl der Prozesse zu erhalten
	// atoi wandelt einen String in ein Int um
	anz = atoi((*argv)[0]);
	printf("Anzahl: %i\n", anz);
	for (i = 0; i < anz; i++){
		pid = fork();
		//Prüft ob der Prozess ein Kindprozess ist
		if (pid > 0){
			return OSMP_SUCCESS;
		}

	}
	waitpid(pid, NULL, 0);
	printf("\n\nAll finished\n\n");
	return OSMP_SUCCESS;
}



int OSMP_Size(int *size){


	return OSMP_SUCCESS;
}

int OSMP_Rank(int *rank){

	return OSMP_SUCCESS;
}

int OSMP_Send(const void *buf, int count, int dest ){

	return OSMP_SUCCESS;
}

int OSMP_Recv(void *buf, int count, int *source, int *len){

	return OSMP_SUCCESS;
}

int OSMP_Finalize(void){

	return OSMP_SUCCESS;
}
