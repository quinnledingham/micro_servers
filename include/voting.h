#ifndef VOTING_H
#define VOTING_H

#include "socketM.h"

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

#define votingClose 60

#define START 0
#define MIDDLE 1
#define END 2

struct Candidate
{
    char *name;
    int id;
    int votes;
};

// Used to make a candiate to be voted for
struct Candidate* createCandidate(char *n, int i, int v);

// Used to store the ip's of people who have voted
struct ipList
{
    struct ipNode *head;
    struct ipNode *start;
    struct ipNode *end;
};

struct ipNode
{
    int t;
    char ip[80];
    struct ipNode *next;
};

// Initializes the list. After nodes can be added.
void ipListInit(struct ipList *i);

// Adds a node to the list. The node contains the ip.
void ipListAdd(struct ipList *i, char* ip);

// Returns a node allocated that contains the ip. 
struct ipNode* createipNode(char *ip);

// Checks if this ip has already voted. Compares ip to the ip's in the list.
// Returns 1 if the ip has voted before. Returns 0 if not.
int alreadyVoted(struct ipList *i, char *ip);

#endif //VOTING_H
