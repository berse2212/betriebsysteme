/*
 * monitor.c
 *
 *  Created on: 15.06.2016
 *      Author: tobias
 */
#include "OSMP.h"

void* sharedMemory;

int getMemory() {
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

	int size = sizeof(struct sharedMemoryHeader) +  count * (sizeof(int) + sizeof(struct offsetOfKP))
		+ OSMP_MAX_SLOTS * (sizeof(struct messageBlockHeader) + OSMP_MAX_PAYLOAD_LENGTH);

	shmid = shmget(key, size, 0);

	if(shmid == -1) {
		return OSMP_ERROR;
	}

	sharedMemory = shmat(shmid, NULL, 0);
	return OSMP_SUCCESS;
}

void printMemory() {
	printf("Aufbau des Shared Memory:\n");


	struct sharedMemoryHeader* sharedMemoryStruct = (struct sharedMemoryHeader *) sharedMemory;

	pid_t* pids = (pid_t *)(((char*) sharedMemory) + ((struct sharedMemoryHeader*) sharedMemory)->pidOffset);

	struct messageBlockHeader* emptyMessage = (struct messageBlockHeader*) (((char*) sharedMemory) + sharedMemoryStruct->firstEmptyMessageOffset);

	struct offsetOfKP * messageOffset = (struct offsetOfKP * ) ((char *) sharedMemory + sharedMemoryStruct->messageOffset);

	printf("Shared Memory Header:\n");

	printf("\tsizePids: %d\n", sharedMemoryStruct->sizePids);
	printf("\tpidOffset: %d\n", sharedMemoryStruct->pidOffset);
	printf("\tmessageOffset: %d\n", sharedMemoryStruct->messageOffset);
	printf("\tfirstEmptyMessageOffset: %d\n", sharedMemoryStruct->firstEmptyMessageOffset);


	printf("PID Bereich:\n");

	for(int i = 0; i < sharedMemoryStruct->sizePids; i++) {
		printf("\tpid[%d]: %d\n", i ,pids[i]);
	}

	printf("Offset Bereich:\n");

	for(int i = 0; i < sharedMemoryStruct->sizePids; i++) {
		printf("\tRank: %d, firstMessageOffset: %d\n", i, messageOffset[i].firstMessageOffset);
		printf("\t         lastMessageOffset: %d\n", messageOffset[i].lastMessageOffset);
	}


	printf("Empty List:\n");

	while(emptyMessage->nextMessageOffset != -1) {
		printf("\tnextMessageOffset: %d\n", emptyMessage->nextMessageOffset);
		emptyMessage = (struct messageBlockHeader*) (((char*) sharedMemory) + emptyMessage->nextMessageOffset);
	}


	int messageOffsetInbox = -1;
	struct messageBlockHeader* message;

	for(int i = 0; i < sharedMemoryStruct->sizePids; i++) {
		printf("Inbox Rank %d:\n", i);

		messageOffsetInbox = messageOffset[i].firstMessageOffset;

		while(messageOffsetInbox != -1) {
			message = (struct messageBlockHeader*) (((char*) sharedMemory) + messageOffsetInbox);

			printf("\tsourcePid: %d\n", message->sourcePid);
			printf("\tpayloadOffset: %d\n", message->payloadOffset);
			printf("\tpayloadLength: %d\n", message->payloadLength);
			printf("\tnextMessageOffset: %d\n", message->nextMessageOffset);
			printf("\tpayload: %s\n", ((char*) sharedMemory) + message->payloadOffset);

			messageOffsetInbox = message->nextMessageOffset;
		}
	}

}

int main(int argc, char **argv) {
	int rv = getMemory();

	if(rv == OSMP_ERROR) {
		printf("Fehler beim Laden des Shared Memorys! Grund: %s", strerror(errno));
		return OSMP_ERROR;
	}

	int value = 10;

	while(value == 10) {
		printMemory();
		value = getchar();
		fflush(stdin);
	}

	return 1;
}
