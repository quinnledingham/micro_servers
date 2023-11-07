#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../types.h"
#include "../../qsock.h"

#include "buffer.h"

// default menu
const char* mainmenu =
	"Select an action:\n"
	"1. Translate\n"
	"2. Currency Exchange\n"
	"3. Vote\n"
	"4. View Voting Result\n"
	"9. Echo\n"
	"0. Close Connection\n";

internal void
echo(Socket *server)
{
	printf("Starting echo service\n");

	Message m = {};
	m.prompt = string_malloc("Enter a message: ");
	m.text = string_malloc("Echo:\nWill send back whatever you send.\nType quit to leave.\n");
	send_message(*server, m);
	free_message(m);

	while(1) {
		Message received = recv_message(server);
		printf("%s\n", received.text);
		
		if (equal(received.text, "quit")) {
			break;
		}	

		Message m = {};
		m.prompt = string_malloc("Enter a message: ");
		m.text = string_malloc_concat(received.text, "\n");
		send_message(*server, m);

		free_message(received);
		free_message(m);
	}
}

internal b32
micro_server_pass_thru(Socket *server, const char *ip, const char *port, const char *system_message)
{
	b32 show_menu = true;
	struct Socket client = qsock_client(ip, port, UDP);

	{
		Message m = {};
		m.system = string_malloc(system_message);
		send_message(client, m);
		free_message(m);
		printf("Send connecting to port %s\n", port);
	}

	while(1) {
		Message t_received = recv_message(&client);

		if (t_received.text_length == 0) {
			// timed out
			show_menu = false;
			break;
		}

		printf("\n%s\n", t_received.text);

		send_message(*server, t_received);

		Message c_received = recv_message(server);
		if (equal(c_received.text, "quit")) {
			break;
		}

		send_message(client, c_received);
	}

	qsock_free_socket(client);
	return show_menu;
}

internal b32
micro_server_single(Socket *server, const char *ip, const char *port, const char *system_message)
{
	struct Socket client = qsock_client(ip, port, UDP);

	Message m = {};
	m.system = string_malloc(system_message);
	send_message(client, m);
	free_message(m);

	Message t_received = recv_message(&client);

	if (t_received.text_length == 0) {
		// timed out
		return false;
	}

	printf("\n%s\n", t_received.text);

	send_message(*server, t_received);

	qsock_free_socket(client);

	return true;
}

internal void
handle_client(const char *micro_services_ip, Socket *server)
{
	printf("Client Connected\n");

	b32 show_menu = true;
	while(1) {		
		if (show_menu) {
			Message m = {};
			m.prompt = string_malloc("Command: ");
			m.text = string_malloc(mainmenu);
			send_message(*server, m);
			free_message(m);
			show_menu = false;
		}

		Message received = recv_message(server);

		int request = atoi(received.text);
		switch(request) {
			case 1: {
				// Translator
				show_menu = micro_server_pass_thru(server, micro_services_ip, "44690", "Connecting");
				if (!show_menu) {
					Message m  = {};
					m.text = string_malloc("Translator service is down.\n");
					m.prompt = string_malloc("Command: ");
					send_message(*server, m);
					free_message(m);
				}
			} break;
			case 2: {
				// Convertor
				show_menu = micro_server_pass_thru(server, micro_services_ip, "44691", "Connecting");
				if (!show_menu) {
					Message m  = {};
					m.text = string_malloc("Convertor service is down.\n");
					m.prompt = string_malloc("Command: ");
					send_message(*server, m);
					free_message(m);
				}
			}break;
			case 3: {
				// Voting
				show_menu = micro_server_pass_thru(server, micro_services_ip, "44692", "Connecting");
				if (!show_menu) {
					Message m  = {};
					m.text = string_malloc("Voting service is down.\n");
					m.prompt = string_malloc("Command: ");
					send_message(*server, m);
					free_message(m);
				}
			} break;
			case 4: {
				// Voting Summary
				if (!micro_server_single(server, micro_services_ip, "44692", "Summary")) {
					Message m  = {};
					m.text = string_malloc("Voting service is down.\n");
					m.prompt = string_malloc("Command: ");
					send_message(*server, m);
					free_message(m);
				}
			} break;
			case 9: echo(server); show_menu = true; break;
			case 0: {
				Message m = {};
				m.text = string_malloc("quit\n");
				send_message(*server, m);
				free_message(m);
				printf("Client Disconnected\n");
				return;
			} break;
			default: {
				Message m = {};
				m.prompt = string_malloc("Command: ");
				m.text = string_malloc("Not a service\n");
				send_message(*server, m);
				free_message(m);
			} break;
		}


		free_message(received);
	}
}

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