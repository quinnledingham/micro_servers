#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../types.h"
#include "../qsock.h"

#include "buffer.h"

internal void
echo(Socket *server)
{

}

internal void
handle_client(const char *micro_services_ip, Socket *server)
{
	printf("Client Connected\n");

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	memcpy(buffer, "Command: ", 9);
	qsock_send(*server, buffer, BUFFER_SIZE, 0);

	while(1) {
		memset(buffer, 0, BUFFER_SIZE);
		int bytes = qsock_server_recv(*server, buffer, BUFFER_SIZE);
		int request = atoi(buffer);
		switch(request) {
			case 1: break;
			case 2: break;
			case 3: break;
			case 4: break;
			case 9: echo(server); break;
			case 0: break;
			default: break;
		}
	}
}

// default menu
const char* mainmenu =
	"Select an action:\n"
	"1. Translate\n"
	"2. Currency Exchange\n"
	"3. Vote\n"
	"4. View Voting Result\n"
	"9. Echo\n"
	"0. Close Connection\n";

int main(int argc, char *argv[])
{
	if (argc != 3) {
        printf("Usage: ./indirectionServer <micro services ip> <Port Number>\n");
        return 1;
    }

    Socket server = qsock_server(argv[2], TCP);

    pid_t pid;
    while(1) {
    	qsock_accept(&server);
    	pid = fork();
    	if (pid == 0) handle_client(argv[1], &server);
    }

    qsock_free_socket(server);

	return 0;
}