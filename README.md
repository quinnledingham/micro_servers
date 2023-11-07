# qsock

# Implementing micro servers using socket programming
  
The user can connect to the indirection server which will connect to the micro servers so the user can use the micro services.  
The indirection server and the micro servers can all be running on a different machines.
  
![architecture](https://github.com/quinnledingham/MicroServicesServer/blob/main/images/architecture.PNG?raw=true)
  
The user and indirection server communicate using TCP.  
The indireciton server and the micro servers communicate using UDP.  
  
Last worked on in 2021

### Manual
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
