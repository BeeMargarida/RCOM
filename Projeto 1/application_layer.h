#ifndef APP_LAYER_H
#define APP_LAYER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include "utils.h"

#define DATA_BLOCK 0x01
#define DATA_START 0x02
#define DATA_END 0x03

#define FILESIZE 0x00
#define FILENAME 0x01
#define MODE 0111111000

int startReceiver(int serial_no);
int startSender(char* fileName, int serial_no);

#endif
