#ifdef OS_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

// winsock
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "Ws2_32.lib")

#elif OS_LINUX

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <time.h>
#include <malloc.h>
#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>

#endif // OS

#include <string.h>
//#include <stdbool.h> // C
//#include <stdint.h> // C
#include <cstdint> // C++

#include <stdio.h>
#include <malloc.h>

#include "defines.h"

void *platform_malloc(u32 size) { 
    return malloc(size); 
}
void platform_free(void *ptr) { free(ptr); }
void platform_memory_copy(void *dest, void *src, u32 num_of_bytes) { 
    memcpy(dest, src, num_of_bytes); 
}
void platform_memory_set(void *dest, s32 value, u32 num_of_bytes) { memset(dest, value, num_of_bytes); }

#include "print.h"
#include "char_array.h"
#include "qsock.h"

#include "print.cpp"

