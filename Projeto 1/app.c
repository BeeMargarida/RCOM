#include "data_link.h"

int fileSize = 0;
unsigned char* filename;
int serial_fd = 0;
int serial_id = 0;
int lastCycle = 0;
int nSeq = 0; //is it right?
int fileDescriptor = 0;
int cnt = 0;


// RECEIVER
int startReceiver()
{
	serial_fd = llopen(serial_id, RECEIVER);

	int reading = TRUE;
	unsigned char* buf = calloc(BUF_SIZE, sizeof(unsigned char));
	int first = FALSE;
	int* duplicate = calloc(1, sizeof(int));
	while (reading)
	{
		int read = llread(serial_fd, buf, duplicate);

		if(buf == NULL){
			printf("Buffer is null\n");
			continue;
		}

		cnt += read;

		if (read < 0)
		{
			printf("Error reading from llread\n");
			return 1;
		}
		if (read == 0)
			continue;

		printf("Start of data packet on startReceiver: %x\n", buf[0]);
		if (buf[0] == DATA_START && first == FALSE)
		{
			printf("Processing trama START\n");
			first = TRUE;
			unpackStartPacket(buf);
		}
		else if (buf[0] == DATA_END && first == TRUE)
		{
			printf("Processing trama END\n");
			unpackEndPacket(buf);
			reading = FALSE;
		}
		else if (buf[0] == DATA_BLOCK && first == TRUE)
		{/*
			if(*duplicate){
				*duplicate = 0;
				continue;
			}*/
			printf("Processing trama DATA\n");
			unpackDataPacket(buf);
		}
		else
		{
			printf("Invalid trama\n");
			cnt--;
		}
	}
	printf("\ncnt = %d\n", cnt);
	return llclose(serial_fd);
}

void unpackStartPacket(unsigned char* buf)
{
	int i;
	if (buf[1] == FILESIZE)
	{
		int sizelength = buf[2];
		for (i = 3; i < sizelength + 3; i++)
		{
			fileSize << 8;
			fileSize |= buf[i];
		}
		printf("File size: %d bytes\n", fileSize);

	}
	if (buf[i] == FILENAME)
	{
		i++;
		int namelength = buf[i];
		filename = malloc(sizeof(unsigned char) * namelength);
		int j;
		int n = i;
		for (i += 1, j = 0; i <= namelength + n; i++, j++)
			filename[j] = buf[i];
		printf("File name: %s\n", filename);
	}

	fileDescriptor = open(filename, O_WRONLY | O_CREAT, MODE);
	if (fileDescriptor < 0)
	{
		printf("Error opening/creating file %s\n", filename);
	}
}

void unpackDataPacket(unsigned char* buf)
{
	int seqN = buf[1];
	printf("Processing packet %d\n", seqN);
	int n = 256 * buf[2] + buf[3];
	int i;
	int x = write(fileDescriptor, buf + 4, n);

	if (x != n)
	{
		printf("\nERROR\n");
	}

	printf("Wrote %d bytes\n\n", x);
	/*if(first == 1){
	for (i = 4; i < n + 6; i++)
		printf("%x : ", buf[i]);
	printf("\n");
	first++;
	}
	first++;*/
}

void unpackEndPacket(unsigned char* buf)
{
	printf("Last packet received\n");
	//close(fileDescriptor);
	return;
}



// SENDER




/**************cenas fixes****************************/
int getFileSize() {
	struct stat st;
	fstat(fileDescriptor, &st);
	return st.st_size;
}

control_packet_t createFirstEndPacket(int fsize, unsigned char * fileName, int id) {
	unsigned char size[4];
	size[0] = (fsize >> 24) & 0xFF;
	size[1] = (fsize >> 16) & 0xFF;
	size[2] = (fsize >> 8) & 0xFF;
	size[3] = fsize & 0xFF;

	unsigned char C = id == 0  ? 0x02 : 0x03; //flag de start
	unsigned char T1 = 0x00; //type = tamanho do ficheiro
	unsigned char L1 = 0x04; //tamanho de V1
	unsigned char T2 = 0x01; //type = nome do ficheiro
	unsigned char L2 = strlen(fileName); //tamanho do nome pinguim.gif

	unsigned char *packet = malloc((5+4+L2) * sizeof(unsigned char));
	packet[0] = C;
	packet[1] = T1;
	packet[2] = L1;
	memcpy(packet + 3, size, 4*sizeof(unsigned char));
	packet[7] = T2;
	packet[8] = L2;
	memcpy(packet + 9, fileName, strlen(fileName)*sizeof(unsigned char));

	control_packet_t controlPacket;
	controlPacket.params = packet;
	controlPacket.size = 9+strlen(fileName);

	return controlPacket;
}

int sendControlPacket(int SorR, int fsize, unsigned char *fname){
	control_packet_t packet;
	switch(SorR){
		case 0: //start
			packet = createFirstEndPacket(fsize, fname, SorR);
			break;

		case 1: //end
			packet = createFirstEndPacket(fsize, fname, SorR);
			break;
	}
	if(llwrite(serial_fd, packet)){
		printf("erro a enviar pacote de controlo\n");
		return -1;
	}
	printf("enviou o pacote de controlo corretamente\n");
	return 0;
}

control_packet_t createDataPacket(int fdimage, int nseq){
	unsigned char imageBuf[256] = {};
	int size = getImageData(imageBuf, fdimage);
	if(size < 0){
		return ;
	} else if(size < 256){
		lastCycle = 1;
	}
	unsigned char C = 0x01;
	unsigned char N = nseq;
	unsigned char L2;
	unsigned char L1;
	if(size == 256){
		L2 = 0x01;
		L1 = 0x00;
	} else {
		L2 = 0x00;
		L1 = size;
	}
	unsigned char *temp = malloc((4+size) * sizeof(unsigned char));
	temp[0] = C;
	temp[1] = N;
	temp[2] = L2;
	temp[3] = L1;
	memcpy(temp+4, imageBuf, size * sizeof(unsigned char));

	control_packet_t packet;
	packet.params = temp;
	packet.size = size+4;

	return packet;
}

control_packet_t sendDataPacket() {
	nSeq++;
	return (createDataPacket(fileDescriptor, nSeq));
}

int getImageData(unsigned char* buf, int fdimage) {
	int x = read(fdimage, buf, 256);
	printf("Agora li: %d\n", x);
	if(x < 0){
		printf("Error reading the file");
		return -1;
	}
	return x;
}

int startSender(unsigned char* fileName)
{
	serial_fd = llopen(serial_id, SENDER);

	fileDescriptor = open(fileName, O_RDONLY);
	if(fileDescriptor < 0){
		printf("Error opening file %s\n", fileName);
		return -1;
	}

	int fsize = getFileSize();

	//send start packet
	if(sendControlPacket(0, fsize, fileName))
		return -1;

	//send data packet
	while(!lastCycle){
		control_packet_t packet	= sendDataPacket();
		if(llwrite(serial_fd, packet)){
			return -1;
		}
	}

	//send end packet
	if(sendControlPacket(1, fsize, fileName))
		return -1;
	//llclose();
	//close(fd);
	return 0;
}
