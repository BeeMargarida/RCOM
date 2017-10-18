#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "data_link.h"

void printUsage()
{
	printf("Usage: rcom [-s filename] | [-r]\n");
}

int main(int argc, char** argv)
{
	if (argc <= 1)
	{
		printUsage();
		return 1;
	}

	if (strcmp(argv[1], "-r") == 0 && argc == 2)
 	{
		return startReceiver();
	}
	else if (strcmp(argv[1], "-s") == 0)
	{
		if (argc != 3)
		{
			printf("Error: missing filename to send\n");
			printUsage();
			return 1;
		}
		return startSender(argv[2]);
	}
	else
	{
		printf("Error: invalid arguments\n");
		printUsage();
		return 1;
	}
}
