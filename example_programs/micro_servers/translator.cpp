#include "basic.h"

#include "buffer.h"

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "run convertor like this: ./ms_translator <port>\n");
		return 1;
	} 

	qsock_init_qsock();

	char *port = argv[1];
	QSock_Socket server = {};
 	if (!qsock_server(&server, port, UDP))
		return 1;

	while(1) {
		QSock_Socket client = {};
		Message received = recv_message(server, &client);

		// Using the micro service again
		if (equal(received.system, "Connecting")) {
			printf("Start Message Sending\n");
			Message m = {};
			m.prompt = string_malloc("Enter a English word: ");
			m.text = string_malloc("Connected to translator service.\nWords that can be translated:\nbread, yellow, hello, bye, chair\nType quit to leave\n");
			m.system = string_malloc("clear");
			send_message(server, &client, m);
			free_message(m);
			//continue;
		}
		else {
			printf("\n%s\n", received.text);

			Message m = {};
			const char *f = "French translation: ";
			if      (equal(received.text, "bread"))  m.text = string_malloc_concat(f, "pain\n");
			else if (equal(received.text, "yellow")) m.text = string_malloc_concat(f, "jaune\n");
			else if (equal(received.text, "hello"))  m.text = string_malloc_concat(f, "bonjour\n");
			else if (equal(received.text, "bye"))    m.text = string_malloc_concat(f, "au revoir\n");
			else if (equal(received.text, "chair"))  m.text = string_malloc_concat(f, "chaise\n");

			m.prompt = string_malloc("Enter an English word: ");
			send_message(server, &client, m);

			free_message(m);
		}

		free_message(received);
	}

	qsock_free_socket(server);
	return 0;
}