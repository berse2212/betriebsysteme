/*
 * osmprun.c
 *
 *  Created on: 19.04.2016
 *      Author: tobias
 */

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <wait.h>
#include <sys/errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>


void create(int, char*, char**);

void* allocate(int, int*);

void delete(void*, int);

void* sharedMemory;

void create(int count, char* path, char** arguments) {

	memcpy(sharedMemory, &count, sizeof(int));

	for(int i = 0; i < count; i++) {
		pid_t pid = fork();

		printf("Pid ist: %d\n", pid);

		if(pid == -1) {
			printf("Fehler beim Erzeugen\n");
			exit(-1);
		}

		if(pid == 0) {

			printf("Ich bin der Kindprozess\n");

			printf("Starting Process %s\n", path);
			int rv = execvp(path, arguments);
			if (rv == -1) {
				printf("Could not start %s\n", path);
				printf("Reason: %s\n", strerror(errno));
				exit(rv);
			}
			printf("Fehler bei der AUsführung\n");
			exit(-1);
		}

		memcpy(sharedMemory + sizeof(int) * (i+1), &pid, sizeof(int));
	}




	printf("Ich bin der ELternprozess\n");
}


void* allocate(int key, int* shmid) {
	printf("key: %d\n", key);

	(*shmid) = shmget(key, 4096, IPC_CREAT | 0640);

	printf("id %d\n", *shmid);

	if(*shmid == -1) {
		printf("Fehler beim allokieren vom Gemeinsammenspeicher. Grund: %s\n", strerror(errno));
		exit(-1);

	}

	void* ptr = shmat(*shmid, NULL, 0);

	if((char*) ptr == (char*) -1) {
		printf("Fehler beim einblenden des gemeinsammen Speichers. Grund: %s\n", strerror(errno));
		exit(-1);
	}

	printf("Speicher erfolgreich erstellt :)!\n");
	return ptr;
}

void delete(void* ptr, int key) {
	int rv =  shmdt(ptr);

	if(rv == -1) {
		printf("Fehler beim loeschen des Gemeinsammen speichers. Grund: %s\n", strerror(errno));
	} else {
		printf("Gemeinsammer Speicher wurde ausgebledet!\n");

		struct shmid_ds* shmid = malloc(sizeof(struct shmid_ds));

		rv = shmctl(key, IPC_RMID, shmid);

		if(rv == -1) {
			printf("Fehler beim loeschen des Gemeinsammen speichers. Grund: %s\n", strerror(errno));
		} else {
			printf("Speicher geloescht! :)\n");
		}
	}
}

int main(int argc, char **argv) {

	int shmid = 0;

	sharedMemory = allocate(ftok("/home/tobias/git/betriebsysteme/keyDatei", 5), &shmid);

	int count = 1;

	create(count, "/home/tobias/git/betriebsysteme/osmpexecutable/Debug/osmpexecutable", NULL);

	int* ptr = sharedMemory;

	printf("Shared Mem:\n");
	printf("Size: %d\n", (*ptr));

	ptr++;

	printf("Ausgabe Ranks:\n");
	for(int i = 0; i < count; i++, ptr++) {
		printf("Rank Nr %d: %d\n", i, (*ptr));
	}

	printf("Shmid = %d\n", shmid);

	sleep(5);

	delete(sharedMemory, shmid);
}

