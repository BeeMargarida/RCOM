#include "data_link_layer.h"

volatile int STOP=FALSE;
int flag=1, conta=1;

void receive_alarm() {
    printf("alarme # %d\n", conta);
    flag=1;
    conta++;
    if (conta >= 4)
      exit(-1);
}

int create_alarm() {
    (void)signal(SIGALRM, receive_alarm);  // instala  rotina que atende interrupcao
    return SIGALRM;
}

int llopen(int serial, Types_t type) {

    int fd;
    struct termios oldtio,newtio;

    switch (serial)
    {
        case 0:
            fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY);
            break;
        case 1:
            fd = open("/dev/ttyS1", O_RDWR | O_NOCTTY);
            break;
        case 2:
            fd = open("/dev/ttyS2", O_RDWR | O_NOCTTY);
            break;
        case 3:
            fd = open("/dev/ttyS3", O_RDWR | O_NOCTTY);
            break;
        default:
            printf("Invalid serial port %d specified in llopen", serial);
            return -1;
    }


    if (fd < 0)
    {
        perror("Could not open serial port\n");
        exit(-1);
    }

    tcgetattr(fd,&oldtio); /* save current port settings */

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);

    int first = 0;

    if(type == SENDER)
    {
        unsigned char set[5] = {0x7E, 0x03, 0x03, 0x00, 0x7E};
        create_alarm();
        while(conta < 4 && flag == 1 )
        {
            write(fd, set, 5);

            flag = 0;
            alarm(3);

            unsigned char res[5];
            int x;
            unsigned char input;
            int currentIndex = 0;
            while (STOP==FALSE && flag == 0 )
            {       /* loop for input */
                x = read(fd, &input, 1);
                if (x == 0)
                    continue;
                res[currentIndex] = input;
                if (res[currentIndex] == 0x7E){
                    if(first == 0)
                        first = 1;
                    else{
                        first = 0;
                        alarm(0);
                        STOP = TRUE;
                    }
                }
                currentIndex++;
                if(currentIndex == 5) break;
            }
        }
        if (TRUE)
        {
          printf("llopen exited successfully\n");
          return fd;
        }
        else
        {
          printf("Error in llopen: invalid tram\n");
          return -1;
        }
    }

    else if(type == RECEIVER)
    {
        char res[5];
        char input;
        int x;
        int currentIndex = 0;
        while (STOP==FALSE) {       /* loop for input */
            x = read(fd, &input, 1);
            if (x == 0)
                continue;
            res[currentIndex] = input;
            if (res[currentIndex] == 0x7E){
                if(first == 0)
                    first = 1;
                else{
                    first = 0;
                    break;
                }
            }
            currentIndex++;
            if (currentIndex == 5) break;
        }

        if(res[3] == (res[1] ^ res[2]) && res[2] == 0x03)
        {
            char ua[5] = {0x7E, 0x03, 0x07, 0x04, 0x7E};
            write(fd, ua, 5);
            printf("llopen exited successfully\n");
            return fd;
        }
        else
        {
          printf("Error in llopen: invalid tram\n");
          return -1;
        }
    }
    return -1;
}
