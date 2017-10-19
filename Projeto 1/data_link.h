#ifndef DATA_LINK_H
#define DATA_LINK_H

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

#define DATA_BLOCK 0x01
#define DATA_START 0x02
#define DATA_END 0x03

#define FILESIZE 0x00
#define FILENAME 0x01
#define MODE 0111111000

enum
{
	SENDER, RECEIVER
} types;


void receive_alarm();
int create_alarm();
int llopen(int fd, int type);
int llclose(int fd);

//APP
void writeDataStart(char* buf);
void writeDataBlock(char* buf);
int getfileSize();
int getImageData(unsigned char* buf);
void createTramaStartEnd(unsigned char *fsize, char *fname, int id);
void createFirstEndPacket();
struct tramaData * createDataPacket(int n);
struct tramaData* getDataPacket(int f, int n);

#endif
