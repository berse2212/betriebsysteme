/******************************************************************************
* FILE: osmpLib.c
* DESCRIPTION:
*
* Hier stehen die Sourcen zu den OSMP-Routinen.
*
* LAST MODIFICATION: Dominik und Tobias, 15.06.2016*****************************************************************************/

#include "OSMP.h"

/**
 * Pointer, der auf den Shared Memory zeigt.
 */
void* sharedMemory = NULL;

/**
 * Flag, ob der Prozséss initialisiert wurde.
 */
int initialised = 0;

/**
 * ID des Semaphorsatzes
 */
int semSatz;

/**
 * Erzeugt einen neuen Semaphorsatz
 */
int createSemaphore();

int OSMP_Init(int *argc, char ***argv) {
	printf("OSMP_init\n");

//	int key =  ftok("/home/bsduser022/keyDatei", 5);
	int key =  ftok("/home/tobias/git/betriebsysteme/keyDatei", 5);

	if(key == -1) {
		return OSMP_ERROR;
	}

	int shmid = shmget(key, sizeof(struct sharedMemoryHeader), 0);

	if(shmid == -1) {
		return OSMP_ERROR;
	}

	sharedMemory = shmat(shmid, NULL, 0);

	if(sharedMemory == FAIL) {
		return OSMP_ERROR;
	}

	int count = ((struct sharedMemoryHeader*) sharedMemory)->sizePids;

	int rv = shmdt(sharedMemory);
	if(rv == -1) {
		return OSMP_ERROR;
	}

	int size = sizeof(struct sharedMemoryHeader) 
		+  count * (sizeof(int) + sizeof(struct offsetOfKP))
		+ OSMP_MAX_SLOTS * (sizeof(struct messageBlockHeader) + OSMP_MAX_PAYLOAD_LENGTH);

	shmid = shmget(key, size, 0);

	if(shmid == -1) {
		return OSMP_ERROR;
	}

	sharedMemory = shmat(shmid, NULL, 0);

	initialised = 1;
	createSemaphore();
	return OSMP_SUCCESS;
}

int createSemaphore() {
	int keyEmpty = ftok("/home/tobias/git/betriebsysteme/semaphoreDateiEmpty", 42);

	int size = -1;

	if(OSMP_Size(&size) == OSMP_ERROR) {
		return OSMP_ERROR;
	}

	semSatz = semget(keyEmpty, 2 * size + 2, 0);

	return OSMP_SUCCESS;
}

int getSemOffset(int semName) {
	int size = -1;

	switch(semName) {
		case SEMAPHORE_EMPTY_ALL: return 0;
		case SEMAPHORE_MUTEX: return 1;
		case SEMAPHORE_EMPTY: return 2;
		case SEMAPHORE_FULL:
			OSMP_Size(&size);
			return size + 2;
	}
	
	return OSMP_ERROR;
}

void sem_wait(int semName, int index) {
	struct sembuf sembuf = {.sem_num = (unsigned short) (getSemOffset(semName) + index), .sem_op = -1, .sem_flg = 0};

	semop(semSatz, &sembuf, 1);
}

void sem_signal(int semName, int index) {
	struct sembuf sembuf = {.sem_num = (unsigned short) (getSemOffset(semName) + index), .sem_op = 1, .sem_flg = 0};

	semop(semSatz, &sembuf, 1);
}

int OSMP_Size(int *size){
	if(initialised && (size != NULL)) {
		(*size) = ((struct sharedMemoryHeader*) sharedMemory)->sizePids;
		return OSMP_SUCCESS;
	}

	return OSMP_ERROR;
}

int OSMP_Rank(int *rank){
	if(initialised && (rank != NULL)) {
		int size = ((struct sharedMemoryHeader*) sharedMemory)->sizePids;

		printf("pid offset %d\n", ((struct sharedMemoryHeader*) sharedMemory)->pidOffset);

		pid_t* pids = (pid_t *)(((char*) sharedMemory) + ((struct sharedMemoryHeader*) sharedMemory)->pidOffset);

		for(int i = 0; i < size; i++) {
			printf("pids[i]: %d ; pid: %d\n", pids[i], getpid());

			if(pids[i] == getpid()) {
				(*rank) = i;
				return OSMP_SUCCESS;
			}
		}
	}
	
	return OSMP_ERROR;
}

int OSMP_Send(const void *buf, int count, int dest ){
	if(initialised) {
		int rank = -1;

		if(OSMP_Rank(&rank) == OSMP_ERROR) {
			return OSMP_ERROR;
		}

		printf("Send SemEmpty value: %d\n", semctl(semSatz, getSemOffset(SEMAPHORE_EMPTY) + rank, GETVAL));
		printf("Send SemEmptyAll value: %d\n", semctl(semSatz, getSemOffset(SEMAPHORE_EMPTY_ALL) + 0, GETVAL));
		printf("Send SemMutex value: %d\n", semctl(semSatz, getSemOffset(SEMAPHORE_MUTEX) + 0, GETVAL));

		//Synchronisation
		sem_wait(SEMAPHORE_EMPTY, rank);
		sem_wait(SEMAPHORE_EMPTY_ALL, 0);
		sem_wait(SEMAPHORE_MUTEX, 0);
		//Synchronisation

		printf("Am Senden (Nach wait)\n");

		struct sharedMemoryHeader* sharedMemoryStruct = (struct sharedMemoryHeader *) sharedMemory;

		struct messageBlockHeader message = {.sourcePid = rank, .payloadOffset = -1, .payloadLength = count, .nextMessageOffset = -1};

		int firstEmptyMessageOffset = sharedMemoryStruct->firstEmptyMessageOffset;

		printf("firstOffset %d\n", firstEmptyMessageOffset);

		struct messageBlockHeader* emptyMessage = (struct messageBlockHeader*) (((char*) sharedMemory) + firstEmptyMessageOffset);

		int nextEmptyOffset = emptyMessage->nextMessageOffset;

		struct offsetOfKP * messageOffset = (struct offsetOfKP * ) ((char *) sharedMemory + sharedMemoryStruct->messageOffset);

		sharedMemoryStruct->firstEmptyMessageOffset = nextEmptyOffset;

		(*emptyMessage) = message;

		printf("send message size %d\n", emptyMessage->payloadLength);

		printf("firstOffset %d\n", firstEmptyMessageOffset);

		emptyMessage->payloadOffset = firstEmptyMessageOffset + (int) sizeof(struct messageBlockHeader);

		printf("send message offset %d\n", emptyMessage->payloadOffset);

		//Eigentliches kopieren der Daten
		memcpy((void*) ((char*) sharedMemory) + emptyMessage->payloadOffset, buf, (size_t) count);
		printf("send message payload %s\n", ((char*) sharedMemory) + emptyMessage->payloadOffset);

		//Prüfe ob noch keine Nachrichten im Postfach des Empfängers liegen
		if(messageOffset[dest].lastMessageOffset != -1) {
			struct messageBlockHeader* block = (struct messageBlockHeader*) (((char*) sharedMemory) + messageOffset[dest].lastMessageOffset);

			block->nextMessageOffset = firstEmptyMessageOffset;
			messageOffset[dest].lastMessageOffset = firstEmptyMessageOffset;
		} else {
			printf("First message offset: %d\n", firstEmptyMessageOffset);

			messageOffset[dest].lastMessageOffset = firstEmptyMessageOffset;
			messageOffset[dest].firstMessageOffset = firstEmptyMessageOffset;
		}

		//Synchronisation
		sem_signal(SEMAPHORE_MUTEX, 0);
		sem_signal(SEMAPHORE_FULL, dest);

		printf("Send nach signal SemFull value: %d\n", semctl(semSatz, getSemOffset(SEMAPHORE_FULL) + rank, GETVAL));
		printf("Send nach signal SemMutex value: %d\n", semctl(semSatz, getSemOffset(SEMAPHORE_MUTEX) + 0, GETVAL));
		//Synchronisation

		printf("Senden beendet (Nach Signal)\n");
	}

	return OSMP_SUCCESS;
}

int OSMP_Recv(void *buf, int count, int *source, int *len){
	if(initialised) {
		int rank = -1;

		if(OSMP_Rank(&rank) == OSMP_ERROR) {
			return OSMP_ERROR;
		}

		printf("Rec SemFull value: %d\n", semctl(semSatz, getSemOffset(SEMAPHORE_FULL) + rank, GETVAL));
		printf("Rec SemMutex value: %d\n", semctl(semSatz, getSemOffset(SEMAPHORE_MUTEX) + 0, GETVAL));

		//Synchronisation
		sem_wait(SEMAPHORE_FULL, rank);

		printf("Am Receiven (Nach wait FULL)\n");

		sem_wait(SEMAPHORE_MUTEX, 0);
		//Synchronisation

		printf("Am Receiven (Nach wait)\n");

		struct sharedMemoryHeader* sharedMemoryStruct = (struct sharedMemoryHeader *) sharedMemory;

		struct offsetOfKP * messageOffset = (struct offsetOfKP * ) ((char *) sharedMemory + sharedMemoryStruct->messageOffset);

		int firstMessageOffset = messageOffset[rank].firstMessageOffset;

		struct messageBlockHeader* message = (struct messageBlockHeader*) ((char *) sharedMemory + firstMessageOffset);

		(*source) = message->sourcePid;

		(*len) = message->payloadLength;

		memcpy(buf, ((char*) sharedMemory) + message->payloadOffset, (size_t) (count > message->payloadLength? message->payloadLength : count));

		printf("Alter Offset %d\n", messageOffset[rank].firstMessageOffset);
		printf("Neuer Offset %d\n", message->nextMessageOffset);

		messageOffset[rank].firstMessageOffset = message->nextMessageOffset;

		if(message->nextMessageOffset == -1) {
			messageOffset[rank].lastMessageOffset = -1;
		}

		int firstEmptyOffset = sharedMemoryStruct->firstEmptyMessageOffset;

		sharedMemoryStruct->firstEmptyMessageOffset = firstMessageOffset;

		message->nextMessageOffset = firstEmptyOffset;

		//Synchronisation
		sem_signal(SEMAPHORE_MUTEX, 0);
		sem_signal(SEMAPHORE_EMPTY_ALL , rank);
		sem_signal(SEMAPHORE_EMPTY, 0);
		//Synchronisation

		printf("Receiven beendet (Nach signal)\n");
	}

	return OSMP_SUCCESS;
}

int OSMP_Finalize(void){
	int rv = shmdt(sharedMemory);
	if(rv == -1) {
		return OSMP_ERROR;
	}

	return OSMP_SUCCESS;
}
