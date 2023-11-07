#ifndef QSOCK_H
#define QSOCK_H

#ifndef TYPES_H
#define TYPES_H

#include <unistd.h>
#include <stdbool.h> // C
#include <stdint.h> // C
//#include <cstdint> // C++
#include <string.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef s8 b8;
typedef s32 b32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float r32;
typedef double r64;
typedef r32 f32;
typedef r64 f64;

#define function      static
#define internal      static
#define local_persist static
#define global        static

#define ARRAY_COUNT(n)     (sizeof(n) / sizeof(n[0]))
#define ARRAY_MALLOC(t, n) ((t*)platform_malloc(n * sizeof(t)))

#endif //TYPES_H

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
	struct Address_Info info; // client: info about address of server connected to, server: info about server address

	struct Address_Info recv_info; // most recent address that was received from (UDP)

	b32 passive; // if true it waits for connection requests to come in (ie server)
	struct Socket *other; // filled in qsock_accept()
};

#include "qsock_linux.c"

internal struct Socket
qsock_client(const char *ip, const char *port, int type)
{
	struct Socket socket = {};
	socket.type = type;
	socket.passive = false;
	socket.info = get_other_info(ip, port, socket.type);
	socket.handle = qsock_socket(socket.info);
	if (socket.type == TCP) qsock_connect(socket);
	socket.other = (struct Socket*)malloc(sizeof(struct Socket));
	printf("Client socket connected to ip: %s port: %s\n", ip, port);
	return socket;
}

internal struct Socket
qsock_server(const char *port, int type)
{
	struct Socket socket = {};
	socket.type = type;
	socket.passive = true;
	socket.info = get_this_info(port, socket.type);
	socket.handle = qsock_socket(socket.info);
	qsock_bind(socket);
	if (socket.type == TCP) qsock_listen(socket);
	socket.other = (struct Socket*)malloc(sizeof(struct Socket));
	printf("Server socket set up with port %s\n", port);
	return socket;
}

internal void
qsock_free_socket(struct Socket socket)
{
	close(socket.handle);
	free(socket.other);
}

internal int
qsock_general_recv(struct Socket *socket, const char *buffer, int buffer_size)
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
qsock_general_send(struct Socket socket, const char *buffer, int buffer_size)
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