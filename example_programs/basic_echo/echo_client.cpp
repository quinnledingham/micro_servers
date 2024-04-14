#include <basic.h>

internal void
get_user_input(const char* prompt, char *buffer)
{   
	char c;
	int bytes = 0;
    printf("%s", prompt);
    while ((c = getchar()) != '\n')
    {
        buffer[bytes] = c;
        bytes++;
    }
    
    buffer[bytes] = '\0';
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "run like this: ./client <ip> <port>\n");
		return 1;
	}

	qsock_init_qsock();

	struct QSock_Socket client = {};
	if (!qsock_client(&client, argv[1], argv[2], TCP)) {
		return 1;
	}
	char buffer[100];
	s32 bytes = 0;
	
	while(1) {
		memset(buffer, 0, 100);
		get_user_input("Enter a message:", buffer);
		bytes = qsock_send(client, buffer, 100);
		if (bytes < 0) break;
		
		memset(buffer, 0, 100);
		
		bytes = qsock_recv(client, buffer, 100);
		if (bytes < 0) break;
		printf("Recv: %s\n", buffer);
	};

	printf("Closing Client\n");

	return 0;
}
