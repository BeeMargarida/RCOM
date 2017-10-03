#include "llopen.h"

volatile int STOP=FALSE;
int flag=1, conta=1;

int receive_alarm() {
    printf("alarme # %d\n", conta);
    flag=1;
    conta++;
}

int create_alarm() {
    (void) signal(SIGALRM, receive_alarm);  // instala  rotina que atende interrupcao

    while(conta < 4){
       if(flag){
          alarm(3);                 // activa alarme de 3s
          flag=0;
       }
    }
    printf("Vou terminar.\n");
}

int llopen(int fd, int type) {
    if(type == 0){    
        char set[5] = {0x7E, 0x03, 0x03, 0x00, 0x7E};
        while(conta < 4){
           
            write(fd, set, 5);

            create_alarm();

            char res[5];
            int x;
            char input;
            int currentIndex = 0;
            while (STOP==FALSE) 
            {       /* loop for input */
                if(flag == 1){
                    break;
                }
                x = read(fd, &input, 1);
                if (x == 0)
                    continue;
                res[currentIndex] = input;
                if (res[currentIndex] == 0x7E)
                    break;
                currentIndex++;
            }
            printf("%s\n", res);
        }
        return 0;
    }
    else if(type == 1){
        char res[5];
        char input;
        int x;
        int currentIndex = 0;
        while (STOP==FALSE) {       /* loop for input */
            if(flag == 1){
                break;
            }
            x = read(fd, &input, 1);
            if (x == 0)
                continue;
            res[currentIndex] = input;
            if (res[currentIndex] == 0x7E)
                break;
            currentIndex++;
        }
        if(res[3] == (res[1] ^ res[2]) && res[2] == 0x03){
            char ua[5] = {0x7E, 0x03, 0x07, 0x04, 0x7E};
            write(fd, ua, 5);
            return 0;
        }
        else  {
            printf("There was a problem....ups\n");
            return -1;
        }
    } else {
        return -1;
    }
}