internal struct QSock_Address_Info
win32_addrinfo_to_address_info(struct addrinfo og) {
    struct QSock_Address_Info info = {};
    
    info.family = (QSock_Family)og.ai_family;
    info.socket_type = (QSock_Socket_Type)og.ai_socktype;
    info.protocol = (QSock_Protocol)og.ai_protocol;
    info.address_length = (u32)og.ai_addrlen;

    info.address = (const char *)malloc(info.address_length + 1);
    memset((void*)info.address, 0, info.address_length + 1);
    memcpy((void*)info.address, og.ai_addr, info.address_length);

    return info;
}

void win32_init_qsock() {
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != 0) {
        print("WSAStartup failed with error: %d\n", result);
    }
    
}

internal void
win32_print_platform_error() {   
    print("WSAerror %d\n", WSAGetLastError());
}

internal bool8
win32_init_socket(QSock_Socket *sock, const char *ip, const char *port) {
    addrinfo hints = {};
    hints.ai_family = AF_INET; // IPv4
    if (sock->passive)
        hints.ai_flags = AI_PASSIVE;

    switch(sock->protocol) {
        case TCP: hints.ai_socktype = SOCK_STREAM; hints.ai_protocol = IPPROTO_TCP; break;
        case UDP: hints.ai_socktype = SOCK_DGRAM;  hints.ai_protocol = IPPROTO_UDP; break;
    }

    addrinfo *info = (addrinfo*)malloc(sizeof(addrinfo));
    if (getaddrinfo(ip, port, &hints, &info)) {
        logprint("win32_init_socket()", "getaddrinfo error\n");
        return false;
    }

    addrinfo *ptr;
    for (ptr = info; ptr != NULL; ptr = ptr->ai_next) {
        sock->handle = (s32)socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (sock->handle == INVALID_SOCKET) {
            logprint("win32_init_socket()", "socket() call failed\n");
            continue;
        }

        if (sock->passive) {
            if (bind(sock->handle, ptr->ai_addr, (int)ptr->ai_addrlen) != SOCKET_ERROR) 
                break;
            logprint("win32_init_socket()", "bind() call failed\n");
            return false;
        } else {
            if (sock->protocol == UDP || connect(sock->handle, ptr->ai_addr, (int)ptr->ai_addrlen) != SOCKET_ERROR) 
                break;
            logprint("win32_init_socket()", "connect() call failed\n");
            return false;
        }
    }

    sock->info = win32_addrinfo_to_address_info(*ptr);
    sock->recv_info = win32_addrinfo_to_address_info(*ptr);
    freeaddrinfo(info);
    
    return true;
}

internal s32
win32_select(QSock_Socket sock, s32 seconds, s32 microseconds) {
    // Waits for a recieve on sock. if none it returns 0.
    fd_set fds;
    int n;
    struct timeval tv;
    
    FD_ZERO(&fds);
    FD_SET(sock.handle, &fds);
    
    tv.tv_sec = seconds;
    tv.tv_usec = microseconds;
    
    n = select(sock.handle, &fds, NULL, NULL, &tv);
    if (n == 0)
        return 1; // Timeout
    else if (n < 0) {
        logprint("Timeout()", "select() call failed!\n");
        return 0; // Timeout Failed
    }
    
    return 0; //No Timeout
}

internal void
win32_free_socket(struct QSock_Socket socket) {
    closesocket(socket.handle);
}

internal s32
win32_recv(struct QSock_Socket socket, const char *buffer, int buffer_size, int flags) {
    int bytes = recv(socket.handle, (char *)buffer, buffer_size, flags);
    if (bytes == SOCKET_ERROR) {
        logprint("win32_recv()", "error\n");
        win32_print_platform_error();
    }
    return bytes;
}

internal s32
win32_recv_from(struct QSock_Socket socket, const char *buffer, int buffer_size, int flags, struct QSock_Address_Info *info) {
    int bytes = recvfrom(socket.handle, (char *)buffer, buffer_size, flags, (struct sockaddr*)info->address, (int *)&info->address_length);
    if (bytes == SOCKET_ERROR) {
        logprint("win32_recv_from()", "error\n");
        win32_print_platform_error();
    }
    return bytes;
}

internal s32
win32_send(struct QSock_Socket socket, const char *buffer, int buffer_size, int flags) {
    int bytes = send(socket.handle, (char *)buffer, buffer_size, flags);
    if (bytes == SOCKET_ERROR) {
        logprint("win32_send()", "error\n");
        win32_print_platform_error();
    }
    return bytes;
}

internal s32
win32_send_to(struct QSock_Socket socket, const char *buffer, int buffer_size, int flags, struct QSock_Address_Info info) {
    int bytes = sendto(socket.handle, (char *)buffer, buffer_size, flags, (struct sockaddr*)info.address, info.address_length);
    if (bytes == SOCKET_ERROR) {
        logprint("win32_send_to()", "error\n");
        win32_print_platform_error();
    }
    
    return bytes;
}

internal void
win32_listen(struct QSock_Socket socket) {
    int backlog = 5;
    int error = listen(socket.handle, backlog);
    if (error == SOCKET_ERROR) { 
        print("win32_listen() error\n"); 
        win32_print_platform_error();
    }
}

internal void
win32_accept(struct QSock_Socket *socket, struct QSock_Socket *client) {
    if (!socket->passive) {
        fprintf(stderr, "win32_accept(): not a passive(server) socket\n");
        return;
    }
    else if (socket->protocol != TCP) {
        fprintf(stderr, "win32_accept(): not a TCP socket\n");
        return;
    }

    struct sockaddr address = {};
    s32 address_length = sizeof(sockaddr);

    client->handle = (s32)accept(socket->handle, &address, &address_length);
    if (client->handle == SOCKET_ERROR) {
        logprint("win32_accept()", "accept() call failed\n");
        win32_print_platform_error();
        return;
    }

    client->info.address = (const char *)malloc(sizeof(sockaddr));
    memcpy((void*)client->info.address, (void*)&address, sizeof(sockaddr));
    client->info.address_length = address_length;

    socket->connected = client;
}
