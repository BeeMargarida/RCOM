#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ftp.h"

int sendCommandFTP(int controlSocket, char* command)
{
	int sent = write(controlSocket, command, strlen(command));
	return sent;
}

int receiveAnswerFTP(int controlSocket, char* answer)
{
	int nread = 0;
	int rd;
	do
	{
		rd = read(controlSocket, answer + nread, BUF_SIZE*3);
		if (rd == -1)
			return -1;
		nread += rd;
	} while (answer[nread - 1] != '\n');

	return nread;
}

ftpConnection_t createConnectionFTP(url_t url)
{
	printf("\nEstablishing connection to host with IP = %s\n", url.ip);
	ftpConnection_t ftp;
	ftp.controlSocket = createSocket(DEFAULT_CONTROL_PORT, url.ip);

	char* answer = (char*)calloc(BUF_SIZE, sizeof(char));
	int received = receiveAnswerFTP(ftp.controlSocket, answer);
	if (received < 3 || strncmp(answer, "220", 3) != 0)
	{
		printf("Error establishing connection to %s\n", url.host);
		close(ftp.controlSocket);
		ftp.controlSocket = 0;
		return ftp;
	}
	printf("%s", answer);

	return ftp;
}

int authenticateFTP(ftpConnection_t ftp, url_t url)
{
	char* command = calloc(BUF_SIZE, sizeof(char));
	char* answer = calloc(BUF_SIZE, sizeof(char));

	sprintf(command, "USER %s\r\n", url.username);
	int sent = sendCommandFTP(ftp.controlSocket, command);
	if (sent < strlen(command))
	{
		printf("Error sending command USER\n");
		return -1;
	}
	int received = receiveAnswerFTP(ftp.controlSocket, answer);
	if (received < 3 || strncmp(answer, "331", 3) != 0)
	{
		printf("Error logging in with username %s\n", url.username);
		return -1;
	}
	printf("%s", answer);

	sprintf(command, "PASS %s\r\n", url.password);
	sent = sendCommandFTP(ftp.controlSocket, command);
	if (sent < strlen(command))
	{
		printf("Error sending command PASS\n");
		return -1;
	}
	received = receiveAnswerFTP(ftp.controlSocket, answer);
	if (received < 3  || strncmp(answer, "230", 3) != 0)
	{
		printf("Error logging in with username %s\n", url.username);
		return -1;
	}
	printf("%s", answer);

	return 0;
}

int setDirectoryFTP(ftpConnection_t ftp, char* directory)
{
	char* command = calloc(BUF_SIZE, sizeof(char));
	sprintf(command, "CWD %s\r\n", directory);
	char* answer = calloc(BUF_SIZE, sizeof(char));

	int sent = sendCommandFTP(ftp.controlSocket, command);
	if (sent < strlen(command))
	{
		printf("Error sending command CWD\n");
		return -1;
	}
	int received = receiveAnswerFTP(ftp.controlSocket, answer);
	if (received < 3 /*|| strstr(answer, "230") == NULL*/)
	{
		printf("Error switching to directory %s\n", directory);
		return -1;
	}
	printf("%s", answer);

	return 0;
}

int destroyConnectionFTP(ftpConnection_t ftp)
{
	return close(ftp.controlSocket);
}
