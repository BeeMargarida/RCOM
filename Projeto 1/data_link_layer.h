#ifndef DATA_LINK_H
#define DATA_LINK_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "utils.h"

#define BAUDRATE B38400
#define BUF_SIZE 256
#define RETRIES 4

int llopen(int serial, Types_t type);
int llread(int fd, unsigned char* buf);
int llwrite(int serial_fd, control_packet_t packet);
int llclose(int fd, Types_t type);

void receive_alarm();
int create_alarm();

#endif
