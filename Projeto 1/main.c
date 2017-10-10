#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "llopen.h"

#define DATA_BLOCK 0x01
#define DATA_START 0x02
#define DATA_END 0x03

#define FILESIZE 0x00
#define FILENAME 0x01
#define MODE 0111111000

void printUsage();
int startReceiver();
int startSender(char* filename);

void writeDataStart(char* buf);
void writeDataBlock(char* buf);

int fd;
int filesize = 0;
char* filename;

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
	llopen(0, RECEIVER);
	int reading = 1;
	char buf[512];
	int first = 0;
	while (reading)
	{
		int read = -1;/*llread(buf);*/
		if (read < 0)
		{
			printf("Error reading from llread\n");
			return 1;
		}
		if (read == 0)
			continue;

		if (buf[0] == DATA_START && first == 0)
		{
			first = 1;
			writeDataStart(buf);

		}
		else if (buf[0] == DATA_END)
		{
			reading = 0;
		}
		else
			writeDataBlock(buf);
	}
	return 0;
}

void writeDataStart(char* buf)
{
	int i;
	if (buf[1] == FILESIZE)
	{
		int sizelength = buf[2];
		for (i = 3; i < sizelength; i++)
		{
			filesize < 8;
			filesize |= buf[i];
		}
		printf("File size: %d bytes\n", filesize);

	}
	if (buf[i] == FILENAME)
	{
		i++;
		int namelength = buf[i];
		filename = malloc(sizeof(char) * namelength);
		int j;
		for (i += 1, j = 0; i < namelength; i++, j++)
			filename[j] = buf[i];
		printf("File name: %s\n", filename);
	}

	fd = open(filename, O_WRONLY | O_CREAT, MODE);
	if (fd < 0)
	{
		printf("Error opening/creating file %s\n", filename);
	}
}

void writeDataBlock(char* buf)
{
	int seqN = buf[1];
	int n = 256 * buf[2] + buf[3];
	int i;
	for (i = 0; i < n; i++)
		write(fd, buf[i], 1);
}

int startSender(char* filename)
{
	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		printf("Error: file %s does not exist\n", filename);
		return 1;
	}
	llopen(0, SENDER);
	//llwrite();
	//llclose();
	close(fd);
	return 0;
}








