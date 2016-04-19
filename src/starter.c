/*
 ============================================================================
 Name        : Betriebssysteme.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "OSMP.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

void waitForChildrenToComplete();

void startEchoAll(int);

void childOutput();

void startOutput();

void waitForChildrenToComplete() {
	int pid = 0;
	do {
		pid = wait(0);

		if(pid != -1) {
			printf("Child Completeted: %d\n", pid);
		}
	} while(pid != -1);

	printf("All Children completetd :)\n");
}


void startEchoAll(int anz) {


	printf("\nStarting Echo\n");

	int i;
	pid_t pid;
	printf("Anzahl: %i\n", anz);
	for (i = 0; i < anz; i++) {
		pid = fork();
		//Prüft ob der Prozess ein Kindprozess ist
		if (pid < 0) {
			printf("Fehler bei der Prozesserzeugung: %s", strerror(errno));
			exit(pid);
		} else if (pid == 0) {
			printf("pid: %u; ", getpid());
			printf("parentpid: %u\n", getppid());
			char* process =
					"/home/tobias/workspace/betriebsysteme/EchoAll/Debug/EchoAll";
			printf("Starting Process %s\n", process);
			int rv = execlp(process, "EchoAll", "Argument1", "2", NULL);
			if (rv == -1) {
				printf("Could not start %s\n", process);
				printf("Reason: %s\n", strerror(errno));
				exit(rv);
			}
		}
	}

	waitForChildrenToComplete();
	printf("Parent stopped\n");
}

void childOutput() {

	for(int i = 0; i < 10; i++) {
		printf("Child counting: %d\n", i);
		sleep(i < 5 ? i : 5);
	}
}

void parentOutput() {
	for(int i = 0; i < 10; i = i + 2) {
		printf("Parent helping: %d\n", i);

		sleep(2);
	}
}

void startOutput() {

	printf("Starting Output\n");

	int pid = fork();

	if(pid < 0) {
		printf("Fehler bei der Prozesserzeugung: %d", pid);
		exit(pid);
	} else if(pid == 0) {
		childOutput();
	} else {
		parentOutput();
		//Waiting for Child
		wait(0);
	}
}

int main(int argc, char *argv[]) {

//	startOutput();
	startEchoAll(5);
}
