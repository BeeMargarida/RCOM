#ifndef _UTILS_H
#define _UTILS_H

typedef struct url_t {
	char* url;
	char* username;
	char* password;
	char* host;
	char* dir;
	char* ip;
} url_t;

char* getIPbyHostname(char* hostname);
int createSocket(int port, char* ip);

#endif
