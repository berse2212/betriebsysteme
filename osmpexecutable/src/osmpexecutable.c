/*
 * osmpexecutable.c
 *
 *  Created on: 19.04.2016
 *      Author: tobias
 */

#include <stdio.h>

int main(int argc, char **argv) {
	printf("Argc: %d; ", argc);
	printf("Arguments: ");

	int i;

	for(i = 0; i < argc; i++) {
		printf("%s; ", argv[i]);
	}
	printf("\n");

	return 0;
}


