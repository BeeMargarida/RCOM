#include "application.h"

int main(int argc, char** argv)
{
	if (argc != 2)
		usageError("Invalid number of arguments");

	url_t url = parseUrl(argv[1]);
	return download(url);
}
