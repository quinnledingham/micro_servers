# qsock

## Example Programs

### Basic Echo

Testing program where the server passes back what the client sent.

### Implementing micro servers using socket programming
  
The user can connect to the indirection server which will connect to the micro servers so the user can use the micro services.  
The indirection server and the micro servers can all be running on a different machines.
  
![architecture](https://github.com/quinnledingham/MicroServicesServer/blob/main/images/architecture.PNG?raw=true)
  
The user and indirection server communicate using TCP.  
The indireciton server and the micro servers communicate using UDP.  
  
Last worked on in 2021

#### Manual
./build.sh compiles all of the files to build folder.

Then in the build folder:
./client <Server IP> <Port Number>
./indirectionServer <micro services ip> <Port Number>
./driver (runs all of the micro services)

Micro services ports are defined in mirco.h
How many seconds until voting is closed is defined in voting.h

1. Translate can translate 5 words from English to French
2. Currency Exchange asks for <amount> <source currency> <destination currency>
with no dollar sign in front of amount and using the 3 letter shortenings for  the currencies
3. Asks the user for a vote if they can vote.
4. Returns a summary if voting is closed.
9. Echo returns the user's messages back.
0. Disconnects the user from the indirection server.

All testing was done at home on a linux desktop and on the linux compute servers.
I was having trouble connecting to 136.159.5.27 but I think it worked one time.

As far as I know all the micro services work. The universal time works for the voting
micro service.

### Web Censorship Proxy

A proxy that can a browser can connect to and it will block a website if it contains a keyword to be blocked. Only works with HTTP. Implemented using sockect programming.

#### How it works

First connects to the browser and receives a HTTP GET request from it.
Scans the request for keywords.
Then it will connect to the web server in the request and sends the HTTP GET request and scans what is returned for the keywords it has stored if body text search is turned on.
If a keyword is found at anytime it sends a error screen.


#### Manual

The port number of the proxy can be changed by changing the definition of PROXYPORTNUM

Blocks pages that contain certain keywords. By default "Floppy" and "SpongeBob" are keywords
that are blocked.

Can telnet into proxy to dynamically add and remove keywords.
KEYWORDA+ adds a the word typed after the + to the list of blocked words.
KEYWORDD+ deletes the word typed after the + from the list of blocked words.
EXIT to close the connect. Needs to be done because there are no forks.