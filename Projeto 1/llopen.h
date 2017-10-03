#ifndef LLOPEN_H
#define LLOPEN_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

enum
{
	SENDER, RECEIVER
} types;

void receive_alarm();
int create_alarm();
int llopen(int fd, int type);

#endif
