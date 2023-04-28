#pragma once

#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef float f32;
typedef double f64;

typedef char b8;
typedef int b32;

STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte(s).");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 byte(s).");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 byte(s).");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 byte(s).");

STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte(s).");
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 byte(s).");
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 byte(s).");
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 byte(s).");

STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 byte(s).");
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 byte(s).");

STATIC_ASSERT(sizeof(b8) == 1, "Expected b8 to be 1 byte(s).");
STATIC_ASSERT(sizeof(b32) == 4, "Expected b32 to be 4 byte(s).");

#define CX_TRUE 1
#define CX_FALSE 0

#ifdef __APPLE__
    #define CX_PLATFORM_APPLE 1
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        #define CX_PLATFORM_MACOS 1
        // MacOS specific stuff
    #else
        #error "Cortex only supports MacOS Desktops at this time."
    #endif
#else
    #error "Cortex only supports Apple platforms at this time."
#endif