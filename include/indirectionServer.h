#ifndef INDIRECTION_SERVER_H
#define INDIRECTION_SERVER_H

#include "socketM.h"

// handles the new client that joined.
void handleClient(struct Server* s);

// handles things to do with the translator micro server.
void translator(struct Server *s, struct SockMsg *userMsg);

// handles things to do with the convertor micro server.
void convertor(struct Server *s, struct SockMsg *userMsg);

// handles things to do with the voting part of the voter mirco server.
void voter(struct Server *s, struct SockMsg *userMsg);

// handles things to do with the result viewing part of the voter micro server.
void voterResult(struct Server *s, struct SockMsg *userMsg);

// implements a service that echos the user's messages.
void echo(struct Server *s);

#endif //INDIRECTION_SERVER_H
