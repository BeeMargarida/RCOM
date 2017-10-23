#include "llwrite.h"

int turn = 0;
int tries = 0;
int numPacket = 0;
int first = 0;

//isto funciona bem
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
	}
	return trama;
}

//isto funciona bem
control_packet_t createTramaI(control_packet_t packet){
	unsigned char F = 0x7E;
	unsigned char A = 0x03;
	unsigned char C1 = (turn == 0 ? 0x00 : 0x40);
	unsigned char BCC1 = A ^ C1;
	unsigned char BCC2 = packet.params[0];
	int i = 1;
	for(i; i < packet.size; i++){
		BCC2 ^= packet.params[i];
	}

	int stuffedSize = packet.size;
	//printf("A: %d  Size: %d\n",asd,stuffedSize);
	//asd++;
	unsigned char * stuffedPacket = stuffingData(packet.params, &stuffedSize);

	unsigned char* trama = malloc((6 + stuffedSize) * sizeof(unsigned char));
	trama[0] = F; trama[1] = A; trama[2] = C1; trama[3] = BCC1;
	memcpy(trama + 4, stuffedPacket, stuffedSize*sizeof(unsigned char));
	trama[stuffedSize + 5 - 1] = BCC2;
	trama[stuffedSize + 6 - 1] = F;

	control_packet_t packetI;
	packetI.params = trama;
	packetI.size = stuffedSize + 6;

	/*int j = 0;
	for(j; j < packetI.size + 5; j++){
		printf("%x : ", packetI.params[j]);
	}
	printf("\n");*/

	return packetI;
}

//isto funciona bem
void sendTrama(int serial_fd, control_packet_t packet){
	int i = 0;
	printf("PACKET: %d\n", numPacket);
	printf("SIZE: %d\n", packet.size);
	if(numPacket == 40){
		for (i; i < packet.size; i++) {
			printf("i%d %x : ", i, packet.params[i]);
		}
		printf("\n");
	}


	int wrote = write(serial_fd, packet.params, packet.size);
	numPacket++;
}

//isto se calhar nao
int waitForAnswer(int serial_fd){
	int reading = TRUE;
	int nread;

	unsigned char rr[5] = {0x7e, 0x03, 0x01 , 0x01, 0x7e};
	unsigned char rej[5] = {0x7e, 0x03, 0x01, 0x01, 0x7e};
	rr[2] = (turn == 0 ? 0x05 : 0x85);
	rej[2] = (turn == 0 ? 0x01 : 0x81);

	unsigned char answer[5] = {};
	int i = 0;

	nread = read(serial_fd, answer, 5);
	if (nread < 0)
	{
		printf("Error reading answer");
		return 1;
	}
	/*for(int j = 0; j < 5; j++)
		printf("%x\n", answer[j]);*/


	if(answer[3] != answer[1]^answer[2]){
		//printf("Bcc de merda\n");
		return 1;
	}
	if(answer[0] != 0x7e || answer[4] != 0x7e){
		//printf("inicio ou fim de merda\n");
		return 1;
	}
	if((answer[2] == 0x05 && turn == 0) || (answer[2] == 0x85 && turn == 1)){
		//printf("Fixe\n");
		return 0;
	}
	else if((answer[2] == 0x01 && turn == 0) || (answer[2] == 0x81 && turn == 1)){
		printf("Reenvia\n");
		return 1;
	}
	else{
		//printf("turno de merda\n");
		return 1;
	}
}

int llwrite(int serial_fd, control_packet_t packet) {
	control_packet_t packetI = createTramaI(packet);
	turn = 1 - turn;

	//sendTrama(serial_fd, packetI);

	if(waitForAnswer(serial_fd) || first == 0){
		first = 1;
		//if(tries < 4){
			sendTrama(serial_fd, packetI);
			//tries++;
		//}
		/*if(tries == 4){
			tries = 0;
			return 1;
		}*/
	}
	tries = 0;

	return 0;
}
