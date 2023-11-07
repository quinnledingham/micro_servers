#include "../../types.h"
#include "../../qsock.h"

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "run like this: ./server <port>\n");
		return 1;
	}

	Socket server = qsock_server(argv[1], UDP);
	char buffer[TCP_BUFFER_SIZE];
	qsock_accept(&server);

	while(1) {
		memset(buffer, 0, 100);
		int bytes = qsock_general_recv(&server, buffer, TCP_BUFFER_SIZE);
		printf("%s %d\n", buffer, bytes);
		qsock_general_send(server, buffer, 100);
	}

	return 0;
}