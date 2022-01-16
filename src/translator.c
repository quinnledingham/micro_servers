#include "../include/translator.h"

int main(int argc, char **argv)
{
    struct Server s;
    struct SockMsg *msgIn;
    struct SockMsg *msgOut;
    
    createServer(&s, argv[1], 1);
    
    while (1)
    {
        //char *p = "Enter an English word: \n";
        msgIn = serverRecieve(&s);
        printf("\n%s\n", msgIn->buffer);
        
        // if the user is first connecting
        if (atoi(msgIn->buffer) == 1)
        {
            serverSendPSM(&s,
                          "Enter a English word: ",
                          "clear",
                          "Connected to translator service.\n"
                          "Words that can be translated:\n"
                          "bread, yellow, hello,\n"
                          "bye, chair.\n"
                          "Type quit to leave.\n");
        }
        // else they must be asking for a translation
        else 
        {
            char *f = "French translation: ";
            char m[1000];
            
            if (strncmp(msgIn->buffer, "bread", 5) == 0)
            {
                sprintf(m, "%s%s", f, "pain\n");
                msgOut = charToBuffer(m);
            }
            else if (strncmp(msgIn->buffer, "yellow", 6) == 0)
            {
                sprintf(m, "%s%s", f, "jaune\n");
                msgOut = charToBuffer(m);
            }
            else if (strncmp(msgIn->buffer, "hello", 5) == 0)
            {
                sprintf(m, "%s%s", f, "bonjour\n");
                msgOut = charToBuffer(m);
            }
            else if (strncmp(msgIn->buffer, "bye", 3) == 0)
            {
                sprintf(m, "%s%s", f, "au revoir\n");
                msgOut = charToBuffer(m);
            }
            else if (strncmp(msgIn->buffer, "chair", 5) == 0)
            {
                sprintf(m, "%s%s", f, "chaise\n");
                msgOut = charToBuffer(m);
            }
            else
            {
                sprintf(m, "%s", "Can't translate this word.\n");
                msgOut = charToBuffer(m);
            }
            setPrompt(msgOut, "Enter an English word: ");
            serverSend(&s, msgOut);
            destroySockMsg(msgOut);
        }
        
        destroySockMsg(msgIn);
    }
    
    close(s.sock);
}