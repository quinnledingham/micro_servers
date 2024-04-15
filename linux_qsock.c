#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <time.h>
#include <malloc.h>
#include <stdio.h>
#include <errno.h>

// Waits for a recieve on sock.
// returns zero on success
// https://www.it-swarm-fr.com/fr/c/udp-socket-set-timeout/1070229989/
internal void
linux_set_timeout(struct Socket socket, int seconds, int microseconds) 
{
	struct timeval tv;
	tv.tv_sec = seconds;
	tv.tv_usec = microseconds;
	
	// set socket option
	int success = setsockopt(socket.handle, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	if (success < 0) fprintf(stderr, "timeout(): error\n");
}

internal int
linux_recv(struct Socket socket, const char *buffer, int buffer_size, int flags)
{
	int bytes = recv(socket.handle, (void*)buffer, buffer_size, flags);
	if (bytes == -1) perror("linux_recv() error");
	return bytes;
}

internal int
linux_recv_from(struct Socket socket, const char *buffer, int buffer_size, int flags, struct Address_Info *info)
{
	int bytes = recvfrom(socket.handle, (void*)buffer, buffer_size, flags, (struct sockaddr*)info->address, &info->address_length);
	if (bytes == -1) perror("linux_recv_from() error");
	return bytes;
}

internal int
linux_send(struct Socket socket, const char *buffer, int buffer_size, int flags)
{
	int bytes = send(socket.handle, (void*)buffer, buffer_size, flags);
	if (bytes == -1) perror("linux_send() error");
	return bytes;
}

internal int
linux_send_to(struct Socket socket, const char *buffer, int buffer_size, int flags, struct Address_Info info)
{
	int bytes = sendto(socket.handle, (void*)buffer, buffer_size, flags, (struct sockaddr*)info.address, info.address_length);
	if (bytes == -1) perror("linux_send_to() error");
	return bytes;
}

internal void
linux_listen(struct Socket socket)
{
	int backlog = 5;
	int error = listen(socket.handle, backlog);
	if (error == -1) { perror("sock_listen() error"); }
}

internal void
linux_accept(struct Socket *socket)
{
	if (!socket->passive) {
		fprintf(stderr, "linux_accept(): not a passive(server) socket\n");
		return;
	}
	else if (socket->type != TCP) {
		fprintf(stderr, "linux_accept(): not a TCP socket\n");
		return;
	}

	struct Socket new_socket = {};

	struct sockaddr address = {};
	unsigned int address_length;

	new_socket.handle = accept(socket->handle, &address, &address_length);
	if (new_socket.handle == -1) fprintf(stderr, "linux_accept(): accept() call failed\n");

	new_socket.info.address = (const char *)malloc(address_length);
	memcpy((void*)new_socket.info.address, (void*)&address, address_length);
	
	close(socket->other->handle);
	free((void*)socket->other->info.address);

	memcpy((void*)socket->other, (void*)&new_socket, sizeof(struct Socket));
}

internal const char*
linux_get_ip(struct Address_Info info)
{
	struct sockaddr_in *c = (struct sockaddr_in *)info.address;
	char *ip = (char *)malloc(80);
   	inet_ntop(info.family, &(c->sin_addr), ip, 80);
   	return ip;
}

internal const char*
linux_get_ip_from_web(const char *webname)
{
	printf("address: %s\n", webname);

	struct addrinfo hints, *info;
	memset(&hints, 0, sizeof(struct addrinfo));
	info = (struct addrinfo*)malloc(sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(webname, NULL, &hints, &info)) perror("linux_get_ip_from_web() getaddrinfo error\n");

    struct sockaddr_in *address = (struct sockaddr_in *)info->ai_addr;
    char *ip = (char *)malloc(200);
    memset(ip, 0, 200);
   	const char *dst = inet_ntop(info->ai_family, &(address->sin_addr), ip, 200);
   	if (dst == 0) perror("inet_ntop error");

   	printf("ip: %s\n", ip);
   	return ip;

	/*
    struct Address_Info address_info = {};
    address_info.family = info->ai_family;
    address_info.address = (void*)info->ai_addr;
    printf("YO: %p\n", address_info.address);
    return qsock_get_ip(address_info);
    */
}

internal struct Address_Info
linux_addrinfo_to_address_info(struct addrinfo og)
{
	struct Address_Info info = {};
	info.family = og.ai_family;
	info.socket_type = og.ai_socktype;
	info.protocol = og.ai_protocol;
	info.address_length = og.ai_addrlen;

	info.address = (const char *)malloc(info.address_length + 1);
	memset((void*)info.address, 0, info.address_length + 1);
	memcpy((void*)info.address, og.ai_addr, info.address_length);

	return info;
}

// find a address for the socket that works
// ip == NULL for passive to get wildcard address
internal void
linux_init_socket(struct Socket *sock, const char *ip, const char *port)
{
	struct addrinfo hints = {};
	hints.ai_family = AF_INET; //IPv4
	if (sock->passive) hints.ai_flags = AI_PASSIVE; // returns address for bind/accept
    //hints.ai_protocol = 0;

    switch(sock->type)
	{
		case TCP: hints.ai_socktype = SOCK_STREAM; break;
		case UDP: hints.ai_socktype = SOCK_DGRAM;  break;
	}

	struct addrinfo *info = (struct addrinfo*)malloc(sizeof(struct addrinfo));
	if (getaddrinfo(ip, port, &hints, &info)) fprintf(stderr, "getaddrinfo error");

	// find address that works looping through addrinfo linked list
	struct addrinfo *ptr;
	for (ptr = info; ptr != NULL; ptr = ptr->ai_next) {
		sock->handle = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		
		if (sock->handle == -1) {
			perror("linux_socket(): socket() call failed");
			continue;
		}

		if (sock->passive) {
			// bind socket to address if passive socket everytime
			if (bind(sock->handle, ptr->ai_addr, ptr->ai_addrlen) != -1) break;
			perror("get_address_info(): bind() call failed");
		} else {
			// connect socket to address if not passive socket only when using TCP
			if (sock->type == UDP || connect(sock->handle, ptr->ai_addr, ptr->ai_addrlen) != -1) break;
			perror("get_address_info(): connect() call failed");
		}
	}

	sock->info = linux_addrinfo_to_address_info(*ptr);
	sock->other = (struct Socket*)malloc(sizeof(struct Socket));

	freeaddrinfo(info);
}
