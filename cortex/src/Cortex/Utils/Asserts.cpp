#include "Cortex/Utils/Asserts.h"

#include <iostream>

void LogAssertionFailure(const char *expr, const char *file, i32 line)
{
    CoreLog(LOG_LEVEL_FATAL, "Assertion failed for expression %s", file, line, expr);
}

void LogAssertionFailureMsg(const char *expr, const char *file, i32 line, const char* msg)
{
    CoreLog(LOG_LEVEL_FATAL, "Assertion failed for expression %s. %s", file, line, expr, msg);
}