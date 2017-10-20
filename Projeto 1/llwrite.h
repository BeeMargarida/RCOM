#ifndef LLWRITE_H
#define LLWRITE_H

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>

#define FALSE 0
#define TRUE 1

typedef struct {
	unsigned char *params;
	int size;
} control_packet_t;

control_packet_t createTramaI(control_packet_t packet);
int llwrite(int serial_fd, control_packet_t packet);

#endif
