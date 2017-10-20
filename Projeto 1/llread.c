#include "llread.h"
#include <stdlib.h>
#include <string.h>

unsigned char* lastData[BUF_SIZE];
int serial_fd;
int turn = 0;
/*
int find7E(unsigned char* array, int size)
{
	int i;
	for (i = 1; i < size; i++)
	{
		if (array[i] == 0x7E)
			return TRUE;
	}
	return FALSE;
}

unsigned char* divideBuffers(int length)
{
	unsigned char* trama = calloc(BUF_SIZE, sizeof(unsigned char));
	trama[0] = buffer[0];

	printf("ok");
	int notFound7E = TRUE;
	int i = 1;
	while (notFound7E)
	{
		trama[i] = buffer[i];
		if (trama[i] == 0x7E)
			notFound7E = FALSE;
		i++;
	}*/
/*
	printf("Trama: ");
	for (int j = 0; j < i; j++)
		printf("%x ", trama[j]);*/
/*
	currIndex = length - i - 1;
	memcpy(buffer, buffer + i, currIndex);
/*
printf("\nResto: ");
for (int j = 0; j < currIndex; j++)
	printf("%x ", buffer[j]);*/
/*
	printf("trama[0] = %d\n", trama[0]);
	return trama;
}*/

int llread(int fd, char* buf)
{
	serial_fd = fd;

	//unsigned char* received = (unsigned char*)calloc(BUF_SIZE * 4, sizeof(unsigned char));

	int reading = TRUE;
	int nread;
	unsigned char* buffer = calloc(BUF_SIZE, sizeof(unsigned char));
	int i = 0;
	while (reading)
	{
		nread = read(serial_fd, buffer + i, 1);
		if (nread < 0)
		{
			printf("Error reading from serial port on llread");
			return 1;
		}
		if (i != 0 && buffer[i] == 0x7E){
			reading = FALSE;
			processTram(buffer, buf, i);
		}
		i += nread;
	}
	//memset(received, 0, i);
	printf("Read = %d\n", i);
	return i;
}

int destuffing (unsigned char* tram, unsigned char* buf, int size) {
	int destuffing = 1;
	int i = 4, j = 0;
	while(destuffing){
		if (i == size || j == size)
			return j;
		if(tram[i] == 0x7E){
			destuffing = 0;
			buf[j] = tram[i];
			break;
		}
		else if(tram[i] == 0x7D & tram[i + 1] == 0x5E){
			buf[j] = 0x7E;
			j++;
			i+=2;
		}
		else if(tram[i] == 0x7D & tram[i + 1] == 0x5D){
			buf[j] = 0x7D;
			j++;
			i+=2;
		}
		else {
			buf[j] = tram[i];
			j++;
			i++;
		}
	}
	return j;
}

void processTram(unsigned char* tram, unsigned char* buf, int size)
{
	char bcc1 = tram[3];
	if (bcc1 != (tram[1] ^ tram[2]))
	{
		sendREJ();
		return;
	}
	char bcc2;

	int j = destuffing(tram, buf, size);

	int check = generateBCC(buf, j);

	if (bcc2 != check)
	{
		sendREJ();
		return;
	}

	int isNew = memcmp(buf, lastData, j - 1) == 0 ? TRUE : FALSE;

	if (isNew)
	{
		turn = ~turn;
		sendRR();
		memcpy(lastData, buf, j - 1);
	}
	else
	{
		buf = NULL;
		sendRR();
	}
}

int generateBCC(char* buf, int size)
{
	char bcc = buf[0];
	int i;
	for (i = 1; i < size; i++)
		bcc ^= buf[i];
	return bcc;
}

void sendREJ()
{
	printf("Sending REJ\n");
	char rej[] = { 0x7E, 0x03, 0x01, 0x03, 0x7E };
	int n = write(serial_fd, rej, 5);
	if (n < 0)
		perror("Failed to write to serial port in sendREJ\n");
}

void sendRR()
{
	printf("Sending RR\n");
	char rr[] = { 0x7E, 0x03, 0x05, 0xF3, 0x7E };
	int n = write(serial_fd, rr, 5);
	if (n < 0)
		perror("Failed to write to serial port in sendRR\n");
}
