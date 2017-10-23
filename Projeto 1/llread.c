#include "llread.h"

int serial_fd;
int turnPacket = 0;
char* lastData;

int llread(int fd, unsigned char* buf, int* duplicate)
{
	serial_fd = fd;

	//unsigned char* received = (unsigned char*)calloc(BUF_SIZE * 4, sizeof(unsigned char));

	lastData = calloc(BUF_SIZE, sizeof(char));
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
			/*printf("FACK\n");
			/nt j = 0;
			for(j; j < i; j++){
				printf("%x : ", buffer[j]);
			}
			printf("\n");*/
			processTram(buffer, buf, i, duplicate);
		}
		i += nread;
	}
	//memset(received, 0, i);
	printf("Read = %d\n", i);
	return i;
}

int destuffing (unsigned char* tram, unsigned char* buf, int size) { //tirar o size daqui
	int destuffing = 1;
	int i = 4, j = 0;
	while(destuffing){
		if (i == size || j == size)
			return j;
		if(tram[i] == 0x7E){
			destuffing = 0;
			buf[j] = tram[i];
			return j;
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

void processTram(unsigned char* tram, unsigned char* buf, int size, int* duplicate){
	int i = 0;
	for(i; i < size; i++){
		printf("%x-", tram[i]);
	}
	printf("\n");
	char bcc1 = tram[3];
	if (bcc1 != (tram[1] ^ tram[2]))
	{
		buf = NULL;
		sendREJ();
		return;
	}
	char bcc2;


	int j = destuffing(tram, buf, size);
	bcc2 = buf[j-1];
	int check = generateBCC(buf, j);

	if (bcc2 != check)
	{
		buf = NULL;
		sendREJ();
		return;
	}


	int isNew = memcmp(buf, lastData, j - 1) == 0 ? TRUE : FALSE;

	if(!isNew)
		printf("REPETIDO\n");

	if(isNew /*&& (turnPacket == 0 && tram[2] == 0x00) || (turnPacket == 1 && tram[2] == 0x40)*/){
		sendRR();
		turnPacket = ~turnPacket;
		memcpy(lastData, buf, j - 1);
	}
	else if(!isNew /*|| (turnPacket == 1 && tram[2] == 0x00) || (turnPacket == 0 && tram[2] == 0x40)*/){
		buf = NULL;
		sendRR();
	}
	else {
		buf = NULL;
		sendREJ();
	}
}

int generateBCC(unsigned char* buf, int size)
{
	char bcc = buf[0];
	int i;
	for (i = 1; i < size; i++)
		bcc ^= buf[i];
	return bcc;
}

void sendREJ()
{
	printf("MANDEI REJ\n");
	unsigned char rej[] = { 0x7E, 0x03, 0x01, 0x03, 0x7E };
	rej[2] = (turnPacket == 0 ? 0x01 : 0x81);
	rej[3] = rej[1] ^ rej[2];
	int n = write(serial_fd, rej, 5);
	if (n < 0)
		perror("Failed to write to serial port in sendREJ\n");
}

void sendRR()
{
	printf("MANDEI RR\n");
	unsigned char rr[] = { 0x7E, 0x03, 0x05, 0xF3, 0x7E };
	rr[2] = (turnPacket == 0 ? 0x05 : 0x85);
	rr[3] = rr[1] ^ rr[2];
	int n = write(serial_fd, rr, 5);
	if (n < 0)
		perror("Failed to write to serial port in sendRR\n");
}
