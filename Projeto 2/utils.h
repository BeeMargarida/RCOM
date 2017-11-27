#ifndef _UTILS_H
#define _UTILS_H

typedef struct url_t {
	char* url;
	char* username;
	char* password;
	char* host;
	char* ip;
} url_t;

char* getIPbyHostname(char* hostname);

#endif
