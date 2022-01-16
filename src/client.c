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

#include "../include/client.h"

int main(int argc, char **argv)
{
    // Check if there are enough arguments
    if (argc != 3)
    {
        printf("Usage: ./client <Server IP> <Port Number>\n");
        exit(1);
    }
    
    system("clear");
    
    // Create client to connect to indirection server
    struct Client c;
    struct SockMsg *userIn;
    struct SockMsg *userOut;
    createClient(&c, argv[1], argv[2], 0);
    
    while (1)
    {
        
        userOut = clientRecieve(&c);
        
        // if quit was returned that means the client and server is going to disconnect now.
        if (strncmp(userOut->buffer, "quit", 4) == 0)
        {
            break;
        }
        
        // There is a system command that was returned.
        if (strncmp(userOut->system, "0", 1) != 0)
            system(userOut->system);
        else
            printf("\n");
        
        printf("%s", userOut->buffer);
        printf("\n");
        
        userIn = getUserInput(userOut->prompt);
        
        // If a key was returned.
        if (userOut->key != 0)
        {
            // convert what the user inputted into a int and multiple it by the key.
            int vote = atoi(userIn->buffer);
            int encrypted = userOut->key * vote;
            char m[100];
            sprintf(m, "%d", encrypted);
            destroySockMsg(userIn);
            userIn = charToBuffer(m);
        }
        
        clientSend(&c, userIn); // Send to the indirection server
        
        destroySockMsg(userIn);
        destroySockMsg(userOut);
    }
    
    close(c.sock);
    return 0;
}

struct SockMsg* getUserInput(char* prompt)
{
    struct SockMsg *data = createSockMsg();
    createBlankBuffer(data);
    
    char c;
    printf("%s", prompt);
    while ((c = getchar()) != '\n')
    {
        data->buffer[data->bytes] = c;
        data->bytes++;
    }
    
    data->buffer[data->bytes] = '\0';
    
    return data;
}