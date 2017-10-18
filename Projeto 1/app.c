#include "data_link.h"
#include "llread.h"
#include "llwrite.h"

int fileSize = 0;
char* filename;
int fd = 0;
int serial_fd = 0;
int serial_id = 0;
int lastCycle = 0;

int nSeq = 0; //is it right?


// RECEIVER

int startReceiver()
{
	serial_fd = llopen(serial_id, RECEIVER);

	int reading = 1;
	char buf[512];
	int first = 0;
	while (reading)
	{
		int read = llread(serial_fd, buf);
		if (read < 0)
		{
			printf("Error reading from llread\n");
			return 1;
		}
		if (read == 0)
			continue;

		if (buf[0] == DATA_START && first == 0)
		{
			first = 1;
			unpackStartPacket(buf);

		}
		else if (buf[0] == DATA_END)
		{
			unpackEndPacket(buf);
			reading = 0;
		}
		else
			unpackDataPacket(buf);
	}

	return llclose(serial_fd);
}

void unpackStartPacket(char* buf)
{
	int i;
	if (buf[1] == FILESIZE)
	{
		int sizelength = buf[2];
		for (i = 3; i < sizelength; i++)
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
		filename = malloc(sizeof(char) * namelength);
		int j;
		for (i += 1, j = 0; i < namelength; i++, j++)
			filename[j] = buf[i];
		printf("File name: %s\n", filename);
	}

	fd = open(filename, O_WRONLY | O_CREAT, MODE);
	if (fd < 0)
	{
		printf("Error opening/creating file %s\n", filename);
	}
}

void unpackDataPacket(char* buf)
{
	int seqN = buf[1];
	printf("Processing packet %d\n", seqN);
	int n = 256 * buf[2] + buf[3];
	int i;
	for (i = 4; i < n; i++)
		write(fd, buf[i], 1);
}

void unpackEndPacket(char* buf)
{
	printf("Last packet received\n");
	return;
}



// SENDER
int getfileSize() {
	struct stat st;
	fstat(fd, &st);
	fileSize = st.st_size;
	return 0;
}

int getImageData(unsigned char* buf) {
	int x = read(fd, buf, 256);
	if(x < 0){
		printf("Error reading the file");
		return -1;
	}
	return x;
}

unsigned char* createTramaStartEnd(unsigned char *fsize, char *fname, int id){
	char C = id == 0  ? 0x02 : 0x03; //flag de start
	char T1 = 0x00; //type = tamanho do ficheiro
	char L1 = 0x04; //tamanho de V1
	char T2 = 0x01; //type = nome do ficheiro
	char L2 = 0x0B; //tamanho do nome pinguim.gif

	unsigned char *trama = malloc(19 * sizeof(unsigned char));
	trama[0] = C; trama[1] = T1; trama[2] = L1;
	memcpy(trama + 3, fsize, 4*sizeof(unsigned char));
	trama[7] = T2;
	trama[8] = L2;
	memcpy(trama + 9, fname, 11*sizeof(unsigned char));
	return trama;
}

int createFirstEndPacket(int fd) {
	getfileSize();
	unsigned char comp[4];
	comp[0] = (fileSize >> 24) & 0xFF;
	comp[1] = (fileSize >> 16) & 0xFF;
	comp[2] = (fileSize >> 8) & 0xFF;
	comp[3] = fileSize & 0xFF;
	createTramaStartEnd(comp, filename, 0);
}

struct tramaData * createDataPacket(int n){
	unsigned char imageBuf[512] = {};
	int size = getImageData(imageBuf);
	if(size < 0){
		return 0;
	} else if(size < 256){
		lastCycle = 1;
	}
	unsigned char C = 0x01;
	unsigned char N = n;
	unsigned char L2;
	unsigned char L1;
	if(size == 256){
		L2 = 0x01;
		L1 = 0x00;
	} else {
		L2 = 0x00;
		L1 = size;
	}
	unsigned char *trama = malloc((4+size) * sizeof(unsigned char));
	trama[0] = C;
	trama[1] = N;
	trama[2] = L2;
	trama[3] = L1;
	memcpy(trama+4, imageBuf, size * sizeof(unsigned char));
	struct tramaData *td = malloc(sizeof(struct tramaData));
	td->trama = trama;
	td->size = size + 4;

	return td;
}

struct tramaData * getDataPacket(int f, int n){
	fd = f;
	struct tramaData *td = createDataPacket(n);
	return td;
}

int startSender(char* fileName)
{
	serial_fd = llopen(serial_id, SENDER);
	
	filename = malloc(sizeof(char) * strlen(fileName));
	filename = fileName;
	int fdimage = open(filename, O_RDONLY);
	if (fdimage < 0)
	{
		printf("Error: file %s does not exist\n", filename);
		return 1;
	}
	//serial_fd = llopen(serial_id, SENDER);
	/*while(!lastCycle){
		struct tramaData* buf = malloc(sizeof(struct tramaData));
		buf = getDataPacket(fdimage, nSeq);
		if(buf == 0){
			printf("Error reading the file\n");
			return -1;
		}
		llwrite(fdimage, serial_fd, buf);
		nSeq++;
		char *response = malloc(5*sizeof(char));
		int x = read(serial_fd, response, 5);
		checkResponse(response);

	}*/
	struct tramaData* buf = malloc(sizeof(struct tramaData));
	buf = getDataPacket(fdimage, nSeq);

	llwrite(serial_fd, buf);

	nSeq++;
	//llclose();
	//close(fd);
	return 0;
}