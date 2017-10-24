#ifndef LLREAD_H
#define LLREAD_H

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>
#include <string.h>

#define FALSE 0
#define TRUE 1
#define BUF_SIZE 256


unsigned char generateBCC(unsigned char* buf, int size);
void processTram(unsigned char* tram, unsigned char* buf, int size, int* duplicate);
void sendRR();
void sendREJ();
int llread(int fd, unsigned char* buf, int* duplicate);

#endif
