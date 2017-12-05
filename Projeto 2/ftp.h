#ifndef _FTP_H
#define _FTP_H

#include "utils.h"

#define DEFAULT_CONTROL_PORT 21
#define SIZE_OF_REPLY 3
#define BUF_SIZE 512

typedef struct ftpConnection_t {
	int controlSocket;
	int dataSocket;
} ftpConnection_t;

ftpConnection_t createConnectionFTP(url_t url);
int authenticateFTP(ftpConnection_t ftp, url_t url);
int setDirectoryFTP(ftpConnection_t ftp, char* directory);
int setPassiveModeFTP(ftpConnection_t ftp);
int downloadFTP(ftpConnection_t ftp);
int destroyConnectionFTP(ftpConnection_t ftp);

#endif
