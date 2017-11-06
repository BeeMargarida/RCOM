#include "application_layer.h"
#include "data_link_layer.h"

int fileSize = 0;
int currentSize = 0;
int lastCycle = 0;
int nSeq = 0;
int cnt = 0;

int serial_fd;
int fileDescriptor;

void unpackStartPacket(unsigned char* buf);
void unpackDataPacket(unsigned char* buf);
void unpackEndPacket(unsigned char* buf);
void writeDataStart(unsigned char* buf);
void writeDataBlock(unsigned char* buf);
int getFileSize();
int getImageData(unsigned char* buf, int fdimage);
control_packet_t createFirstEndPacket(int fsize, char* fileName, int id);
control_packet_t createDataPacket(int fdimage, int nseq);

void printProgressBar()
{
	clearScreen();
	printf("Progress: [");
	float percentage = (float)currentSize / fileSize;
	int numberOfSymbols = (percentage * 20*100) / 100;
	int i;
	for (i = 0; i < numberOfSymbols; i++)
	 	printf("█");
	for (i = 0; i < 20 - numberOfSymbols; i++)
		printf(".");
	printf("] %.1f%%\n", percentage*100.0);
}

void printStatistics(char* filename){
	printf("File %s transfered successfully\n", filename);
	printf("Final size: %d bytes\n", currentSize);
	printf("Distinct data packets: %d\n", cnt);
}

void printStatisticsSender(char* filename, double time_diff, int fsize, statistics_t *stats){
	printf("File %s transfered successfully in %.02f seconds\n", filename, time_diff);
	printf("Total data packets: %d\n", stats->packets);
	printf("Distinct data packets: %d\n", cnt);
	printf("RR received: %d\n", stats->rr);
	printf("REJ received: %d\n", stats->rej);
	printf("Connection Cap: %.02f bits/sec\n", (double)((fsize*8) / time_diff));
}

int startReceiver(int serial_no)
{
	serial_fd = llopen(serial_no, RECEIVER);

	int reading = TRUE;
	int first = FALSE;
	while (reading)
	{
		unsigned char* buf = malloc(BUF_MAX*sizeof(unsigned char));
		int read = llread(serial_fd, buf);

		if(buf == NULL){
			continue;
		}

		if (read < 0)
		{
			printf("Error reading from llread\n");
			return 1;
		}
		if (read == 0)
			continue;

		if (buf[0] == DATA_START && first == FALSE)
		{
			first = TRUE;
			unpackStartPacket(buf);
		}
		else if (buf[0] == DATA_END && first == TRUE)
		{
			unpackEndPacket(buf);
			reading = FALSE;
		}
		else if (buf[0] == DATA_BLOCK && first == TRUE)
		{
			unpackDataPacket(buf);
		}
	}
	return llclose(serial_fd, RECEIVER);
}

void unpackStartPacket(unsigned char* buf)
{
	int i;
	if (buf[1] == FILESIZE)
	{
		int sizelength = buf[2];
		for (i = 3; i < sizelength + 3; i++)
		{
			fileSize <<= 8;
			fileSize |= buf[i];
		}
	}
	char* filename;
	if (buf[i] == FILENAME)
	{
		i++;
		int namelength = buf[i];
		filename = malloc(sizeof(unsigned char) * namelength);
		int j;
		int n = i;
		for (i += 1, j = 0; i <= namelength + n; i++, j++)
			filename[j] = buf[i];
	}

	fileDescriptor = open((char*)filename, O_WRONLY | O_CREAT, MODE);
	if (fileDescriptor < 0)
	{
		printf("Error opening/creating file %s\n", filename);
	}
}

void unpackDataPacket(unsigned char* buf)
{
	int seqN = buf[1];
	int n = 256 * buf[2] + buf[3];
	int x = write(fileDescriptor, buf + 4, n);

	if (x != n)
		printf("Failure writing the proper number of bytes of packet %d\n", seqN);

	currentSize += x;
	cnt++;
	printProgressBar();
}

void unpackEndPacket(unsigned char* buf)
{
	int i;
	int fileSizeFinal = 0;

	if (buf[1] == FILESIZE)
	{
		int sizelength = buf[2];

		for (i = 3; i < sizelength + 3; i++)
		{
			fileSizeFinal <<= 8;
			fileSizeFinal |= buf[i];
		}
	}
	if (fileSize != fileSizeFinal)
		printf("Initial and final file sizes do not match\n");

	char* filename;
	if (buf[i] == FILENAME)
	{
		i++;
		int namelength = buf[i];
		filename = malloc(sizeof(unsigned char) * namelength);
		int j;
		int n = i;
		for (i += 1, j = 0; i <= namelength + n; i++, j++)
			filename[j] = buf[i];
	}
	if (access(filename, F_OK) != -1)
		printStatistics(filename);

	return;
}


int getFileSize() {
	struct stat st;
	fstat(fileDescriptor, &st);
	return st.st_size;
}

control_packet_t createFirstEndPacket(int fsize, char* fileName, int id) {
	unsigned char size[4];
	size[0] = (fsize >> 24) & 0xFF;
	size[1] = (fsize >> 16) & 0xFF;
	size[2] = (fsize >> 8) & 0xFF;
	size[3] = fsize & 0xFF;

	unsigned char C = id == 0  ? 0x02 : 0x03; 	//flag de start
	unsigned char T1 = 0x00; 					//type = tamanho do ficheiro
	unsigned char L1 = 0x04; 					//tamanho de V1
	unsigned char T2 = 0x01; 					//type = nome do ficheiro
	unsigned char L2 = strlen(fileName); 		//tamanho do nome pinguim.gif

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

int sendControlPacket(int SorR, int fsize, char* fname, statistics_t *stats){
	control_packet_t packet;
	switch(SorR){
		case 0: //start
			packet = createFirstEndPacket(fsize, fname, SorR);
			break;

		case 1: //end
			packet = createFirstEndPacket(fsize, fname, SorR);
			break;
	}
	if(llwrite(serial_fd, packet, stats)){
		return -1;
	}
	cnt++;
	return 0;
}

control_packet_t createDataPacket(int fdimage, int nseq){
	unsigned char imageBuf[BUF_SIZE] = {};
	int size = getImageData(imageBuf, fdimage);
	if(size < 0){
		control_packet_t packet;
		return packet;
	} else if(size < BUF_SIZE){
		lastCycle = 1;
	}
	unsigned char C = 0x01;
	unsigned char N = nseq;
	unsigned char L2;
	unsigned char L1;
	L2 = size / 256;
	L1 = size % 256;
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
	int x = read(fdimage, buf, BUF_SIZE);
	if(x < 0){
		printf("Error reading the file");
		return -1;
	}
	return x;
}

int startSender(char* fileName, int serial_no)
{
	serial_fd = llopen(serial_no, SENDER);

	fileDescriptor = open(fileName, O_RDONLY);
	if(fileDescriptor < 0){
		printf("Error opening file %s\n", fileName);
		return -1;
	}

	struct timespec start,end;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);

	statistics_t *stats = malloc(sizeof(statistics_t));
	stats->packets = 0;
	stats->rr = 0;
	stats->rej = 0;

	int fsize = getFileSize();

	//send start packet
	if(sendControlPacket(0, fsize, fileName, stats))
		return -1;

	//send data packet
	while(!lastCycle){
		control_packet_t packet	= sendDataPacket();
		if(llwrite(serial_fd, packet, stats)){
			return -1;
		}
		cnt++;
	}

	//send end packet
	if(sendControlPacket(1, fsize, fileName, stats))
		return -1;

	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	double elapsedTime = (end.tv_sec - start.tv_sec) * 1000000.00 + (end.tv_nsec - start.tv_nsec) / 1000.00;

	clearScreen();
	stats->rej = -(cnt - stats->packets);
	printStatisticsSender(fileName, elapsedTime/1000000, fsize, stats);

	return llclose(serial_fd, SENDER);
}
