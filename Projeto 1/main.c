#include "application_layer.h"

#define DEFAULT_SERIAL_PORT 0

void printUsage()
{
	printf("Usage: rcom [ [-s filename] | [-r] ] [-ser 0|1|2|3]\n");
}

int main(int argc, char** argv)
{
	if (argc <= 1)
	{
		printUsage();
		return 1;
	}

	int serial_no = DEFAULT_SERIAL_PORT;
	int arg = DEFAULT_SERIAL_PORT;

	if (argc == 5 && strcmp(argv[3], "-ser") == 0)
		arg = atoi(argv[4]);
	else if (argc == 4 && strcmp(argv[2], "-ser") == 0)
		arg = atoi(argv[3]);

	if (arg < 0 || arg > 3)
		printf("Serial port must be 0, 1, 2, or 3. Specified value was %d. Assuming default serial port %d\n", arg, DEFAULT_SERIAL_PORT);
	else
		serial_no = arg;

	if (strcmp(argv[1], "-r") == 0 && argc >= 2)
 	{
		return startReceiver(serial_no);
	}
	else if (strcmp(argv[1], "-s") == 0)
	{
		if (argc < 3)
		{
			printf("Error: missing filename to send\n");
			printUsage();
			return 1;
		}
		return startSender(argv[2], serial_no);
	}
	else
	{
		printf("Error: invalid arguments\n");
		printUsage();
		return 1;
	}
}
