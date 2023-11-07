#include <stdio.h>
#include <stdlib.h>

#include "../../qsock.h"

#include "buffer.h"

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

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "run client like this: ./ms_client <ip> <port>\n");
		return 1;
	}

	system("clear");

	struct Socket client = qsock_client(argv[1], argv[2], TCP);

	while(1) {
		
		Message m = recv_message(&client);
		printf("%s", m.text);

		if (equal(m.text, "quit\n")) return 0;

		Message user_message = {};
		user_message.text = (char *)malloc(100);
		memset(user_message.text, 0, 100);
		get_user_input(m.prompt, user_message.text);

		if (m.key != 0) {
			// encrypt vote
			int vote = atoi(user_message.text);
			int encrypted = m.key * vote;
			memset(user_message.text, 0, 100);
			sprintf(user_message.text, "%d", encrypted);
			//printf("encrypted vote %d\n", encrypted);
		}

		send_message(client, user_message);

		free_message(m);
		free_message(user_message);
	}

	qsock_free_socket(client);

	return 0;
}