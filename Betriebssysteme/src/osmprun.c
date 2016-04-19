/*
 * osmprun.c
 *
 *  Created on: 19.04.2016
 *      Author: tobias
 */

#include "OSMP.h"
#include <sys/ipc.h>
#include <sys/shm.h>

void create(char*);

void* allocate(int, int*);

void delete(void*, int);


void create(char* path) {
	pid_t pid = fork();

	printf("Pid ist: %d\n", pid);

	if(pid == -1) {
		printf("Fehler beim Erzeugen\n");
		exit(-1);
	}

	if(pid == 0) {

		printf("Ich bin der Kindprozess\n");
		printf("argv %s\n", path);

		int count = 1;
		char** argv = malloc(sizeof(char*) * 2);
		argv[0] = path;
		argv[1] = "test";


		OSMP_Init(&count, &argv);
		printf("Fehler bei der AUsführung\n");
		exit(-1);
	}

	waitpid(pid, NULL, 0);

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
	create("/home/tobias/git/betriebsysteme/osmpexecutable/Debug/osmpexecutable");

	int shmid = 0;

	void* ptr = allocate(ftok("/home/tobias/git/betriebsysteme/keyDatei", 5), &shmid);

	printf("Shmid = %d\n", shmid);

	delete(ptr, shmid);
}

