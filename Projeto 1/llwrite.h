#ifndef LLWRITE_H
#define LLWRITE_H

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>

#define FALSE 0
#define TRUE 1
struct tramaData
{
	char * trama;
	int size;
};

char* createTramaI(struct tramaData* buf);
int llwrite(int fdimage, int serial_fd);

#endif
