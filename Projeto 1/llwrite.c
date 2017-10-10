#include "header.h"

int fileSize = 0;
int fd = 0;
int c1 = 0;

int getFileSize() {
	fseek(fd, 0, SEEK_END);
	fileSize = ftell(fd);
	rewind(fd);
}

int getImageData(char* buf) {
	int x = read(fd, buf, 256);
	if(x < 0){
		printf("Error reading the file");
		return -1;
	}
}

int createFirstTram() {
	getFileSize();
	//char firstTram[]= {0x02, 0}
}

int createDataTram(char* buf) {
	char imageBuf[256] = {};
	getImageData(imageBuf);
	//buf = {0x76, 0x03, c1,} //0x00 not sure BCC1
}


int llwrite(int f) {
	fd = f;
}