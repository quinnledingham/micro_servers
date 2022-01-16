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

#include "../include/indirectionServer.h"
#include "../include/micro.h"

char *msip;

// default menu
char* mainmenu =
"Select an action:\n"
"1. Translate\n"
"2. Currency Exchange\n"
"3. Vote\n"
"4. View Voting Result\n"
"9. Echo\n"
"0. Close Connection\n";

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: ./indirectionServer <micro services ip> <Port Number>\n");
        exit(1);
    }
    
    // ip of the mirco services
    msip = argv[1];
    
    // Server to talk to the user
    struct Server s;
    createServer(&s, argv[2], TCP);
    
    pid_t pid;
    while(1)
    {
        waitForConnection(&s); // wait for some one to join
        pid = fork();
        if (pid == 0)
            handleClient(&s); // fork and handle the client
        else
        {}
    }
    
    return 0;
}

void handleClient(struct Server* s)
{
    printf("%s", msip);
    printf("\nClient Connected.\n");
    
    int i = 0;
    
    // Sends the default menu to the user
    struct SockMsg *request;
    request = charToBuffer(mainmenu);
    setPrompt(request, "Command: ");
    serverSend(s, request);
    destroySockMsg(request);
    
    while (1)
    {
        request = serverRecieve(s);
        fprintf(stderr, "\n%s\n", request->buffer);
        
        if(atoi(request->buffer) == 1)
        {
            translator(s, request);
        }
        else if (atoi(request->buffer) == 2)
        {
            convertor(s, request);
        }
        else if (atoi(request->buffer) == 3)
        {
            voter(s, request);
        }
        else if (atoi(request->buffer) == 4)
        {
            voterResult(s, request);
        }
        else if (atoi(request->buffer) == 9)
        {
            echo(s);
        }
        else if (atoi(request->buffer) == 0)
        {
            serverSendPM(s, " ", "quit"); // tell the client they are getting disconneted.
            close(s->otherSock);
            printf("\nClient Disconnected.\n");
            break;
        }
        else
        {
            serverSendPM(s, "Command: ","\nNot a service.\n");
        }
        
        // When Ctrl-C is used to leave the client the server gets stuck in a loop of recieve blank
        // messages. This will stop it from going on forever.
        if (request->bytes == 0)
        {
            if (i++ > 50)
            {
                printf("\nToo many blank messages (Client probably is no longer connected).\n");
                break;
            }
        }
        
        destroySockMsg(request);
    }
}


void translator(struct Server *s, struct SockMsg *userMsg)
{
    // Create client with the first micro server port.
    struct Client c;
    char p[6];
    sprintf(p, "%d", fp1PORT);
    createClient(&c, msip, p, UDP);
    
    struct SockMsg *msg;
    msg = charToBuffer(userMsg->buffer);
    
    while (1)
    {
        // Send message to udp mirco server and get message back.
        clientSend(&c, msg);
        destroySockMsg(msg);
        msg = clientRecieve(&c);
        
        // What was returned is 0. The mirco server should never return 0 so the return timeout.
        if (msg == 0)
        {
            serverSendPM(s, "Command: ", "Translator service is down.\n");
            break;
        }
        
        // print what was recieved the the server console.
        fprintf(stderr, "\n%s\n", msg->buffer);
        
        // Send message back to user and get new message back.
        serverSend(s, msg);
        destroySockMsg(msg);
        msg = serverRecieve(s);
        
        // if the user sent quit leave the translater service
        if (strncmp(msg->buffer, "quit", 4) == 0)
        {
            char y[160];
            char x[160];
            sprintf(y, "%s", "Disconnected from translator service.\n\n");
            sprintf(x, "%s", mainmenu);
            strcat(y, x);
            
            serverSendPSM(s, "Command: ", "clear", y);
            break;
        }
    }
}


void convertor(struct Server *s, struct SockMsg *userMsg)
{
    // Create client with 2nd micro server port
    struct Client c;
    char p[6];
    sprintf(p, "%d", fp2PORT);
    fprintf(stderr, "%s", p);
    createClient(&c, msip, p, UDP);
    
    struct SockMsg *msg;
    msg = charToBuffer(userMsg->buffer);
    
    while(1)
    {
        // Send user msg to the mirco server and get its response.
        clientSend(&c, msg);
        destroySockMsg(msg);
        msg = clientRecieve(&c);
        
        if (msg == 0)
        {
            serverSendPM(s, "Command: ", "Convertor service is down.\n");
            break;
        }
        
        fprintf(stderr, "\n%s\n", msg->buffer);
        
        // Send the mirco server's response to the client.
        serverSend(s, msg);
        destroySockMsg(msg);
        msg = serverRecieve(s);
        
        // if the user says they want to quit this micro server.
        if (strncmp(msg->buffer, "quit", 4) == 0)
        {
            char y[160];
            char x[160];
            sprintf(y, "%s", "Disconnected from convertor service.\n\n");
            sprintf(x, "%s", mainmenu);
            strcat(y, x);
            
            serverSendPSM(s, "Command: ", "clear", y);
            break;
        }
    }
}



void voter(struct Server *s, struct SockMsg *userMsg)
{
    // Create a client with the 3rd micro server port.
    struct Client c;
    char p[6];
    sprintf(p, "%d", fp3PORT);
    createClient(&c, msip, p, UDP);
    
    struct SockMsg *msg;
    msg = charToBuffer(userMsg->buffer);
    setIP(msg, userMsg->ip); // save the ip of the client to give the micro server.
    
    while(1)
    {
        // send the user message to the micro server and get its response
        clientSend(&c, msg);
        destroySockMsg(msg);
        msg = clientRecieve(&c);
        
        if (msg == 0)
        {
            serverSendPM(s, "Enter a command: ", "Voting service is down.\n");
            break;
        }
        
        // send to the user if they have already voted.
        if (strncmp(msg->buffer, "VoteD", 5) == 0)
        {
            char y[160];
            char x[160];
            sprintf(y, "%s", "You have already voted. \n\n");
            sprintf(x, "%s", mainmenu);
            strcat(y, x);
            
            serverSendPSM(s, "Command: ", "clear", y);
            break;
        }
        // send to the user if their vote was successful.
        else if (strncmp(msg->buffer, "VoteS", 5) == 0)
        {
            char y[160];
            char x[160];
            sprintf(y, "%s", "Vote successful.\n\n");
            sprintf(x, "%s", mainmenu);
            strcat(y, x);
            
            serverSendPSM(s, "Command: ", "clear", y);
            break;
        }
        // send to the user if their vote was not successful.
        else if (strncmp(msg->buffer, "VoteF", 5) == 0)
        {
            char y[160];
            char x[160];
            sprintf(y, "%s", "Vote failed.\n\n");
            sprintf(x, "%s", mainmenu);
            strcat(y, x);
            
            serverSendPSM(s, "Command: ", "clear", y);
            break;
        }
        // send to the user if the time to vote is over.
        else if (strncmp(msg->buffer, "VoteO", 5) == 0)
        {
            char y[160];
            char x[160];
            sprintf(y, "%s", "The time to vote is over.\n\n");
            sprintf(x, "%s", mainmenu);
            strcat(y, x);
            
            serverSendPSM(s, "Command: ", "clear", y);
            break;
        }
        
        fprintf(stderr, "\n%s\n", msg->buffer);
        
        // Send the micro server response to the client.
        serverSend(s, msg);
        destroySockMsg(msg);
        msg = serverRecieve(s);
        
        // if the user tells the indirectionServer to stop talking to the micro server.
        if (strncmp(msg->buffer, "quit", 4) == 0)
        {
            char y[160];
            char x[160];
            sprintf(y, "%s", "Disconnected from voting service.\n");
            sprintf(x, "%s", mainmenu);
            strcat(y, x);
            
            serverSendPSM(s, "Command: ", "clear", y);
            break;
        }
    }
}

void voterResult(struct Server *s, struct SockMsg *userMsg)
{
    // Again make a client with the third micro server port.
    struct Client c;
    char p[6];
    sprintf(p, "%d", fp3PORT);
    createClient(&c, msip, p, UDP);
    
    struct SockMsg *msg;
    msg = charToBuffer("summary"); // gonna tell the mirco server to send the voting summary.
    setIP(msg, userMsg->ip); // Save the ip of the client to give to the micro server.
    
    while(1)
    {
        // Send and recieve from the micro server.
        clientSend(&c, msg);
        destroySockMsg(msg);
        msg = clientRecieve(&c);
        
        if (msg == 0)
        {
            serverSendPM(s, "Command: ", "Voting service is down.\n");
            break;
        }
        
        //  tell the user that they did not vote during the voting time.
        if (strncmp(msg->buffer, "VoteV", 5) == 0)
        {
            char y[160];
            char x[160];
            sprintf(y, "%s", "You did not vote. \n\n");
            sprintf(x, "%s", mainmenu);
            strcat(y, x);
            
            serverSendPSM(s, "Command: ", "clear", y);
            break;
        }
        // send what the mirco server returned (the voting summary) along with the main menu
        else
        {
            char y[160];
            sprintf(y, "%s", mainmenu);
            strcat(msg->buffer, y);
        }
        
        fprintf(stderr, "\n%s\n", msg->buffer);
        
        serverSend(s, msg);
        destroySockMsg(msg);
        
        break;
    }
}


void echo(struct Server *s)
{
    // Send the instructions for the echo service
    struct SockMsg *msg;
    serverSendPSM(s, "Enter a message: ",
                  "clear",
                  "Echo:\n"
                  "Will send back whatever you send.\n"
                  "Type quit to leave.\n");
    while(1)
    {
        msg = serverRecieve(s);
        fprintf(stderr, "%s\n", msg->buffer);
        
        // if the user said they want to quit
        if (strncmp(msg->buffer, "quit", 4) == 0)
        {
            char y[160];
            char x[160];
            sprintf(y, "%s", "Disconnected from Echo service.\n\n");
            sprintf(x, "%s", mainmenu);
            strcat(y, x);
            
            serverSendPSM(s, "Command: ", "clear", y);
            break;
        }
        // set prompt and send the sockmsg with the same buffer as theirs back.
        setPrompt(msg, "Enter a message: ");
        char *newLine = "\n";
        strcat(msg->buffer, newLine);
        serverSend(s, msg);
        destroySockMsg(msg);
    }
    destroySockMsg(msg);
}