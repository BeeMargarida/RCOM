#include "llwrite.h"

int turn = 0;
int tries = 0;
int numPacket = 0;
int first = 0;

int f=1;

void receive_alarm1() {
    printf("alarme #\n");
    f=1;
}

int create_alarm1() {
    (void) signal(SIGALRM, receive_alarm1);  // instala  rotina que atende interrupcao
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
			*size += 1;
			j+=2;
		}
		else if(buf[i] == 0x7D){
			trama[j] = 0x7D;
			trama[j+1] = 0x5D;
			*size += 1;
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

	int stuffedSize = packet.size + 1;
	unsigned char * notStuffedPacket = malloc((1 + packet.size) * sizeof(unsigned char));
	
	memcpy(notStuffedPacket, packet.params, packet.size*sizeof(unsigned char));
	notStuffedPacket[packet.size] = BCC2;

	unsigned char * stuffedPacket = stuffingData(notStuffedPacket, &stuffedSize);

	unsigned char* trama = malloc((5 + stuffedSize) * sizeof(unsigned char));
	trama[0] = F; trama[1] = A; trama[2] = C1; trama[3] = BCC1;
	memcpy(trama + 4, stuffedPacket, stuffedSize*sizeof(unsigned char));
	trama[4 + stuffedSize] = F;

	control_packet_t packetI;
	packetI.params = trama;
	packetI.size = stuffedSize + 5;
	return packetI;
}

//isto funciona bem
void sendTrama(int serial_fd, control_packet_t packet){
	int i = 0;
	int counter = 0;
	int not7E = TRUE;
	while(not7E)
	{
		int read = write(serial_fd, packet.params + i, 1);
		if (read == 1)
		{
			if (packet.params[i] == 0x7E && i > 0)
			{
				not7E = FALSE;
			}
			i++;
			counter++;
		}
	}
	printf("\n");
	printf("numPacket: %d\n", numPacket);
	printf("Wrote %d bytes\n", counter);
}

int waitForAnswer(int serial_fd){
	int reading = TRUE;
	int nread;

	unsigned char answer[5] = {};
	int i = 0;

	nread = read(serial_fd, answer, 5);
	if (nread < 0)
	{
		printf("Error reading answer");
		return 1;
	}
	if(answer[0] == 0x7E){
		if(answer[3] != (answer[1]^answer[2])){
			printf("BCC Error\n");
			return 1;
		}
		else if((answer[2] == 0x01 && turn == 0) || (answer[2] == 0x81 && turn == 1)){
			printf("REJ Received\n");
			return 1;
		}
		else if((answer[2] == 0x05 && turn == 0) || (answer[2] == 0x85 && turn == 1)){
			printf("RR Received\n");
			numPacket++;
			return 0;
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}
}

int llwrite(int serial_fd, control_packet_t packet) {
	control_packet_t packetI = createTramaI(packet);
	create_alarm1();
	turn = 1 - turn;
	int cycle = TRUE;
	while(cycle){
		sendTrama(serial_fd, packetI);
		f = 0;
		alarm(1);
		int answer = -1;
		while(f == 0 && answer == -1){
			answer = waitForAnswer(serial_fd);
			if(answer == 0){
				cycle = FALSE;
				alarm(0);
				break;
			}
			else if (answer == 1){
				alarm(0);
				break;
			}
		}
	}
	return 0;
}
