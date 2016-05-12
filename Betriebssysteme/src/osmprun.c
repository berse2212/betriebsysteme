/******************************************************************************
* FILE: osmprun.c
* DESCRIPTION:
*
* File in dem das Programm gestartet wird.
* Es werden 5 Prozesse erzeugt, die das Programm osmpexecuteble ausführen.
* Für die Interprozesskommunikation wird ein Shared Memory zur Verfügung gestellt.
*
* LAST MODIFICATION: Dominik und Tobias, 19.04.2016*****************************************************************************/

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "OSMP.h"

#define PROCESS_COUNT 5

/**
 * Erzeugt neue Prozesse.
 * Es werden so viele Prozesse erstellt, wie in count angegeben.
 * Jeder Kindprozess lädt das Programm, welches unter dem übergebenden Pfad zu finden, nach der Erzeugung.
 * Beim Programmaufruf werden die übergebenen Argumente mit übergeben.
 *
 * @param count Anzahl der Prozesse, die erzeugt werden sollen.
 * @param path Pfad, wo das Programm osmpexecutable liegt
 * @param arguments Argumente, die bei der Programmausführung übergeben werden
 */
int create(int count, char* path, char** arguments);

/**
 * Allokiert einen neuen Shared Memory mit dem Key.

 * @param key gültiger Schlüssel für IPC-Routinen, um shared memory zu erzeuegen
 * @param shmid Adresse ID des Shared Memory
 * @return Pointer auf den Shared Memory
 */
void* allocate(key_t key, int* shmid);

/**
 * Löscht den Shared Memory.

 * @param ptr Pointer zum Shared Memory
 * @param key gültiger Schlüssel für IPC-Routinen
 */
void delete(void* ptr, int key);

/**
 * Pointer zum Shared Memory.
 */
void* sharedMemory;


int create(int count, char* path, char** arguments) {

	printf("hello\n");

	struct sharedMemory * sharedMemoryStruct = (struct sharedMemory*) sharedMemory;

	sharedMemoryStruct->size = count;

	sharedMemoryStruct->pidOffset = sizeof(struct sharedMemory);

    pid_t * pids = (pid_t *)((char*)sharedMemory + sharedMemoryStruct->pidOffset);

	for(int i = 0; i < count; i++) {
		int pid = fork();

		if(pid == -1) {
			printf("Fehler beim Erzeugen\n");
			return OSMP_ERROR;
		}

		if(pid == 0) {
			printf("Ich bin der Kindprozess\n");
			printf("Starting Process %s\n", path);
			int rv = execv(path, arguments);

            printf("Could not start %s\n", path);
            printf("Reason: %s\n", strerror(errno));
            exit(rv);
		}

		pids[i] = pid;
	}
	printf("Ich bin der ELternprozess\n");

	return OSMP_SUCCESS;
}


void* allocate(key_t key, int* shmid) {
	(*shmid) = shmget(key, 4096, IPC_CREAT | 0640);

	if(*shmid == -1) {
		printf("Fehler beim allokieren vom Gemeinsammenspeicher. Grund: %s\n", strerror(errno));
		exit(-1);
	}

	void* ptr = shmat(*shmid, NULL, 0);

	if( ptr == FAIL) {
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

		free(shmid);

		if(rv == -1) {
			printf("Fehler beim loeschen des Gemeinsammen speichers. Grund: %s\n", strerror(errno));
		} else {
			printf("Speicher geloescht! :)\n");
		}
	}
}

int main(int argc, char **argv) {

	int shmid = 0;

    key_t key = ftok("/home/tobias/git/betriebsysteme/keyDatei", 5);

    if(key == -1) {
        printf("Fehler beim Erstellen des Keys. Grund: %s\n", strerror(errno));
        exit(-1);
    }

	sharedMemory = allocate(key, &shmid);
	//sharedMemory = allocate(ftok("/home/tobias/git/betriebsysteme/keyDatei", 5), &shmid);

    char* arguments[] = {"OSMPexecutable", NULL};


	//create(PROCESS_COUNT, "/home/bsduser022/OSMPexecutable/bin/Debug/OSMPexecutable", arguments);
	create(PROCESS_COUNT, "/home/tobias/git/betriebsysteme/osmpexecutable/Debug/osmpexecutable", arguments);

	int* ptr = sharedMemory;

	printf("Shared Mem:\n");
	printf("Size: %d\n", ((struct sharedMemory*) ptr)->size);

	printf("Ausgabe Ranks:\n");

	pid_t * pids = (pid_t *)((char*) sharedMemory + ((struct sharedMemory*) ptr)->pidOffset);

	for(int i = 0; i < PROCESS_COUNT; i++) {
		printf("Rank Nr %d: %d\n", i, pids[i]);
	}

    for(int i = 0; i < PROCESS_COUNT; i++) {
        waitpid(pids[i], NULL, 0);
    }
	delete(sharedMemory, shmid);

	scanf("%d", ptr);
}

