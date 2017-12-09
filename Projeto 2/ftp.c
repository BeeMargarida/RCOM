#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "ftp.h"

int sendCommandFTP(int controlSocket, char* command)
{
	int sent = send(controlSocket, command, strlen(command), MSG_WAITALL);
	return sent;
}

int receiveAnswerFTP(int controlSocket, char* answer)
{
	int n = recv(controlSocket, answer, BUF_SIZE*3, 0);
	return n;
}

ftpConnection_t* createConnectionFTP(url_t url)
{
	printf("\nEstablishing connection to host with IP = %s\n", url.ip);
	ftpConnection_t *ftp = malloc(sizeof(ftpConnection_t));
	ftp->controlSocket = createSocket(DEFAULT_CONTROL_PORT, url.ip);

	char* answer = (char*)calloc(BUF_SIZE, sizeof(char));
	int received = receiveAnswerFTP(ftp->controlSocket, answer);
	if (received < 3 || strncmp(answer, "220", 3) != 0)
	{
		printf("Error establishing connection to %s\n", url.host);
		close(ftp->controlSocket);
		ftp->controlSocket = 0;
		return ftp;
	}
	printf("%s", answer);

	return ftp;
}

int authenticateFTP(ftpConnection_t *ftp, url_t url)
{
	char* command = calloc(BUF_SIZE, sizeof(char));
	char* answer = calloc(BUF_SIZE, sizeof(char));

	sprintf(command, "USER %s\r\n", url.username);
	int sent = sendCommandFTP(ftp->controlSocket, command);
	if (sent < strlen(command))
	{
		printf("Error sending command USER\n");
		return -1;
	}
	int received = receiveAnswerFTP(ftp->controlSocket, answer);
	if (received < 3 || strncmp(answer, "331", 3) != 0)
	{
		printf("Error logging in with username %s\n", url.username);
		return -1;
	}
	printf("%s", answer);

	sprintf(command, "PASS %s\r\n", url.password);
	sent = sendCommandFTP(ftp->controlSocket, command);
	if (sent < strlen(command))
	{
		printf("Error sending command PASS\n");
		return -1;
	}
	received = receiveAnswerFTP(ftp->controlSocket, answer);
	if (received < 3  || strncmp(answer, "230", 3) != 0)
	{
		printf("Error logging in with username %s\n", url.username);
		return -1;
	}
	printf("%s", answer);

	return 0;
}

int setDirectoryFTP(ftpConnection_t *ftp, char* directory)
{
	char* command = calloc(BUF_SIZE, sizeof(char));
	sprintf(command, "CWD %s\r\n", directory);
	char* answer = calloc(BUF_SIZE, sizeof(char));

	int sent = sendCommandFTP(ftp->controlSocket, command);
	if (sent < strlen(command))
	{
		printf("Error sending command CWD\n");
		return -1;
	}
	int received = receiveAnswerFTP(ftp->controlSocket, answer);
	if (received < 3 /*|| strstr(answer, "230") == NULL*/)
	{
		printf("Error switching to directory %s\n", directory);
		return -1;
	}
	printf("%s", answer);

	return 0;
}

int setPassiveModeFTP(ftpConnection_t *ftp)
{
	char *passive = calloc(BUF_SIZE, sizeof(char));
	sprintf(passive, "PASV\r\n");

	int sent = sendCommandFTP(ftp->controlSocket, passive);
	if (sent < strlen(passive))
	{
		printf("Error sending command PASV\n");
		return -1;
	}
	int received = receiveAnswerFTP(ftp->controlSocket, passive);
	if (received < 0/*strstr(answer, "227") == NULL*/)
	{
		printf("Error getting into passive mode\n");
		return -1;
	}
	printf("%s", passive);

	int h1, h2, h3, h4, p1, p2;
	if((sscanf(passive,"227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &h1,&h2,&h3,&h4,&p1,&p2)) < 0){
		printf("FTP response isn't the one expected");
		return -1;
	}

	char *newIP = calloc(BUF_SIZE, sizeof(char));
	sprintf(newIP, "%d.%d.%d.%d", h1,h2,h3,h4);
	int newPort = p1*256+p2;

	ftp->dataSocket = createSocket(newPort, newIP);
	if(ftp->dataSocket < 0){
		printf("Error entering passive mode\n");
		return -1;
	}
	return 0;
}

int retrieveFTP(ftpConnection_t *ftp, char* filename){
	char *retrieve = calloc(BUF_SIZE, sizeof(char));
	sprintf(retrieve, "RETR %s\r\n", filename);

	int sent = sendCommandFTP(ftp->controlSocket, retrieve);
	if (sent < strlen(retrieve))
	{
		printf("Error sending command RETR\n");
		return -1;
	}
	int received = receiveAnswerFTP(ftp->controlSocket, retrieve);
	if (received < 0)
	{
		printf("Error using the Retrieve command\n");
		return -1;
	}
	printf("%s", retrieve);
	return 0;
}

int downloadFTP(ftpConnection_t *ftp, char* filename)
{
	FILE * file;

	file = fopen(filename, "w");
	if(file < 0){
		printf("Error opening file with the name %s\n", filename);
		return -1;
	}
	char buf[BUF_SIZE];
	int readBytes;
	while((readBytes = read(ftp->dataSocket, buf, sizeof(buf)))){
		if(readBytes < 0){
			printf("Error reading from the socket\n");
			return -1;
		}

		if((readBytes = fwrite(buf, readBytes, 1, file)) < 0){
			printf("Error writing\n");
			return -1;
		}
	}

	fclose(file);
	close(ftp->dataSocket);
	return 0;
}

int destroyConnectionFTP(ftpConnection_t *ftp)
{
	char *disconnect = calloc(BUF_SIZE, sizeof(char));

	int received = receiveAnswerFTP(ftp->controlSocket, disconnect);
	if (received < 0)
	{
		printf("Error disconnecting\n");
		return -1;
	}
	sprintf(disconnect, "QUIT\r\n");
	int sent = sendCommandFTP(ftp->controlSocket, disconnect);
	if (sent < strlen(disconnect))
	{
		printf("Error sending command QUIT\n");
		return -1;
	}
	return close(ftp->controlSocket);
}
