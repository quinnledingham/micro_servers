#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <time.h>
#include <malloc.h>
#include <stdio.h>
#include <errno.h>

#include "types.h"
#include "qsock.h"

internal void
print_sockaddr(sockaddr_storage *address)
{
	printf("sockaddr %d\n", address[0]);
}

// Waits for a recieve on sock. if none it returns 0.
// https://www.it-swarm-fr.com/fr/c/udp-socket-set-timeout/1070229989/
internal int
timeout(int socket)
{
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	
	int received_packet = setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	if (received_packet < 0) fprintf(stderr, "timeout(): error\n");
	
	fprintf(stderr, "\ntimeout: %d\n", received_packet);
	
	return received_packet;
}

internal Address_Info
get_info(const char *ip, const char *port, int socket_type, struct addrinfo hints)
{
    printf("get_info(): ip: %s port: %s\n", ip, port);

	struct addrinfo *server_info;
	server_info = (struct addrinfo*)malloc(sizeof(struct addrinfo));

	switch(socket_type)
	{
		case TCP: hints.ai_socktype = SOCK_STREAM; break;
		case UDP: hints.ai_socktype = SOCK_DGRAM;  break;
	}

	getaddrinfo(ip, port, &hints, &server_info);

	struct Address_Info info = {};
	info.family = server_info->ai_family;
	info.socket_type = server_info->ai_socktype;
	info.protocol = server_info->ai_protocol;
	info.address_length = server_info->ai_addrlen;

	info.address = (const char *)malloc(info.address_length);
	memcpy((void*)info.address, server_info->ai_addr, info.address_length);

	free(server_info);

	printf("get_info(): successful\n");
	return info;
}

internal Address_Info
get_other_info(const char *ip, const char *port, int socket_type)
{
	struct addrinfo hints = {};
	hints.ai_family = AF_INET;

	return get_info(ip, port, socket_type, hints);
}

internal Address_Info
get_this_info(const char *port, int socket_type)
{
	struct addrinfo hints = {};
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;

	return get_info(NULL, port, socket_type, hints);
}

internal int
qsock_recv(Socket socket, const char *buffer, int buffer_size, int flags)
{
	int bytes = recv(socket.handle, (void*)buffer, buffer_size, flags);
	if (bytes == -1) perror("qsock_recv() error");
	return bytes;
}

internal int
qsock_recv_from(Socket *socket, const char *buffer, int buffer_size, int flags)
{
	if(socket->recv_info.address != 0) free((void*)socket->recv_info.address);
	socket->recv_info.address_length = sizeof(sockaddr);
	socket->recv_info.address = (const char *)malloc(socket->recv_info.address_length);
	socket->recv_info.family = socket->info.family;
	
	int bytes = recvfrom(socket->handle, (void*)buffer, buffer_size, flags, (sockaddr*)socket->recv_info.address, &socket->recv_info.address_length);
	if (bytes == -1) perror("qsock_recv_from() error");
	return bytes;
}

internal int
qsock_send(Socket socket, const char *buffer, int buffer_size, int flags)
{
	int bytes = send(socket.handle, (void*)buffer, buffer_size, flags);
	if (bytes == -1) perror("qsock_send() error");
	return bytes;
}

internal int
qsock_send_to(Socket socket, const char *buffer, int buffer_size, int flags, Address_Info info)
{
	int bytes = sendto(socket.handle, (void*)buffer, buffer_size, flags, (sockaddr*)info.address, info.address_length);
	if (bytes == -1) perror("qsock_send_to() error");
	return bytes;
}

internal int
qsock_socket(Address_Info info)
{
	int handle = socket(info.family, info.socket_type, info.protocol);
	if (handle < 0) fprintf(stderr, "qsock_socket(): socket() call failed\n");
	return handle;
}

internal void
qsock_connect(Socket socket)
{
	int error = connect(socket.handle, (sockaddr*)socket.info.address, socket.info.address_length);
	if (error == -1) fprintf(stderr, "qsock_connect(): connect() call failed\n");
}

internal void
qsock_bind(Socket socket)
{
	int error = bind(socket.handle, (sockaddr*)socket.info.address, socket.info.address_length);
	if (error == -1) perror("sock_bind() error");
	//print_sockaddr((sockaddr_storage*)address);
}

internal void
qsock_listen(Socket socket)
{
	int backlog = 5;
	int error = listen(socket.handle, backlog);
	if (error == -1) { perror("sock_listen() error"); }
}

internal void
qsock_accept(Socket *socket)
{
	if (!socket->passive) {
		fprintf(stderr, "qsock_accept(): not a passive(server) socket\n");
		return;
	}
	else if (socket->type != TCP) {
		fprintf(stderr, "qsock_accept(): not a TCP socket\n");
		return;
	}

	Socket new_socket = {};
	sockaddr address = {};
	unsigned int address_length;

	new_socket.handle = accept(socket->handle, &address, &address_length);
	if (new_socket.handle == -1) fprintf(stderr, "qsock_accept(): accept() call failed\n");

	new_socket.info.address = (const char *)malloc(address_length);
	memcpy((void*)new_socket.info.address, (void*)&address, address_length);
	
	memcpy((void*)socket->other, (void*)&new_socket, sizeof(Socket));
}

internal const char*
qsock_get_ip(Address_Info info)
{
	struct sockaddr_in *c = (struct sockaddr_in *)info.address;
	char *ip = (char *)malloc(80);
   	inet_ntop(info.family, &(c->sin_addr), ip, 80);
   	return ip;
}
