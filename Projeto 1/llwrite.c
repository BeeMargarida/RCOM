#include "llwrite.h"

int tramaC1 = 0;

int checkResponse(char* response){

}

int sendPacket() {

}

char* stuffingData(unsigned char * buf, int *size){
	int n = *size;
	unsigned char *trama = malloc(512*sizeof(unsigned char));
	int stuffing = 1;
	int i = 0, j = 0;
	while(stuffing){
		if(buf[i] == 0x7E){
			trama[j] = 0x7D;
			trama[j+1] = 0x5E;
			size++;
			j+=2;
		}
		else if(buf[i] == 0x7D){
			trama[j] = 0x7D;
			trama[j+1] = 0x5D;
			size++;
			j+=2;
		}
		else {
			trama[j] = buf[i];
			j++;
		}
		i++;

		if (i == n)
			stuffing = 0;
	}/*
	for(int i = 0; i < *size; i++){
		printf("%x\n", trama[i]);
	}*/

	return trama;
}

char* createTramaI(struct tramaData* buf){
	char F = 0x7E;
	char A = 0x03;
	char C1 = (tramaC1 == 0 ? 0x00 : 0x40);
	char BCC1 = A ^ C1;
	char BCC2 = buf->trama[0];
	for(int i = 1; i < buf->size; i++){
		BCC2 ^= buf->trama[i];
	}
	struct tramaData *stuffedBuf = malloc(sizeof(struct tramaData));
	stuffedBuf->size = buf->size;
	//stuffedBuf->trama = malloc(512*sizeof(unsigned char));
	stuffedBuf->trama = stuffingData(buf->trama, &stuffedBuf->size);

	unsigned char* trama = malloc((7 + stuffedBuf->size) * sizeof(unsigned char));
	trama[0] = F; trama[1] = A; trama[2] = C1;
	trama[3] = BCC1;

	memcpy(trama + 4, stuffedBuf, stuffedBuf->size*sizeof(char));
	trama[buf->size + 5 -1] = BCC2;
	trama[buf->size + 6 -1] = F;
/*
	printf("FACK\n");
	for(int i = 0; i < stuffedBuf->size + 7; i++){
		printf("%x\n", trama[i]);
	}*/

	return trama;
}

int llwrite(int serial_fd, struct tramaData* buf) {
	char *tramaI = createTramaI(buf);
	tramaC1 = 1-tramaC1;

	int wrote = write(serial_fd, tramaI, 267);
	printf("Wrote %d bytes\n", wrote);
}
