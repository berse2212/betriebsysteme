/******************************************************************************
* FILE: OSMP.h
* DESCRIPTION:
*
* Bilbliothek in der die OSMP-Routinen zur Verfügung gestellt werden.
* Außerdem werden hier alle nötigen Biblitheken geladen.
*
* LAST MODIFICATION: Dominik und Tobias, 15.06.2016*****************************************************************************/
#ifndef OSMP_H_
#define OSMP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/sem.h>

/**
 * Konstante für Fehlerfall bei Shared Memory Erzeugung
 */
#define FAIL ((void*) -1)

/**
 * maximale Zahl der Nachrichten pro Prozess
 */
#define OSMP_MAX_MESSAGES_PROC 16

/**
 * maximale Anzahl der Nachrichten, die insgesamt vorhanden sein dürfen
 */
#define OSMP_MAX_SLOTS 256

/**
 * maximale Länge der Nutzlast einer Nachricht 
 */
#define OSMP_MAX_PAYLOAD_LENGTH 128

/**
 * Konstante für Zugriff auf Semaphore Empty All
 */
#define SEMAPHORE_EMPTY_ALL 0

/**
 * Konstante für Zugriff auf Semaphore Mutex
 */
#define SEMAPHORE_MUTEX 1

/**
 * Konstante für Zugriff auf Semaphore Empty
 */
#define SEMAPHORE_EMPTY 2

/**
 * Konstante für Zugriff auf Semaphore Full
 */
#define SEMAPHORE_FULL 3

/**
 * Rückgabewert der OSMP-Routinen im Erfolgsfall
 */
#define OSMP_SUCCESS 0
/**
 * Rückgabewert der OSMP-Routinen im Fehlerfall
 */
#define OSMP_ERROR -1


/**
 * Struktur des Headers des Sharead Memorys
 */
struct sharedMemoryHeader {
    int sizePids;
    int pidOffset;
    int messageOffset;
    int firstEmptyMessageOffset;
};

/**
 * Struktur der Postfächer der Kindprozesse
 */
struct offsetOfKP {
	int firstMessageOffset;
	int lastMessageOffset;
};

/**
 * Struktur der Message-Blöcke
 */
struct messageBlockHeader {
	int sourcePid;
	int payloadOffset;
	int payloadLength;
	int nextMessageOffset;
};


/**
 * Die Routine OSMP_Init() initialisiert die OSMP-Umgebung und ermöglicht den Zugang zu den gemein-
 * samen Ressourcen der OSMP-Prozesse. Sie muss von jedem OSMP-Prozess zu Beginn aufgerufen
 * werden.
 *
 * @param arc Adresse der Argumentzahl
 * @param argv Adresse des Argumentenvektors
 *
 * @return OSMP_SUCCESS im Erfolgsfall, OSMP_ERROR im Fehlerfall
 */
int OSMP_Init(int *argc, char ***argv);

/**
 * Die Routine liefert in *size die Zahl der OSMP-Prozesse ohne den OSMP-Starter Prozess zurück. Sollte
 * mit der Zahl übereinstimmen, die in der Kommandozeile dem OSMP-Starter übergeben wird.
 *
 * @param size Adresse der Zahl der OSMP-Prozesse (output)
 *
 * @retum OSMP_SUCCESS im Erfolgsfall, OSMP_ERROR im Fehlerfall
 */
int OSMP_Size(int *size);

/**
 * Die Routine liefert in *rank die eigene OSMP-Prozessnummer von 0,...,np-1 des aufrufenden OSMP-
 * Prozesses zurück.
 *
 * @param rank Adresse der Prozessnummer 0,...,np-1 des aktuellen OSMP-Prozesse (output)
 *
 * @return OSMP_SUCCESS im Erfolgsfall, OSMP_ERROR im Fehlerfall
 */
int OSMP_Rank(int *rank);

/**
 * Die Routine sendet eine Nachricht an den Prozess mit der Nummer dest . Die Nachricht hat die Länge
 * count . Die zu sendende Nachricht beginnt im aufrufenden Prozess bei der Adresse buf . Wenn die Rou-
 * tine zurückkehrt, ist der Kopierprozess abgeschlossen.
 *
 * @param buf Startadresse des Puffers mit der zu sendenden Nachricht (input)
 * @param count Länge der Nachricht (input)
 * @param dest Nummer des Empfängers zwischen 0,...,np-1 (input)
 *
 * @return OSMP_SUCCESS im Erfolgsfall, OSMP_ERROR im Fehlerfall
 */
int OSMP_Send(const void *buf, int count, int dest );

/**
 * Der aufrufende Prozess empfängt eine Nachricht mit der maximalen Länge count . Die Nachricht wird an
 * die Adresse buf des rufenden Prozesses geschrieben. Unter source wird die OSMP-Prozessnummer
 * des sendenden Prozesses und unter len die tatsächliche Länge der gelesenen Nachricht abgelegt.
 * Die Routine ist blockierend, d.h. sie wartet, bis eine Nachricht für den Prozess vorhanden ist. Wenn die
 * Routine zurückkehrt, ist der Kopierprozess abgeschlossen.
 *
 * @param buf Startadresse des Puffers im lokalen Speicher des aufrufenden Prozessen, in den die Nachricht kopiert werden soll.
 * @param count maximale Länge der Nachricht (input)
 * @param source Adresse der Nummer des Senders zwischen 0,...,np-1 (output)
 * @param len Adresse der tatsächliche Länge der empfangenen Nachricht (output)
 *
 * @return OSMP_SUCCESS im Erfolgsfall, OSMP_ERROR im Fehlerfall
 */
int OSMP_Recv(void *buf, int count, int *source, int *len);

/**
 * Alle OSMP-Prozesse müssen diese Routine aufrufen, bevor sie sich beenden. Sie geben damit den
 * Zugriff auf die gemeinsamen Ressourcen frei.
 *
 * @return OSMP_SUCCESS im Erfolgsfall, OSMP_ERROR im Fehlerfall
 */
int OSMP_Finalize(void);

#endif /* OSMP_H_ */
