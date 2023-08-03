#pragma once

#include "Badger/Base/Defines.hpp"

#include <iostream>

typedef enum LogLevel {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5,
} LogLevel;

typedef enum LogLevelBit {
    LOG_LEVEL_FATAL_BIT = (1 << LOG_LEVEL_FATAL),
    LOG_LEVEL_ERROR_BIT = (1 << LOG_LEVEL_ERROR),
    LOG_LEVEL_WARN_BIT = (1 << LOG_LEVEL_WARN),
    LOG_LEVEL_INFO_BIT = (1 << LOG_LEVEL_INFO),
    LOG_LEVEL_DEBUG_BIT = (1 << LOG_LEVEL_DEBUG),
    LOG_LEVEL_TRACE_BIT = (1 << LOG_LEVEL_TRACE),
} LogLevelBit;

void CoreLog(LogLevel verbosity, const char *msg, const char *file, i32 line, ...);

#define USE_LOGGING

#ifdef USE_LOGGING
    #define LOG_FATAL(msg, ...) CoreLog(LOG_LEVEL_FATAL, msg, __FILE__, __LINE__, ##__VA_ARGS__)
    #define LOG_ERROR(msg, ...) CoreLog(LOG_LEVEL_ERROR, msg, __FILE__, __LINE__, ##__VA_ARGS__)
    #define LOG_WARN(msg, ...)  CoreLog(LOG_LEVEL_WARN, msg, __FILE__, __LINE__, ##__VA_ARGS__)
    #define LOG_INFO(msg, ...)  CoreLog(LOG_LEVEL_INFO, msg, __FILE__, __LINE__, ##__VA_ARGS__)
    #define LOG_DEBUG(msg, ...) CoreLog(LOG_LEVEL_DEBUG, msg, __FILE__, __LINE__, ##__VA_ARGS__)
    #define LOG_TRACE(msg, ...) CoreLog(LOG_LEVEL_TRACE, msg, __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define LOG_FATAL(msg, ...)
    #define LOG_ERROR(msg, ...)
    #define LOG_WARN(msg, ...)
    #define LOG_INFO(msg, ...)
    #define LOG_DEBUG(msg, ...)
    #define LOG_TRACE(msg, ...)
#endif