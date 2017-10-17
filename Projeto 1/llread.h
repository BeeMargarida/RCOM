#ifndef LLREAD_H
#define LLREAD_H

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>

#define FALSE 0
#define TRUE 1
#define BUF_SIZE 512

int generateBCC(char* buf, int size);
void processTram(char* tram, char* buf);
void sendRR();
void sendREJ();
int llread(int fd, char* buf);

#endif
