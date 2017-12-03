#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <strings.h>

#include "utils.h"

#define h_addr h_addr_list[0]

//adapted from getip.c
char* getIPbyHostname(char* hostname)
{
	struct hostent *h;

	h = (struct hostent*)gethostbyname(hostname);
    if (h == NULL)
		return NULL;

	return inet_ntoa(*((struct in_addr *)h->h_addr));
}

//adapted from clientTCP.c
int createSocket(int port, char* ip)
{
	int	sockfd;
	struct sockaddr_in server_addr;

	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket()");
		return -1;
    }

	int cnct = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if(cnct < 0)
	{
        perror("connect()");
		return -1;
	}
	return sockfd;
}
