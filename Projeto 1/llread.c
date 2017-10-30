#include "data_link_layer.h"

int serial_fd;
int turnPacket = 0;
char* lastData;

unsigned char generateBCC(unsigned char* buf, int size);
int destuffing(unsigned char* tram, unsigned char* buf, int size);
void processTram(unsigned char* tram, unsigned char* buf, int size);
void sendREJ();
void sendRR();

int llread(int fd, unsigned char* buf)
{
	serial_fd = fd;
	lastData = calloc(BUF_SIZE, sizeof(char));
	int reading = TRUE;
	int nread;
	unsigned char* buffer = calloc(BUF_SIZE*2, sizeof(unsigned char));
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
	return i;
}

int destuffing(unsigned char* tram, unsigned char* buf, int size) {
	int destuffing = 1;
	int i = 4, j = 0;
	while(destuffing){
		if (i == size || j == size)
			return j;
		if(tram[i] == 0x7E){
			destuffing = 0;
			return j;
		}
		else if(tram[i] == 0x7D && tram[i + 1] == 0x5E){
			buf[j] = 0x7E;
			j++;
			i+=2;
		}
		else if(tram[i] == 0x7D && tram[i + 1] == 0x5D){
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

void processTram(unsigned char* tram, unsigned char* buf, int size){
	char bcc1 = tram[3];
	if (bcc1 != (tram[1] ^ tram[2]))
	{
		free(buf);
		sendREJ();
		return;
	}
	int j = destuffing(tram, buf, size);

	unsigned char bcc2 = buf[j - 1];
	int check = generateBCC(buf, j);

	if (bcc2 != check)
	{
		free(buf);
		sendREJ();
		return;
	}

	if((turnPacket == 0 && tram[2] == 0x00) || (turnPacket == 1 && tram[2] == 0x40)){
		turnPacket = turnPacket == 1 ? 0 : 1;
		sendRR();
		memcpy(lastData, buf, j - 1);
		return;
	}
	else if((turnPacket == 1 && tram[2] == 0x00) || (turnPacket == 0 && tram[2] == 0x40)){
		free(buf);
		sendRR();
		return;
	}
	else {
		free(buf);
		sendREJ();
		return;
	}
}

unsigned char generateBCC(unsigned char* buf, int size)
{
	unsigned char bcc = buf[0];
	int i;
	for (i = 1; i < size - 1; i++)
	{
		bcc ^= buf[i];
	}
	return bcc;
}

void sendREJ()
{
	unsigned char rej[] = { 0x7E, 0x03, 0x01, 0x03, 0x7E };
	rej[2] = (turnPacket == 0 ? 0x01 : 0x81);
	rej[3] = rej[1] ^ rej[2];
	int n = write(serial_fd, rej, 5);
	if (n < 0)
		perror("Failed to write to serial port in sendREJ\n");
}

void sendRR()
{
	unsigned char rr[] = { 0x7E, 0x03, 0x05, 0xF3, 0x7E };
	rr[2] = (turnPacket == 0 ? 0x05 : 0x85);
	rr[3] = rr[1] ^ rr[2];
	int n = write(serial_fd, rr, 5);
	if (n < 0)
		perror("Failed to write to serial port in sendRR\n");
}
