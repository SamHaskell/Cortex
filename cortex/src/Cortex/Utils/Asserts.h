#pragma once

#include "Cortex/Utils/Defines.h"

void LogAssertionFailure(const char* expr, const char* file, i32 line);

#define CX_USE_ASSERTS // Disable asserts by removing this define.

#ifdef CX_USE_ASSERTS
    #define DebugBreak() __builtin_trap()

    #define CX_ASSERT(expr)                                                             \
        {                                                                               \
            if (expr) {} else {                                                         \
                LogAssertionFailure(#expr, __FILE__, __LINE__);                         \
                DebugBreak();                                                           \
            }                                                                           \
        }

    #ifdef _DEBUG
        #define CX_DEBUGASSERT(expr)                                                        \
            {                                                                               \
                if (expr) {} else {                                                         \
                    LogAssertionFailure(#expr, __FILE__, __LINE__);                         \
                    DebugBreak();                                                           \
                }                                                                           \
            }
    #else
        #define CX_DEBUGASSERT(expr)
    #endif

#else
    #define CX_ASSERT(expr)
    #define CX_DEBUGASSERT(expr)
#endif

