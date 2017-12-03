#ifndef _FTP_H
#define _FTP_H

#include "utils.h"

#define DEFAULT_CONTROL_PORT 21
#define SIZE_OF_REPLY 3

typedef struct ftpConnection_t {
	int controlSocket;
	int dataSocket;
} ftpConnection_t;

int sendCommandFTP(int controlSocket, char* command);
int receiveAnswerFTP(int controlSocket, char* answer);
ftpConnection_t createConnectionFTP(url_t url);
int authenticateFTP(ftpConnection_t ftp, url_t url);
int setDirectoryFTP(ftpConnection_t ftp, char* directory);
//...
int destroyConnectionFTP(ftpConnection_t ftp);

#endif
