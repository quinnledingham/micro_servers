#include <stdio.h>
#include <stdlib.h>

#include "../types.h"
#include "../qsock.h"

#include "buffer.h"

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "run client like this: ./ms_client <ip> <port>\n");
		return 1;
	}

	system("clear");

	char buffer[BUFFER_SIZE];
	struct Socket client = qsock_client(argv[1], argv[2], TCP);

	while(1) {
		int bytes = qsock_recv(client, buffer, BUFFER_SIZE, 0);
		if (bytes == 0) {
			printf("Zero bytes received\n");
			continue;
		}

		printf("%s", buffer);


	}

	qsock_free_socket(client);

	return 0;
}