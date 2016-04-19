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
