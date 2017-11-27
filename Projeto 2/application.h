#ifndef _APP_H
#define _APP_H

#include "utils.h"

#define URL_START 6

url_t parseUrl(char* rawUrl);
int download(url_t url);
void usageError(char* error);

#endif
