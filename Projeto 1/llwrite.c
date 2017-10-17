#include "llwrite.h"

int tramaC1 = 0;
int nSeq = 1;

char* createTramaI(struct tramaData* buf){
	char F = 0x7E;
	char A = 0x03;
	char C1 = (tramaC1 == 0 ? 0x00 : 0x40);
	char BCC1 = A ^ C1;
	char BCC2 = buf->trama[0];
	for(int i = 1; i < buf->size; i++){
		BCC2 ^= buf->trama[i];
	}
	char* trama = malloc((7 + buf->size) * sizeof(char));
	trama[0] = F; trama[1] = A; trama[2] = C1;
	trama[3] = BCC1;
	memcpy(trama + 4, buf->trama, buf->size*sizeof(char));
	trama[buf->size + 5 -1] = BCC2;
	trama[buf->size + 6 -1] = F;
	/*for(int i = 0; i < buf->size + 6; i++){
		printf("%x\n", trama[i]);
	}*/
	return trama;
}

int llwrite(int fdimage, int serial_fd) {
	struct tramaData* buf = getDataPacket(fdimage, nSeq);
	nSeq++;
	char *tramaI = createTramaI(buf);
	tramaC1 = ~tramaC1;
	//char *lel = "bffvbwehofbaweuipfbaiofbaeiogb";
	
	write(serial_fd, tramaI, 267);
}