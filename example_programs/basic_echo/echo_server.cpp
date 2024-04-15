#include <basic.h>

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "run like this: ./server <port>\n");
		return 1;
	}

	qsock_init_qsock();

	char buffer[100];
	struct QSock_Socket server = {};
	struct QSock_Socket client = {};
	if (!qsock_server(&server, argv[1], UDP)) {
		return 1;
	}
	//qsock_listen(server);
	//qsock_accept(&server, &client);
	
	while(1) {
		memset(buffer, 0, 100);
		int bytes = qsock_recv(server, &client, buffer, 100);
		if (bytes < 0) break;
		
		printf("%s %d\n", buffer, bytes);
		
		bytes = qsock_send(server, &client, buffer, 100);
		if (bytes < 0) break;
	}

	printf("Closing Server\n");

	qsock_free_socket(server);
	qsock_free_socket(client);

	return 0;
}
