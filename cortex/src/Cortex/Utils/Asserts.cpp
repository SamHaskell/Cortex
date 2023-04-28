#include "Cortex/Utils/Asserts.h"

#include <iostream>

void LogAssertionFailure(const char* expr, const char* file, i32 line) {
    printf("[ASSERTION]: Assertion failed for expression %s.\nFile: %s\nLine: %i\n", expr, file, line);
}