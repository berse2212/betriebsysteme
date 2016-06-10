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
#include <sys/sem.h>
#include <inttypes.h>


#define FAIL ((void*) -1)

#define OSMP_MAX_MESSAGES_PROC 16

#define OSMP_MAX_SLOTS 256

#define OSMP_MAX_PAYLOAD_LENGTH 128

/**
 * Rückgabewert der OSMP-Routinen im Erfolgsfall
 */
#define OSMP_SUCCESS 0
/**
 * Rückgabewert der OSMP-Routinen im Fehlerfall
 */
#define OSMP_ERROR -1


struct sharedMemoryHeader {
    int sizePids;
    int pidOffset;
    int messageOffset;
    int firstEmptyMessageOffset;
};

struct offsetOfKP {
	int firstMessageOffset;
	int lastMessageOffset;
};

struct messageBlockHeader {
	int sourcePid;
	int payloadOffset;
	int payloadLength;
	int nextMessageOffset;
};


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

struct sharedMemoryHeader* buildSharedMemoryStruct(int count);

int createSemaphore(int size);

int semSatz;

/**
 * Pointer zum Shared Memory.
 */
void* sharedMemory;

struct sharedMemoryHeader* buildSharedMemoryStruct(int count) {
	struct sharedMemoryHeader* sharedMemoryStruct = (struct sharedMemoryHeader*) sharedMemory;

	sharedMemoryStruct->sizePids = count;
	sharedMemoryStruct->pidOffset = sizeof(struct sharedMemoryHeader);
	sharedMemoryStruct->messageOffset = sharedMemoryStruct->pidOffset + sharedMemoryStruct->sizePids * (int) sizeof(int);
	sharedMemoryStruct->firstEmptyMessageOffset = sharedMemoryStruct->messageOffset	+ sharedMemoryStruct->sizePids * (int) sizeof(struct offsetOfKP);

	struct messageBlockHeader* message = (struct messageBlockHeader*) ((char*) sharedMemory + sharedMemoryStruct->firstEmptyMessageOffset);
	int lastOffset = sharedMemoryStruct->firstEmptyMessageOffset;

	for(int i = 0; i < OSMP_MAX_SLOTS; i++) {
		message->sourcePid = -1;
		message->payloadOffset = -1;
		message->payloadLength = -1;
		message->nextMessageOffset =  lastOffset + (int) sizeof(struct messageBlockHeader) + OSMP_MAX_PAYLOAD_LENGTH;
		lastOffset = message->nextMessageOffset;

		message = (struct messageBlockHeader*) ((char*) sharedMemory + message->nextMessageOffset);
	}

	printf("pid offset %d\n", sharedMemoryStruct->pidOffset);

	return sharedMemoryStruct;
}

int create(int count, char* path, char** arguments) {

	printf("hello\n");

	struct sharedMemoryHeader* sharedMemoryStruct = buildSharedMemoryStruct(count);

	createSemaphore(count);

	pid_t * pids = (pid_t *)((char*)sharedMemory + sharedMemoryStruct->pidOffset);

    struct offsetOfKP * offsetOfKps = (struct offsetOfKP *)((char*)sharedMemory + sharedMemoryStruct->messageOffset);

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

		printf("setze pid: %d\n" , pid);

		pids[i] = pid;
		offsetOfKps[i].firstMessageOffset = -1;
		offsetOfKps[i].lastMessageOffset = -1;
	}
	printf("Ich bin der ELternprozess\n");

	return OSMP_SUCCESS;
}

int createSemaphore(int size) {
	int keyEmpty = ftok("/home/tobias/git/betriebsysteme/semaphoreDateiEmpty", 42);

	semSatz = semget(keyEmpty, 2 * size + 2, IPC_CREAT | 0640);

	printf("SemSatz: %d\n", semSatz);

	int val = -1;

	for(int i = 0; i < 2 * size + 2; i++) {
		if(i == 0) {
			val = semctl(semSatz, i, SETVAL, (int) OSMP_MAX_SLOTS);
		} else if(i == 1) {
			val = semctl(semSatz, i, SETVAL, (int) 1);
		} else if(i < size + 2) {
			val = semctl(semSatz, i, SETVAL, (int) OSMP_MAX_MESSAGES_PROC);
		} else {
			val = semctl(semSatz, i, SETVAL, (int) 0);
		}

		printf("Val: %d \n", val);
	}


	return OSMP_SUCCESS;
}

void deleteSemaphore() {
	semctl(semSatz, 0, IPC_RMID);
}


void* allocate(key_t key, int* shmid) {
	size_t size = OSMP_MAX_SLOTS * (OSMP_MAX_PAYLOAD_LENGTH + sizeof(struct messageBlockHeader))
			+ 1000;

	printf("Size vom SharedMemory %ld\n", size);

	(*shmid) = shmget(key, size, IPC_CREAT | 0640);

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
	printf("Size: %d\n", ((struct sharedMemoryHeader*) ptr)->sizePids);

	printf("Ausgabe Ranks:\n");

	pid_t * pids = (pid_t *)((char*) sharedMemory + ((struct sharedMemoryHeader*) ptr)->pidOffset);

	for(int i = 0; i < PROCESS_COUNT; i++) {
		printf("Rank Nr %d: %d\n", i, pids[i]);
	}

    for(int i = 0; i < PROCESS_COUNT; i++) {
        waitpid(pids[i], NULL, 0);
    }

	delete(sharedMemory, shmid);
	deleteSemaphore();

	//scanf("%d", ptr);
}

