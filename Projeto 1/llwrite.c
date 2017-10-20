#include "llwrite.h"

int turn = 0;
int tries = 0;
int numPacket = 0;

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

	return packetI;
}

void sendTrama(int serial_fd, control_packet_t packet){
	int i = 0;
	printf("Num PACKET: %d", numPacket);
	for(i; i < packet.size; i++){
		printf("%x ", packet.params[i]);
	}
	int wrote = write(serial_fd, packet.params, packet.size);
	numPacket++;
	printf("Wrote %d bytes\n", wrote);
}

int waitForAnswer(int serial_fd){
	int reading = TRUE;
	int nread;
	
	unsigned char rr[5] = {0x7e, 0x03, 0x01 , 0x01, 0x7e};
	unsigned char rej[5] = {0x7e, 0x03, 0x01, 0x01, 0x7e};
	rr[2] = (turn == 0 ? 0x05 : 0x85);
	rej[2] = (turn == 0 ? 0x01 : 0x81);

	unsigned char answer[5] = {};
	int i = 0;
	while (reading)
	{
		nread = read(serial_fd, answer + i, 1);
		if (nread < 0)
		{
			printf("Error reading answer");
			return 1;
		}
		if(i != 0 && answer[i] == 0x7E){
			printf("ANSWER: \n");
			int j = 0;
			for(j; j < nread; j++){
				printf("%x : ", answer[j]);
			}
			printf("\n");
			reading = FALSE;
			if((answer[1] ^ answer[2]) != answer[3]){
				printf("O bcc tem erro\n");
				return 1;
			}
			if(answer[2] == 0x05 && turn == 0){
				printf("All went well\n");
				return 0;
			}
			else if(answer[2] == 0x85 && turn == 1){
				printf("All went well\n");
				return 0;
			}
			else if(answer[2] == 0x01 && turn == 0){
				printf("Packet must be resent\n");
				return 1;
			}
			else if(answer[2] == 0x81 && turn == 1){
				printf("Packet must be resent\n");
				return 1;
			}
			else {
				//printf("Wrong turn number\n");
				return 0; // ?
			}
		}
		else {
			return 1;
		}
		i += nread;
	}
	//return 1 ou 0 consoante o "turno"
	return 1;
}
/*
int verifyAnswer(int answer){
	if(answer != turn){
		return 1;
	}
	else{
		return 0;
	}
}*/

int llwrite(int serial_fd, control_packet_t packet) {
	control_packet_t packetI = createTramaI(packet);
	turn = 1 - turn;

	sendTrama(serial_fd, packetI);
	//int i = waitForAnswer(serial_fd);
	if(waitForAnswer(serial_fd)/*verifyAnswer(i)*/){
		if(tries < 4){
			sendTrama(serial_fd, packetI);
			tries++;
		}
		if(tries == 4){
			return 1;
		}
	}
	tries = 0;

	return 0;
}
