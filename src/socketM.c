#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <time.h>

#include "../include/socketM.h"

void createServer(struct Server* s, char* port, int proto)
{
    // Runs all functions to set up server. Ends by listening for clients.
    s->protocol = proto;
    s->info = addressInit(port, s->protocol);
    s->sock = socketM(*s->info);
    bindM(s->sock, *s->info);
    
    if (s->protocol == TCP)
        listenM(s->sock);
}

void waitForConnection(struct Server* s)
{
    // waits for a client to be accepteD
    s->otherSock = acceptM(s->sock, *s->info);
}

struct SockMsg* serverRecieve(struct Server* s)
{
    // changes what sock to use to send depending on if the server is TCP or UDP
    struct SockMsg* b;
    if (s->protocol == TCP)
    {
        b = recieveBuffer(s->otherSock, s->info, s->protocol, 2);
    }
    else if (s->protocol == UDP)
    {
        b = recieveBuffer(s->sock, s->info, s->protocol, 2);
    }
    return b;
}

void serverSend(struct Server* s, struct SockMsg *toSend)
{
    // Sends with different sock depending on if the server is TCP or UDP
    if (s->protocol == TCP)
    {
        sendBuffer(s->otherSock, s->info, s->protocol, toSend);
    }
    else if (s->protocol == UDP)
    {
        sendBuffer(s->sock, s->info, s->protocol, toSend);
    }
}

void serverSendPM(struct Server* s, char* p, char* msg)
{
    // Puts msg into the buffer, sets prompt to p
    struct SockMsg *m;
    m = charToBuffer(msg);
    setPrompt(m, p);
    serverSend(s, m);
    destroySockMsg(m);
}

void serverSendPSM(struct Server* s, char* p, char* sys, char* msg)
{
    // Puts msg into the buffer, sets prompt to p, sets system to sys.
    struct SockMsg *m;
    m = charToBuffer(msg);
    setSystem(m, sys);
    setPrompt(m, p);
    serverSend(s, m);
    destroySockMsg(m);
}

void createClient(struct Client* c, char* ip, char* port, int proto)
{
    // Running all the functions required to set up a client. Only does connect if it is TCP.
    c->protocol = proto;
    c->info = getHost(ip, port, c->protocol);
    c->sock = socketM(*c->info);
    if (c->protocol == TCP)
    {
        connectM(c->sock, *c->info);
    }
}

struct SockMsg* clientRecieve(struct Client* c)
{
    // Same sock for TCP and UDP
    struct SockMsg *b = recieveBuffer(c->sock, c->info, c->protocol, 3);
    return b;
}

void clientSend(struct Client* c, struct SockMsg *toSend)
{
    // Same sock for TCP and UDP
    sendBuffer(c->sock, c->info, c->protocol, toSend);
}

int timeout(int sock)
{
    // Waits for a recieve on sock. if none it returns 0.
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    int t = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));
    if (t < 0) {
        perror("Error\n");
    }
    fprintf(stderr, "\ntimeout: %d\n", t);
    return t;
}

struct SockMsg* recieveBuffer(int sock, struct addrinfo *info, int protocol, int type)
{
    char b[BUFFER_SIZE]; // used for both recieves
    memset(b, 0, sizeof(b));
    char message[MESSAGE_SIZE]; // used for TCP recieve
    char *cursor; 
    cursor = b;
    
    int bytesRec;
    int bytesTotal;
    
    while(1)
    {
        memset(message, 0, sizeof(message));
        if (protocol == TCP)
        {
            //fprintf(stderr, "waiting for msg recv\n");
            bytesRec = recv(sock, &message, sizeof(message), 0);
            
            if (bytesRec < 0)
            {
                fprintf(stderr, "%d\n", errno);
                fprintf(stderr, "recvM(): recv() call failed!\n");
                exit(1);
            }
            
            // keep track of bytes and where to write in the buffer next.
            bytesTotal = bytesTotal + bytesRec;
            memcpy(cursor, message, bytesRec);
            cursor = cursor + bytesRec;
            
            //fprintf(stderr, "\nbytesRec: %d\n", bytesRec);
            
            char end = 4;
            char* find = strchr(message, end);
            //fprintf(stderr,  "find: %s", find);
            if (find != 0)
                break;
        }
        else if (protocol == UDP)
        {
            //setBytesToBufferSize(b);
            
            // if it is a client wanting to recieve something 
            if (type == CLIENT) 
            {
                timeout(sock);
            }
            
            bytesRec = recvfrom(sock, b, BUFFER_SIZE, 0,
                                info->ai_addr, &info->ai_addrlen);
            if (bytesRec < 0)
            {
                //fprintf(stderr, "%d\n", errno);
                fprintf(stderr, "\nrecvfromM(): recvfrom() call failed!\n");
                return 0;
            }
            
            bytesTotal = bytesRec;
            break;
        }
    }
    
    // Convert the message recieved to a sock msg.
    struct SockMsg *a;
    a = charToSockMsg(b);
    // same the ip if it is a TCP connection. Useful to give a micro server the users ip
    if (protocol == TCP)
    {
        struct sockaddr_in *c = (struct sockaddr_in *)info->ai_addr;
        char ip[80];
        inet_ntop(info->ai_family, &(c->sin_addr), ip, 80);
        setIP(a, ip);
    }
    return a;
}

void sendBuffer(int sock, struct addrinfo *info, int protocol, struct SockMsg *toSend)
{
    // Converting the SockMsg to a char* so that it can be sent
    char *cursor = sockMsgToChar(toSend);
    
    // Putting the EOT value at the end of the message.
    cursor[strlen(cursor) + 1] = '\0';
    cursor[strlen(cursor)] = 4;
    
    //fprintf(stderr, "\nstrlen: %ld\n", strlen(cursor));
    int bytesToSend = strlen(cursor); // just using strlen now since I know it will be a string.
    //fprintf(stderr, "%d",  getBytes(toSend));
    int bytesSent;
    
    // Keep sending until all the bytesToSend are sent.
    while (bytesToSend > 0)
    {
        if (protocol == TCP) 
        {
            //fprintf(stderr, "send msg %s\n", cursor);
            bytesSent = send(sock, cursor, bytesToSend, 0);
            if (bytesSent < 0)
            {
                fprintf(stderr, "%d\n", errno);
                fprintf(stderr, "sendM: send() call failed!\n");
                exit(1);
            }
        }
        else if (protocol == UDP)
        {
            //fprintf(stderr, "sendto msg %s, %d\n", cursor, bytesToSend);
            bytesSent = sendto(sock, cursor, bytesToSend, 0,
                               info->ai_addr, info->ai_addrlen);
            if (bytesSent < 0)
            {
                fprintf(stderr, "sentoM(): sendto() call failed!\n");
                exit(1);
            }
        }
        
        cursor = cursor + bytesSent;
        //fprintf(stderr, "bytesToSend: %d bytesSent: %d", bytesToSend, bytesSent);
        bytesToSend = bytesToSend - bytesSent;
    }
}

struct addrinfo* getHost(char* ip, char* port, int type)
{
    struct addrinfo hints, *server_info;
    server_info = (struct addrinfo*)malloc(sizeof(struct addrinfo));
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    
    if (type == TCP)
        hints.ai_socktype = SOCK_STREAM;
    else if (type == UDP)
        hints.ai_socktype = SOCK_DGRAM;
    
    getaddrinfo(ip, port, &hints, &server_info);
    
    return server_info;
}

struct addrinfo* addressInit(char* port, int type)
{
    struct addrinfo hints, *server_info;
    server_info = (struct addrinfo*)malloc(sizeof(struct addrinfo));
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    
    if (type == TCP)
        hints.ai_socktype = SOCK_STREAM;
    else if (type == UDP)
        hints.ai_socktype = SOCK_DGRAM;
    
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    getaddrinfo(NULL, port, &hints, &server_info);
    return server_info;
}

int socketM(struct addrinfo server_info)
{
    int sock = socket(server_info.ai_family,
                      server_info.ai_socktype,
                      server_info.ai_protocol);
    if (sock == -1)
    {
        fprintf(stderr, "socketM(struct addrinfo server_info): socket() called failed!\n");
        exit(1);
    }
    
    return sock;
}


void connectM(int sock, struct addrinfo server_info)
{
    if ((connect(sock, server_info.ai_addr, server_info.ai_addrlen)) == -1)
    {
        fprintf(stderr, "connectM: connect() call failed!\n");
    }
}

void bindM(int sock, struct addrinfo server_info)
{
    if (bind(sock, server_info.ai_addr, server_info.ai_addrlen) == -1)
    {
        fprintf(stderr, "bindM(): bind() call failed!\n");
        exit(1);
    }
}

void listenM(int sock)
{
    if (listen(sock, 5) == -1)
    {
        fprintf(stderr, "listenM(): listen() call failed!\n");
        exit(1);
    }
}

int acceptM(int sock, struct addrinfo server_info)
{
    int newsock;
    if ((newsock = accept(sock, server_info.ai_addr, &server_info.ai_addrlen)) == -1)
    {
        fprintf(stderr, "acceptM: accept() call failed!\n");
        exit(1);
    }
    return newsock;
}