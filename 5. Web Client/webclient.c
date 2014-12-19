#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("Creating socket failed: ");
		exit(1);
	}

	/* Need to take an input URL and parse it */
	return 0;
}