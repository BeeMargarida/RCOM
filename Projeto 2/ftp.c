#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ftp.h"

void printAnswerFTP(char* command)
{
	int id = atoi(command);
	switch (id)
	{	//https://www.wikipedia.com/en/File_Transfer_Protocol#/FTP_reply_codes
		case 220:
			printf("220: Service ready for new user.\n");
			break;
	}
}

int sendCommandFTP(int controlSocket, char* command)
{
	int sent = write(controlSocket, command, strlen(command) + 1);
	return sent;
}

int receiveAnswerFTP(int controlSocket, char* answer)
{
	int nread = 0;
	do
	{
		int rd = read(controlSocket, answer, SIZE_OF_REPLY);
		if (rd == -1)
			return -1;
		answer += rd;
		nread += rd;
	} while (nread < SIZE_OF_REPLY);
	return nread;
}

ftpConnection_t createConnectionFTP(url_t url)
{
	ftpConnection_t ftp;
	ftp.controlSocket = createSocket(DEFAULT_CONTROL_PORT, url.ip);
	char* answer = (char*)calloc(4, sizeof(char));
	receiveAnswerFTP(ftp.controlSocket, answer);
	printAnswerFTP(answer);
	return ftp;
}

int authenticateFTP(ftpConnection_t ftp, url_t url)
{
	return 0;
}

int setDirectoryFTP(ftpConnection_t ftp, char* directory)
{
	return 0;
}

int destroyConnectionFTP(ftpConnection_t ftp)
{
	return close(ftp.controlSocket);
}
