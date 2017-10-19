#include "llwrite.h"

int turn = 0;
int tries = 0;

int checkResponse(unsigned char* response){

}

int sendPacket() {

}

unsigned char* stuffingData(unsigned char * buf, int *size){
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

control_packet_t createTramaI(control_packet_t packet){
	unsigned char F = 0x7E;
	unsigned char A = 0x03;
	unsigned char C1 = (turn == 0 ? 0x00 : 0x40);
	unsigned char BCC1 = A ^ C1;
	unsigned char BCC2 = packet.params[0];
	printf("Size bitch%d\n", packet.size);
	for(int i = 1; i < packet.size; i++){
		BCC2 ^= packet.params[i];
	}

	control_packet_t stuffedPacket;
	stuffedPacket.size = packet.size;
	stuffedPacket.params = stuffingData(packet.params, &stuffedPacket.size);

	unsigned char* trama = malloc((6 + stuffedPacket.size) * sizeof(unsigned char));
	trama[0] = F; trama[1] = A; trama[2] = C1;
	trama[3] = BCC1;

	memcpy(trama + 4, stuffedPacket.params, stuffedPacket.size*sizeof(unsigned char));
	trama[stuffedPacket.size + 5 -1] = BCC2;
	trama[stuffedPacket.size + 6 -1] = F;

	control_packet_t packetI;
	packetI.params = trama;
	packetI.size = stuffedPacket.size + 6;

	printf("FACK\n");
	for(int i = 0; i < stuffedPacket.size + 6; i++){
		printf("%x\n", packetI.params[i]);
	}
	return packetI;
}

void sendTrama(int serial_fd, control_packet_t packet){
	int wrote = write(serial_fd, packet.params, packet.size);
	printf("Wrote %d bytes\n", wrote);
}

int waitForAnswer(int serial_fd){
	int reading = TRUE;
	int nread;
	unsigned char rr[5] = {0x7e, 0x03, 0x05, 0x01, 0x7e};
	unsigned char rej[5] = {0x7e, 0x03, 0x05, 0x01, 0x7e};

	unsigned char answer[5] = {};
	while (reading)
	{
		nread = read(serial_fd, answer, 5);
		printf("Read %d bytes\n", nread);

		if (nread < 0)
		{
			printf("Error reading answer");
			return 1;
		}
		int i = 0; 
		for(i; i < nread; i++){
			printf("%x\n", answer[i]);
			if(answer[i] != rr[i]){
				printf("deu merda\n");
				break;
			}
		}
		if(i < 5){
			for(i = 0; i < nread; i++){
				if(answer[i] != rej[i]){
					printf("merdou mesmo\n");
					reading = FALSE; //return error
					break;
				}
			}
		}
		if(i == 5){
			printf("esta fixe\n");
			reading = FALSE;
			break;
		}
	}
	//return 1 ou 0 consoante o "turno"
	return 0;
}

int verifyAnswer(int answer){
	if(answer != turn){
		return 1;
	}
	else{
		return 0;
	}
}

int llwrite(int serial_fd, control_packet_t packet) {
	control_packet_t packetI = createTramaI(packet);
	turn = 1 - turn;

	sendTrama(serial_fd, packetI);
	int i = waitForAnswer(serial_fd);
	if(verifyAnswer(i)){
		printf("Tem erro na resposta\n");
		if(tries < 4){
			sendTrama(serial_fd, packetI);
			tries++;
		}
		if(tries == 4){
			return 1;
		}
	}
	tries = 0;
	printf("Correu bem\n");

	return 0;
}
