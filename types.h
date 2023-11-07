#ifndef TYPES_H
#define TYPES_H

#include <stdint.h> // C
//#include <cstdint> // C++
#include <string.h>
/*
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

*/

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef s8 b8;
typedef s32 b32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float r32;
typedef double r64;
typedef r32 f32;
typedef r64 f64;

#define function      static
#define internal      static
#define local_persist static
#define global        static

#define DEG2RAD 0.0174533f
#define PI      3.14159265359f
#define EPSILON 0.00001f

void *platform_malloc(u32 size);
void platform_free(void *ptr);

#define ARRAY_COUNT(n)     (sizeof(n) / sizeof(n[0]))
#define ARRAY_MALLOC(t, n) ((t*)platform_malloc(n * sizeof(t)))

#endif //TYPES_H