#include <unistd.h>
#include <string.h>
#include <stdbool.h> // C
#include <stdint.h> // C
//#include <cstdint> // C++

#include "defines.h"

#include "../../qsock.h"

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
		fprintf(stderr, "run like this: ./client <ip> <port>\n");
		return 1;
	}

	struct Socket client = qsock_client(argv[1], argv[2], UDP);
	char buffer[100];
	
	while(1) {
		memset(buffer, 0, 100);
		get_user_input("Enter a message:", buffer);
		qsock_general_send(client, buffer, 100);
		memset(buffer, 0, 100);
		qsock_general_recv(&client, buffer, 100);
		printf("Recv: %s\n", buffer);
	};

	return 0;
}
