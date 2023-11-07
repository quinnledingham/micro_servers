#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../qsock.h"

#include "buffer.h"

//Conversion Rates
#define USD 1
#define CAD 1.24
#define EURO 0.86
#define GBP 0.73
#define BTC 0.000016

struct Conversion_Rate
{
	const char *tag;
	r32 rate;
};
global const Conversion_Rate rates[5] = {
	{"USD",  1.0f    },
	{"CAD",  1.24f   },
	{"EURO", 0.86f   },
	{"GBP",  0.73f   },
	{"BTC",  0.000016}
};

inline r32
find_rate(const char *tag) 
{
	for (u32 i = 0; i < 5; i++) {
		if (equal(rates[i].tag, tag)) {
			return rates[i].rate;
		}
	}
	return 0.0f;
}

int main(int argc, char *argv[])
{
	char *port = argv[1];
	Socket server = qsock_server(port, UDP);

	while(1) {
		Message received = recv_message(&server);

		// Using the micro service again
		if (equal(received.system, "Connecting")) {
			printf("Start Message Sending\n");
			Message m = {};
			m.prompt = string_malloc("Enter: <amount> <source currency> <destination currency>: ");
			m.text = string_malloc("Connected to convertor service.\n\n"
                          		   "Supports these currencies:\n"
                          		   "Canadian Dollar(CAD), US Dollar(USD), Euro(EURO)\n"
                          		   "British Pound(GBP), and Bitcoin(BTC).\n"
                          		   "Type quit to leave.\n");
			m.system = string_malloc("clear");
			send_message(server, m);
			free_message(m);
		}
		else {
			printf("\n%s\n", received.text);

			Message m = {};

			char copy[100];
            sprintf(copy, "%s", received.text);

            char *c; 
            c = strtok(copy, " ");
            const char *amount = c;
            
            c = strtok(NULL, " ");
            const char *source = c;
            
            c = strtok(NULL, " ");
            const char *dest = c;
            
            double so = atof(amount);
    		double soInUSD;
    		double d;

    		soInUSD = so / find_rate(source);
    		d = soInUSD * find_rate(dest);

    		// Send the converted amount back to 2 decimal places.
            m.text = (char *)malloc(100);
            sprintf(m.text, "%s %s = %.2f %s\n", amount, source, d, dest);

			m.prompt = string_malloc("Enter: <amount> <source currency> <destination currency>: ");
			send_message(server, m);

			free_message(m);
		}

		free_message(received);
	}

	qsock_free_socket(server);
	return 0;
}