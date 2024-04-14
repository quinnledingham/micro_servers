#ifndef QSOCK_H
#define QSOCK_H

enum QSock_Family {
	INET,
	INET6
};

enum QSock_Socket_Type {
	STREAM,   // TCP
	DATAGRAMS // UDP
};

enum QSock_Protocol {
	TCP,
	UDP
};

#define TCP_BUFFER_SIZE 65536

struct QSock_Address_Info {
	enum QSock_Family      family;
	enum QSock_Socket_Type socket_type;
	enum QSock_Protocol    protocol;

	u32 address_length;
	const char *address; // sockaddr (socket address)
};

struct QSock_Socket {
	s32 handle; // linux: file descriptor
	enum QSock_Protocol protocol; // TCP or UDP

	struct QSock_Address_Info info; // client: info about address of server connected to, server: info about server address
	struct QSock_Address_Info recv_info; // most recent address that was received from (UDP)

	bool32 passive; // if true it waits for connection requests to come in (ie server)
	struct QSock_Socket *other; // filled in qsock_accept(). it is the sending sock for server using TCP
};

#ifdef OS_WINDOWS

#define OS_EXT(n) win32_##n

#elif OS_LINUX

#define OS_EXT(n) linux_##n

#endif // OS

#define QSOCK_FUNC(r, n, ...) r OS_EXT(n)(__VA_ARGS__); r (*qsock_##n)(__VA_ARGS__) = &OS_EXT(n)

QSOCK_FUNC(void, init_qsock, );
QSOCK_FUNC(bool8, init_socket, struct QSock_Socket *sock, const char *ip, const char *port);
QSOCK_FUNC(void, print_socket_error, );
QSOCK_FUNC(s32, timeout, struct QSock_Socket sock, s32 seconds, s32 microseconds);
QSOCK_FUNC(void, listen, struct QSock_Socket socket);
QSOCK_FUNC(void, accept, struct QSock_Socket *sock, struct QSock_Socket *client);

internal struct QSock_Address_Info
addrinfo_to_address_info(struct addrinfo og) {
    struct QSock_Address_Info info = {};
    
    info.family = og.ai_family;
    info.socket_type = og.ai_socktype;
    info.protocol = og.ai_protocol;
    info.address_length = (u32)og.ai_addrlen;

    info.address = (const char *)malloc(info.address_length + 1);
    memset((void*)info.address, 0, info.address_length + 1);
    memcpy((void*)info.address, og.ai_addr, info.address_length);

    return info;
}

internal bool8
qsock_client(QSock_Socket *sock, const char *ip, const char *port, enum QSock_Protocol protocol) {
	sock.protocol = protocol;
	socket.passive = false;
	if (qsock_init_socket(sock, ip, port)) {

		if (socket->protocol == UDP)
			qsock_set_timeout(*sock, 1, 0);

		printf("Client socket connected to ip: %s port: %s\n", ip, port);
		return true;
	} else {
		qsock_print_platform_error();
		return false;
	}
}

internal bool8
qsock_server(QSock_Socket *sock, const char *port, enum QSock_Protocol protocol) {
	struct Socket socket = {};
	socket.type = type;
	socket.passive = true;
	if (qsock_init_socket(&socket, NULL, port)) {

		printf("Server socket set up with port %s\n", port);
		return true;
	} else {
		qsock_print_platform_error();
		return false;
	}
}

internal void
qsock_free_socket(struct Socket socket) {
	close(socket.handle);
	free(socket.other);
}

/*
internal int
qsock_general_recv(struct Socket *socket, const char *buffer, int buffer_size)
{
	int bytes = 0;

	if(socket->recv_info.address != 0) free((void*)socket->recv_info.address);
	socket->recv_info.address_length = sizeof(struct sockaddr);
	socket->recv_info.address = (const char *)malloc(socket->recv_info.address_length);
	socket->recv_info.family = socket->info.family;

	// client
	if (!socket->passive) {
		bytes = qsock_recv_from(*socket, buffer, buffer_size, 0, &socket->recv_info);
		return bytes;
	}

	// server
	switch(socket->type)
	{
		case TCP: bytes = qsock_recv_from(*socket->other, buffer, buffer_size, 0, &socket->recv_info); break;
		case UDP: bytes = qsock_recv_from(*socket,        buffer, buffer_size, 0, &socket->recv_info); break;
	}

	return bytes;
}

internal int
qsock_general_send(struct Socket socket, const char *buffer, int buffer_size)
{
	int bytes = 0;

	// client
	if (!socket.passive) {
		bytes = qsock_send(socket, buffer, buffer_size, 0);
		//bytes = qsock_send_to(socket, buffer, buffer_size, 0, socket.info);
		return bytes;
	}

	// server
	switch(socket.type)
	{
		case TCP: bytes = qsock_send(*socket.other, buffer, buffer_size, 0); break;
		case UDP: bytes = qsock_send_to(socket, buffer, buffer_size, 0, socket.recv_info); break;
	}

	return bytes;
}
*/

#ifdef OS_WINDOWS

#include "win32_qsock.c";

#elif OS_LINUX

#include "qsock_linux.c";

#endif // OS

#endif // QSOCK_H
