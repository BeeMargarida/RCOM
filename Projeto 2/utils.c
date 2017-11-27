#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include<arpa/inet.h>

#include "utils.h"

#define h_addr h_addr_list[0]

char* getIPbyHostname(char* hostname)
{
	struct hostent *h;

	h = (struct hostent*)gethostbyname(hostname);
    if (h == NULL)
		return NULL;

	return inet_ntoa(*((struct in_addr *)h->h_addr));
}
