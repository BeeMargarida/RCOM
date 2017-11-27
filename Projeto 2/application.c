#include "application.h"
#include "ftp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

url_t parseUrl(char* rawUrl)
{
	url_t url;

	if (strncmp("ftp://", rawUrl, URL_START))
		usageError("ftp:// header must be declared");

	if (strchr(rawUrl, '@') != NULL)
	{
		url.username = calloc(50, sizeof(char));
		url.password = calloc(50, sizeof(char));
		int i = URL_START;
		int j = 0;
		while(rawUrl[i] != ':')
		{
			url.username[j] = rawUrl[i];
			i++;
			j++;
		}
		url.username[j] = '\0';
		i++;
		j = 0;
		while(rawUrl[i] != '@')
		{
			url.password[j] = rawUrl[i];
			i++;
			j++;
		}
		url.password[j] = '\0';
		i++;
		url.host = rawUrl + i;
	}
	else
	{
		url.username = NULL;
		url.password = NULL;
		url.host = rawUrl + URL_START;
	}

	url.ip = getIPbyHostname(url.host);
	if (url.ip == NULL)
		usageError("Specified host does not match any valid IP");
	printf("IP = %s\n", url.ip);

	return url;
}

void usageError(char* error)
{
	printf("\nError in processing url: %s\n", error);
	printf("Usage: download ftp://[<user>:<password>@]<host>/<url-path>\n\n");
	exit(1);
}

int download(url_t url)
{
	//ftp...
	return 0;
}
