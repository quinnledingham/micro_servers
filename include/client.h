#ifndef CLIENT_H
#define CLIENT_H

#include "../include/socketM.h"

// Returns a SockMsg with the buffer containing what the user typed
struct SockMsg* getUserInput(char* prompt);

#endif