/*
 * OSMP.h
 *
 *  Created on: 05.04.2016
 *      Author: dominik
 */

#ifndef OSMP_H_
#define OSMP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

int OSMP_Init(int *argc, char ***argv);
int OSMP_Size(int *size);
int OSMP_Rank(int *rank);
int OSMP_Send(const void *buf, int count, int dest );
int OSMP_Recv(void *buf, int count, int *source, int *len);
int OSMP_Finalize(void);

#define OSMP_SUCCESS 0
#define OSMP_ERROR -1


#endif /* OSMP_H_ */
