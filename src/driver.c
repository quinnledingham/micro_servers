#include <stdlib.h> // system()
#include <unistd.h> // fork() sleep()
#include <stdio.h> // popen()

#include "../include/micro.h"

int main(int argc, char const *argv[])
{
    FILE *fp1, *fp2, *fp3, *fp4;
    // compile micro servers
    //system("g++ translator.c -o translator");
    //system("g++ translator.c -o convertor");
    //system("g++ voting.c -o voting");
    
    if (!fork())
    {
        char m[50];
        sprintf(m, "./translator %d", fp1PORT);
        fp1 = popen(m, "r");
    }
    else if (!fork())
    {
        char m[50];
        sprintf(m, "./convertor %d", fp2PORT);
        fp2 = popen(m, "r");
    }
    else if (!fork())
    {
        char m[50];
        sprintf(m, "./voting %d", fp3PORT);
        fp3 = popen(m, "r");
    }
    else
    {
        sleep(120);
        pclose(fp1);
        pclose(fp2);
        pclose(fp3);
    }
    
    return 0;
}