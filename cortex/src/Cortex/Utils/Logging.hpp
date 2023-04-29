#pragma once

#include "Cortex/Utils/Asserts.h"

/*
    Logging has 6 levels, final two CX_DEBUG and CX_TRACE are for debug builds only.
    -   CX_TRACE is designed to be used sparingly, as it will be VERY verbose.
    -   CX_WARN and CX_INFO will be present in release builds by default but
        can be stripped if desired.
    -   CX_ERROR and CX_FATAL will be present no matter what.
    Defined as bit flags so that filtering will be easier, but that is a job for future Sam.
    For now we just define our main logging macros and have simple printing as the backend.
*/

typedef enum LogLevel
{
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5,
} LogLevel;

typedef enum LogLevelBit
{
    LOG_LEVEL_FATAL_BIT = (1 << LOG_LEVEL_FATAL),
    LOG_LEVEL_ERROR_BIT = (1 << LOG_LEVEL_ERROR),
    LOG_LEVEL_WARN_BIT = (1 << LOG_LEVEL_WARN),
    LOG_LEVEL_INFO_BIT = (1 << LOG_LEVEL_INFO),
    LOG_LEVEL_DEBUG_BIT = (1 << LOG_LEVEL_DEBUG),
    LOG_LEVEL_TRACE_BIT = (1 << LOG_LEVEL_TRACE),
} LogLevelBit;

// TODO: Wrap these in #ifdef's to switch different levels on and off depending on compiler.

void CoreLog(LogLevel verbosity, const char *msg, const char *file, i32 line, ...);

#define CX_USE_LOGGING

#ifdef CX_USE_LOGGING

#define CX_FATAL(msg, ...) CoreLog(LOG_LEVEL_FATAL, msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define CX_ERROR(msg, ...) CoreLog(LOG_LEVEL_ERROR, msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define CX_WARN(msg, ...) CoreLog(LOG_LEVEL_WARN, msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define CX_INFO(msg, ...) CoreLog(LOG_LEVEL_INFO, msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define CX_DEBUG(msg, ...) CoreLog(LOG_LEVEL_DEBUG, msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define CX_TRACE(msg, ...) CoreLog(LOG_LEVEL_TRACE, msg, __FILE__, __LINE__, ##__VA_ARGS__)

#else

#define CX_FATAL(msg, ...)
#define CX_ERROR(msg, ...)
#define CX_WARN(msg, ...)
#define CX_INFO(msg, ...)
#define CX_DEBUG(msg, ...)
#define CX_TRACE(msg, ...)

#endif