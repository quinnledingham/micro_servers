#ifndef BUFFER_H
#define BUFFER_H

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

#define BUFFER_SIZE 10000
#define IP_SIZE 80

// Older struct that isn't being used anymore
struct Buffer
{
    char *prompt;
    char *ip;
    int bytes;
    char *buffer;
};

struct SockMsg
{
    char *prompt; // tells the user what to enter
    char *system; // make a system call in the users terminal
    char *ip; // ip of the user
    int key; // key for encrypting
    int bytes; // bytes in buffer
    char *buffer; // the msg to be sent
};

// frees all the memory in a SockMsg
void destroySockMsg(struct SockMsg *d);

// allocates memory for a Buffer
struct Buffer* createBuffer();

// allocates memory for the buffer in a SockMsg
void createBlankBuffer(struct SockMsg *m);

// Allocates Memory for a SockMsg. Sets all the pointers inside to 0
struct SockMsg* createSockMsg();

// Buts msg into the buffer of a SockMsg
struct SockMsg* charToBuffer(char *msg);

// Sets the bytes in a SockMsg to the BUFFER_SIZE
void setBytesToBufferSize(struct SockMsg *m);

// Attempts to return the bytes in a SockMsg. Did not work.
int getBytes(struct SockMsg *m);

// Sets the ip in a SockMsg to newIP
void setIP(struct SockMsg *m, char *newIP);

// Sets the prompt in a SockMsg to setPrompt
void setPrompt(struct SockMsg *m, char *newPrompt);

// Sets the system in a SockMsg to newSystem
void setSystem(struct SockMsg *m, char *newSystem);

// Sets the buffer in SockMsg to newBuffer
void setBuffer(struct SockMsg *m, char *newBuffer);

// Converts a SockMsg to a char*. Combines all the members into one char*
char* sockMsgToChar(struct SockMsg *m);

// Converts a char* to a SockMsg. Undos the combining done by sockMsgToChar
struct SockMsg* charToSockMsg(char *c);

// Prints the values inside SockMsg
void printSockMsg(struct SockMsg *m);

// Used to convert SockMsg to char*. Combines base and add with a ~ after it all
void concat(char* base, char* add);
#endif //BUFFER_H
