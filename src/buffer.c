#include "../include/buffer.h"

struct SockMsg* charToBuffer(char *msg)
{
    struct SockMsg* b = createSockMsg();
    createBlankBuffer(b);
    sprintf(b->buffer, "%s", msg);
    b->bytes = strlen(b->buffer);
    b->buffer[b->bytes] = '\0';
    
    return b;
}

struct Buffer* createBuffer()
{
    struct Buffer *newB;
    newB = (struct Buffer*)malloc(sizeof(struct Buffer));
    newB->buffer = malloc(BUFFER_SIZE);
    memset(newB->buffer, 0, BUFFER_SIZE);
    newB->bytes = 0;
    
    return newB;
}

void createBlankBuffer(struct SockMsg *m)
{
    free(m->buffer);
    m->buffer = malloc(BUFFER_SIZE);
    memset(m->buffer, 0, BUFFER_SIZE);
    m->bytes = 0;
}

struct SockMsg* createSockMsg()
{
    struct SockMsg *newM;
    newM = (struct SockMsg*)malloc(sizeof(struct SockMsg));
    
    newM->prompt = malloc(1);
    memcpy(newM->prompt, "0", sizeof(newM->prompt));
    
    newM->system = malloc(1);
    memcpy(newM->system,  "0", sizeof(newM->prompt));
    
    newM->ip = malloc(1);
    memcpy(newM->ip, "0", sizeof(newM->prompt));
    
    newM->key = 0;
    newM->bytes = 0;
    
    newM->buffer = malloc(1);
    memcpy(newM->buffer,  "0", sizeof(newM->prompt));
    
    return newM;
}

void destroySockMsg(struct SockMsg *d)
{
    free(d->prompt);
    free(d->system);
    free(d->ip);
    free(d->buffer);
    free(d);
}

void setBytesToBufferSize(struct SockMsg *m)
{
    m->bytes = BUFFER_SIZE;
}

int getBytes(struct SockMsg *m)
{
    int total;
    total = m->bytes;
    total = total + strlen(m->prompt);
    total = total + strlen(m->ip);
    total = total + sizeof(m->key);
    total = total + sizeof(m->bytes);
    total = total + 3;
    
    return total;
}

void setIP(struct SockMsg *m, char *newIP)
{
    free(m->ip);
    m->ip = malloc(80);
    memset(m->ip, 0, 80);
    sprintf(m->ip, "%s", newIP);
}

void setPrompt(struct SockMsg *m, char *newPrompt)
{
    free(m->prompt);
    m->prompt = malloc(80);
    memset(m->prompt, 0, 80);
    sprintf(m->prompt, "%s", newPrompt);
}

void setSystem(struct SockMsg *m, char *newSystem)
{
    free(m->system);
    m->system = malloc(80);
    memset(m->system, 0, 80);
    sprintf(m->system, "%s", newSystem);
}

void setBuffer(struct SockMsg *m, char *newBuffer)
{
    m->buffer = malloc(BUFFER_SIZE);
    memset(m->buffer, 0, BUFFER_SIZE);
    sprintf(m->buffer, "%s", newBuffer);
    m->bytes = strlen(m->buffer);
    m->buffer[m->bytes] = '\0';
}

char* sockMsgToChar(struct SockMsg *m)
{
    //printSockMsg(m);
    char* c = malloc(BUFFER_SIZE);
    memset(c, 0, BUFFER_SIZE);
    
    concat(c, m->prompt);
    
    concat(c, m->system);
    
    concat(c, m->ip);
    
    char kText[20];
    sprintf(kText, "%d", m->key);
    concat(c, kText);
    
    char bText[20];
    sprintf(bText, "%d", m->bytes);
    concat(c, bText);
    
    concat(c, m->buffer);
    
    //fprintf(stderr, "%s", c);
    return c;
}

struct SockMsg* charToSockMsg(char *c)
{
    struct SockMsg *m = createSockMsg();
    char* e;
    
    e = strtok(c, "~");
    setPrompt(m, e);
    
    e = strtok(NULL, "~");
    setSystem(m, e);
    
    e = strtok(NULL, "~");
    setIP(m, e);
    
    e = strtok(NULL, "~");
    m->key = atoi(e);
    
    e = strtok(NULL, "~");
    m->bytes = atoi(e);
    
    e = strtok(NULL, "~");
    setBuffer(m, e);
    
    //printSockMsg(m);
    return m;
}

void printSockMsg(struct SockMsg *m)
{
    printf("prompt: %s\n", m->prompt);
    printf("system: %s\n", m->system);
    printf("ip: %s\n", m->ip);
    printf("key: %d\n", m->key);
    printf("bytes: %d\n", m->bytes);
    printf("buffer: %s\n", m->buffer);
}

void concat(char* base, char* add)
{
    char y[BUFFER_SIZE];
    sprintf(y, "%s~", add);
    strcat(base, y);
}