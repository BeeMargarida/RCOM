#include "llread.h"
#include <stdlib.h>
#include <string.h>

char* lastData[BUF_SIZE];
int serial_fd;

int llread(int fd, char* buf)
{
	serial_fd = fd;

	unsigned char* received = (unsigned char*)calloc(BUF_SIZE, sizeof(unsigned char));

	int reading = TRUE;
	int nread;
	int i = 0;
	while (reading)
	{
		nread = read(serial_fd, received + i, BUF_SIZE);
		if (nread < 0)
		{
			printf("Error reading from serial port on llread");
			return 1;
		}
		if (nread == 0){
			reading = FALSE;
			printf("Size: %d Trama:\n", i);
			processTram(received, buf);			
		}	
		i += nread;
	}
	return i;
}

int destuffing (unsigned char* tram, unsigned char* buf) {
	int destuffing = 1;
	int i = 4, j = 0;
	while(destuffing){
		if(tram[i] == 0x7D & tram[i + 1] == 0x5E){
			buf[j] = 0x7E;
			j++;
			i+=2;
		}
		else if(tram[i] == 0x7D & tram[i + 1] == 0x5D){
			buf[j] = 0x7D;
			j++;
			i+=2;
		}
		else if(tram[i] == 0x7E){
			destuffing = 0;
			buf[j] = tram[i];
			break;
		}
		else {
			buf[j] = tram[i];
			j++;
			i++;
		}
	}
	return j;
}

void processTram(unsigned char* tram, unsigned char* buf)
{
	char bcc1 = tram[3];
	if (bcc1 == (tram[1] ^ tram[2]) && tram[2] == 0x03)
		return;
	char bcc2;
	
	int j = destuffing(tram, buf);

	int check = generateBCC(buf, j);
	if (bcc2 != check)
		sendREJ();

	int isNew = memcmp(buf, lastData, j - 1) == 0 ? TRUE : FALSE;

	if (isNew)
	{
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
	char rej[] = { 0x7E, 0x03, 0x01, 0x03, 0x7E };
	int n = write(serial_fd, rej, 5);
	if (n < 0)
		perror("Failed to write to serial port in sendREJ\n");
}

void sendRR()
{
	char rr[] = { 0x7E, 0x03, 0x05, 0xF3, 0x7E };
	int n = write(serial_fd, rr, 5);
	if (n < 0)
		perror("Failed to write to serial port in sendRR\n");
}
