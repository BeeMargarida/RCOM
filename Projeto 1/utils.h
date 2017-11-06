#ifndef CTRL_PACK
#define CTRL_PACK

#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define clearScreen() printf("\033[H\033[J")

typedef struct {
	unsigned char *params;
	int size;
} control_packet_t;

typedef enum {
	SENDER, RECEIVER
} Types_t;

typedef struct {
	int packets;
	int rr;
	int rej;
} statistics_t;

#endif
