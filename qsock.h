#ifndef QSOCK_H
#define QSOCK_H

#include <unistd.h>

enum Socket_Types
{
	TCP,
	UDP
};

enum
{
	Client_Socket,
	Server_Socket,
};

#define TCP_BUFFER_SIZE 65536

struct Address_Info
{
	int family;
	int socket_type;
	int protocol;

	u32 address_length;
	const char *address; // sockaddr (socket address)
};

struct Socket
{
	int handle; // linux: file descriptor
	int type; // TCP or UDP
	Address_Info info; // info about server connected to on client

	b32 passive; // if true it waits for connection requests to come in (ie server)
	Socket *other;
};

#include "qsock_linux.cpp"

internal Socket
qsock_client(const char *ip, const char *port, int type)
{
	Socket socket = {};
	socket.type = type;
	socket.passive = false;
	socket.info = get_other_info(ip, port, socket.type);
	socket.handle = qsock_socket(socket.info);
	if (socket.type == TCP) qsock_connect(socket);
	socket.other = (Socket*)malloc(sizeof(Socket));
	printf("Client socket connected to ip: %s port: %s\n", ip, port);
	return socket;
}

internal Socket
qsock_server(const char *port, int type)
{
	Socket socket = {};
	socket.type = type;
	socket.passive = true;
	socket.info = get_this_info(port, socket.type);
	socket.handle = qsock_socket(socket.info);
	qsock_bind(socket);
	if (socket.type == TCP) qsock_listen(socket);
	socket.other = (Socket*)malloc(sizeof(Socket));
	printf("Server socket set up with port %s\n", port);
	return socket;
}

internal void
qsock_free_socket(Socket socket)
{
	close(socket.handle);
	free(socket.other);
}

internal int
qsock_server_recv(Socket server, const char *buffer, int buffer_size)
{
	if (!server.passive) {
		fprintf(stderr, "qsock_server_recv(): not a passive(server) socket\n");
		return 0;
	}

	int bytes = 0;

	switch(server.type)
	{
		case TCP: bytes = qsock_recv_from(*server.other, buffer, buffer_size, 0, server.info); break;
		case UDP: bytes = qsock_recv_from(server,        buffer, buffer_size, 0, server.info); break;
	}

	return bytes;
}

#endif // QSOCK_H