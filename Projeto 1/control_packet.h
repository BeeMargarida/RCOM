#ifndef CTRL_PACK
#define CTRL_PACK

typedef struct {
	unsigned char *params;
	int size;
} control_packet_t;

typedef enum {
	SENDER, RECEIVER
} Types_t;

#endif
