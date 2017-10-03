#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "llopen.h"

void printUsage();
int startReceiver();
int startSender(char* filename);

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

void printUsage()
{
	printf("Usage: rcom [-s filename] | [-r]\n");
}

int startReceiver()
{
	llopen(0, 1);
	//llread();
	//llclose();
	return 0;
}

int startSender(char* filename)
{
	int fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		printf("Error: file %s does not exist\n", filename);
		return 1;
	}
	llopen(0, 0);
	//llwrite();
	//llclose();
	close(fd);
	return 0;
}








