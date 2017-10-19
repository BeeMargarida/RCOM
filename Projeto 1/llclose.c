
int llclose(int fd, int SorR){
	int dc = 0;
	inc conta = 0;

	char *discS[5] = {};
	discS[0] = 0x7e;
	discS[1] = 0x03;
	discS[2] = 0x0b;
	discS[3] = disc[1] ^ disc[2];
	discS[4] = 0x7e;

	char *discR[5] = {};
	discR[0] = 0x7e;
	discR[1] = 0x01;
	discR[2] = 0x0b;
	discR[3] = disc[1] ^ disc[2];
	discR[4] = 0x7e;
	switch(SorR){
		case 0: //sender
			while(!dc){
				write(fd, discS, 5);
				//wait for answer
			}
			break;
		case 1: //receiver
			while(!dc){
				int n = 
			}
	}


	return close(fd);
}
