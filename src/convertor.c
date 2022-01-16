#include "../include/convertor.h"


#define PORTNUM "44107"

//Conversion Rates
#define USD 1
#define CAD 1.24
#define EURO 0.86
#define GBP 0.73
#define BTC 0.000016

int main(int argc, char **argv)
{
    struct Server s;
    struct SockMsg *msgIn;
    struct SockMsg *msgOut;
    
    double so;
    double soInUSD;
    double d;
    
    createServer(&s, argv[1], 1);
    
    while (1)
    {
        //char *p = "Enter <amount> <source currency> <destination currency>: \n";
        msgIn = serverRecieve(&s);
        printf("\n%s\n", msgIn->buffer);
        
        // if the user first connected.
        if (atoi(msgIn->buffer) == 2)
        {
            serverSendPSM(&s,
                          "Enter <amount> <source currency> <destination currency>:",
                          "clear",
                          "Connected to convertor service.\n\n"
                          "Supports these currencies:\n"
                          "Canadian Dollar(CAD), US Dollar(USD), Euro(EURO)\n"
                          "British Pound(GBP), and Bitcoin(BTC).\n"
                          "Type quit to leave.\n");
        }
        // else they must be asking for a conversion
        else
        {
            char copy[100];
            sprintf(copy, "%s", msgIn->buffer);
            
            char *c; 
            c = strtok(copy, " ");
            char *amount = c;
            
            c = strtok(NULL, " ");
            char *source = c;
            
            c = strtok(NULL, " ");
            char *dest = c;
            
            so = atof(amount);
            
            // Convert to USD
            if (strncmp("USD", source, 3) == 0)
            {
                soInUSD = so / USD;
            }
            else if (strncmp("CAD", source, 3) == 0)
            {
                soInUSD = so / CAD;
            }
            else if (strncmp("EURO", source, 4) == 0)
            {
                soInUSD = so / EURO;
            }
            else if (strncmp("GBP", source, 3) == 0)
            {
                soInUSD = so / GBP;
            }
            else if (strncmp("BTC", source, 3) == 0)
            {
                soInUSD = so / BTC;
            }
            
            // Convert to dest currency
            if (strncmp("USD", dest, 3) == 0)
            {
                d = soInUSD * USD;
            }
            else if (strncmp("CAD", dest, 3) == 0)
            {
                d = soInUSD * CAD;
            }
            else if (strncmp("EURO", dest, 4) == 0)
            {
                d = soInUSD * EURO;
            }
            else if (strncmp("GBP", dest, 3) == 0)
            {
                d = soInUSD * GBP;
            }
            else if (strncmp("BTC", dest, 3) == 0)
            {
                d = soInUSD * BTC;
            }
            
            // Send the converted amount back to 2 decimal places.
            char m[1000];
            sprintf(m, "%s %s = %.2f %s\n", amount, source, d, dest);
            msgOut = charToBuffer(m);
            
            setPrompt(msgOut, "Enter <amount> <source currency> <destination currency>: ");
            
            serverSend(&s, msgOut);
            destroySockMsg(msgOut);
        }
        
        destroySockMsg(msgIn);
    }
}
