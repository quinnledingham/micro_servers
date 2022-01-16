#include "../include/voting.h"

#define KEY 2387


int main(int argc, char **argv)
{
    // Save the time when the micro service was started.
    struct timespec time;
    struct tm *timeinfo;
    timespec_get(&time, TIME_UTC);
    time_t votingOpen = time.tv_sec;
    
    struct Server s;
    struct SockMsg *msgIn;
    struct SockMsg *msgOut;
    
    // Create the candidates for people to vote on.
    struct Candidate *c[4];
    c[0] = createCandidate("Louella Conway", 5238, 10);
    c[1] = createCandidate("Jordan Larson", 6483, 12);
    c[2] = createCandidate("Jean Banks", 3233, 15);
    c[3] = createCandidate("Coleen Green", 6327, 19);
    
    size_t cSize = sizeof(c)/sizeof(c[0]); // how many candidates there are
    
    createServer(&s, argv[1], 1);
    
    // Initialize list of ip's who have voted.
    struct ipList i;
    ipListInit(&i);
    
    while (1)
    {
        //char *p = "Enter your vote: \n";
        msgIn = serverRecieve(&s);
        printf("\n%s\n", msgIn->buffer);
        printf("%s\n", msgIn->ip);
        
        // The time of the most request.
        timespec_get(&time, TIME_UTC);
        time_t votingA = time.tv_sec;
        fprintf(stderr, "timeNow: %ld votingStart: %ld", votingA, votingOpen);
        int timeDiff = votingA - votingOpen; // calculate howw long it has been since the
        // micro service started.
        
        // if the user has requested to vote
        if (atoi(msgIn->buffer) == 3)
        {
            // Check if voting is still open
            if (timeDiff > votingClose)
            {
                // Voting is closed
                serverSend(&s, charToBuffer("VoteO"));
            }
            else
            {
                // Voting is open
                // Check if the ip has already voted
                if (alreadyVoted(&i, msgIn->ip) == 0)
                {
                    // It hasn't voted
                    // Send back the candidates and key
                    char m[1000];
                    char y[100];
                    sprintf(m, "%s", 
                            "Connected to voting service.\n\n"
                            "Candidates:\n");
                    //fprintf(stderr, "yo");
                    for (int i = 0; i < cSize; i++)
                    {
                        sprintf(y, "%s %d\n", c[i]->name, c[i]->id);
                        strcat(m, y);
                    }
                    
                    //sprintf(y, "%s", "Type quit to leave.\n");
                    //strcat(m, y);
                    
                    msgOut = charToBuffer(m);
                    setPrompt(msgOut, "Enter your vote:  ");
                    setSystem(msgOut, "clear");
                    msgOut->key  = KEY;
                    serverSend(&s, msgOut);
                    destroySockMsg(msgOut);
                }
                else
                {
                    // The ip has voted before
                    serverSend(&s, charToBuffer("VoteD"));
                }
            }
            
        }
        // if the user is requesting the summary
        else if (strncmp(msgIn->buffer, "summary", 7) == 0)
        {
            // Check if voting is closed
            if (timeDiff > votingClose)
            {
                // Voting isn't closed
                // Check if the ip has voted
                if (alreadyVoted(&i, msgIn->ip) == 1)
                {
                    // it has voted
                    // send back the voting summary.
                    char m[1000];
                    char y[1000];
                    sprintf(m, "%s", "Voting Summary:\n");
                    
                    for (int i = 0; i < cSize; i++)
                    {
                        sprintf(y, "%s %d: %d\n", c[i]->name, c[i]->id, c[i]->votes);
                        strcat(m, y);
                    }
                    
                    sprintf(y, "\n");
                    strcat(m, y);
                    
                    msgOut = charToBuffer(m);
                    setPrompt(msgOut, "Command: ");
                    setSystem(msgOut, "clear");
                    serverSend(&s, msgOut);
                    destroySockMsg(msgOut);
                }
                else
                {
                    // the ip hasn't voted. tell the user the time is up and they did
                    // not vote.
                    serverSend(&s, charToBuffer("VoteV"));
                }
            }
            else
            {
                // The voting time isn't closed yet so the summary can't be viewed.
                // Get time of when voting will be doen and send it to the user.
                time_t closingTime = votingOpen + votingClose;
                timeinfo = localtime (&closingTime);
                //printf ("Current local time and date: %s", asctime(timeinfo));
                
                char m[100];
                sprintf(m, "Voting ends at %s"
                        "After that the results can be viewed\n\n", 
                        asctime(timeinfo));
                
                msgOut = charToBuffer(m);
                setPrompt(msgOut, "Command: ");
                setSystem(msgOut, "clear");
                serverSend(&s, msgOut);
                destroySockMsg(msgOut);
            }
        }
        // If it is nothing else the user must be voting.
        else
        {
            // Doesn't need to check time and voting status because the 
            // indirection server would not have let it here without testing it first.
            int encrypted = atoi(msgIn->buffer);
            int unencrypted = encrypted / KEY;
            int v = 0;
            
            for (int i = 0; i < cSize; i++)
            {
                if (unencrypted == c[i]->id)
                {
                    v = 1;
                    c[i]->votes++;
                }
            }
            
            if (v == 1)
            {
                ipListAdd(&i, msgIn->ip); // add ip because of successful vote
                serverSend(&s, charToBuffer("VoteS")); // tells the indirectionserver the vote
                // was successful
            }
            else
            {
                // tells the indirectionserver the vote failed
                serverSend(&s, charToBuffer("VoteF"));
            }
        }
        
        destroySockMsg(msgIn);
    }
}

struct Candidate* createCandidate(char *n, int i, int v)
{
    struct Candidate *newC;
    newC = (struct Candidate*)malloc(sizeof(struct Candidate));
    newC->name = n;
    newC->id = i;
    newC->votes = v;
    
    return newC;
}

void ipListInit(struct ipList *i)
{
    i->head = createipNode("start");
    i->head->t = START;
    i->head->next = createipNode("end");
    i->head->next->t = END;
}

void ipListAdd(struct ipList *i, char* ip)
{
    struct ipNode *cursor = i->head;
    while (cursor->next->t != END)
    {
        cursor = cursor->next;
    }
    
    struct ipNode *newN = createipNode(ip);
    newN->next = cursor->next;
    cursor->next = newN;
}

struct ipNode* createipNode(char *ip)
{
    struct ipNode *newN;
    newN = (struct ipNode*)malloc(sizeof(struct ipNode));
    snprintf(newN->ip, 80, "%s", ip);
    newN->t = MIDDLE;
    
    return newN;
}

int alreadyVoted(struct ipList *i, char *ip)
{
    struct ipNode *cursor = i->head->next;
    int voted = 0;
    while (cursor->t != END)
    {
        if (strcmp(ip, cursor->ip) == 0)
        {
            voted = 1;
        }
        cursor = cursor->next;
    }
    
    return voted;
}