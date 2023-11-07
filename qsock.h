#ifndef QSOCK_H
#define QSOCK_H

#include <unistd.h>

enum Socket_Types
{
	TCP,
	UDP
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
	Address_Info info; // client: info about address of server connected to, server: info about server address

	Address_Info recv_info; // most recent address that was received from (UDP)

	b32 passive; // if true it waits for connection requests to come in (ie server)
	Socket *other; // filled in qsock_accept()
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
qsock_general_recv(Socket *socket, const char *buffer, int buffer_size)
{
	int bytes = 0;

	// client
	if (!socket->passive) {
		if (socket->type == UDP) timeout(socket->handle);
		bytes = qsock_recv(*socket, buffer, buffer_size, 0);
		return bytes;
	}

	// server
	switch(socket->type)
	{
		case TCP: bytes = qsock_recv(*socket->other, buffer, buffer_size, 0); break;
		case UDP: bytes = qsock_recv_from(socket, buffer, buffer_size, 0); break;
	}

	return bytes;
}

internal int
qsock_general_send(Socket socket, const char *buffer, int buffer_size)
{
	int bytes = 0;

	// client
	if (!socket.passive) {
		bytes = qsock_send_to(socket, buffer, buffer_size, 0, socket.info);
		return bytes;
	}

	switch(socket.type)
	{
		case TCP: bytes = qsock_send(*socket.other, buffer, buffer_size, 0); break;
		case UDP: bytes = qsock_send_to(socket, buffer, buffer_size, 0, socket.recv_info); break;
	}

	return bytes;
}

#endif // QSOCK_H