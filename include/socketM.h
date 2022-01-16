#ifndef SOCKETM_H
#define SOCKETM_H


#define MESSAGE_SIZE 80

#define TCP 0
#define UDP 1
#define SERVER 2
#define CLIENT 3

#include "buffer.h"

/*
* Server struct and functions
*/
struct Server
{
    struct addrinfo *info;
    int sock;
    int otherSock;
    int protocol;
};

// Sets up a server that can be TCP or UDP
void createServer(struct Server* s, char* port, int proto);

// Waits for a the accept function to return a socket. Puts it in the
// server struct.
void waitForConnection(struct Server* r);

// The server struct recieves a message. Using recv for TCP and recvfrom
// for UDP. Changes the sock for TCP and UDP.
struct SockMsg* serverRecieve(struct Server* s);

// Sends the buffer toSend from the server. send for TCP and sendTo for UDP
// Changes the sock for TCP and UDP.
void serverSend(struct Server* s, struct SockMsg *toSend);

// does serverSend but with setting the sockmsg prompt and setting the buffer to msg
void serverSendPM(struct Server* s, char* p, char* msg);

// does serverSend but with setting prompt = p, system = sys, and buffer = msg. 
void serverSendPSM(struct Server* s, char* p, char* sys, char* msg);

/*
 * Client struct and functions
*/
struct Client
{
    struct addrinfo *info;
    int sock;
    int protocol;
};

// Sets up a client that can be TCP or UDP
void createClient(struct Client* s, char* ip, char* port, int proto);

// Client recieves a Buffer. Same for both TCP and UDP.
struct SockMsg* clientRecieve(struct Client* c);

// Client sends the  Buffer toSend. Same for TCP and UDP.
void clientSend(struct Client* c, struct SockMsg *toSend);

// waits for a time and if nothing is recieved it will return 0
// https://www.it-swarm-fr.com/fr/c/udp-socket-set-timeout/1070229989/
int timeout(int sock);

/*
* Socket functions to use with the Servers and Clients
*/
// recieves a buffer using either TCP or UDP. Converts the char* to SockMsg
struct SockMsg* recieveBuffer(int sock, struct addrinfo *info, int protocol, int type);

// sends a buffer. converts SockMsg to char* to send.
void sendBuffer(int sock, struct addrinfo *info, int protocol, struct SockMsg *toSend);

// gets info about the host for a client
struct addrinfo* getHost(char* ip, char* port, int type);

// gets info to set up a server
struct addrinfo* addressInit(char* port, int type);

// creates socket
int socketM(struct addrinfo server_info);

// connects a socket to the server_info
void connectM(int sock, struct addrinfo server_info);

// binds a socket to the server_info
void bindM(int sock, struct addrinfo server_info);

// listens for a client wanting to join
void listenM(int sock);

// Accepts a client
int acceptM(int sock, struct addrinfo server_info);

#endif