#pragma once

#include "Cortex/Utils/Logging.hpp"

void LogAssertionFailure(const char *expr, const char *file, i32 line);
void LogAssertionFailureMsg(const char *expr, const char *file, i32 line, const char *msg);

#define CX_USE_ASSERTS // Disable asserts by removing this define.

#ifdef CX_USE_ASSERTS
#define DebugBreak() __builtin_trap()

#define CX_ASSERT(expr)                                     \
    {                                                       \
        if (expr)                                           \
        {                                                   \
        }                                                   \
        else                                                \
        {                                                   \
            LogAssertionFailure(#expr, __FILE__, __LINE__); \
            DebugBreak();                                   \
        }                                                   \
    }

#define CX_ASSERT_MSG(expr, msg)                                     \
    {                                                                \
        if (expr)                                                    \
        {                                                            \
        }                                                            \
        else                                                         \
        {                                                            \
            LogAssertionFailureMsg(#expr, __FILE__, __LINE__, #msg); \
            DebugBreak();                                            \
        }                                                            \
    }

#ifdef _DEBUG
#define CX_DEBUGASSERT(expr)                                \
    {                                                       \
        if (expr)                                           \
        {                                                   \
        }                                                   \
        else                                                \
        {                                                   \
            LogAssertionFailure(#expr, __FILE__, __LINE__); \
            DebugBreak();                                   \
        }                                                   \
    }                                   
#define CX_DEBUGASSERT_MSG(expr, msg)                                \
    {                                                                \
        if (expr)                                                    \
        {                                                            \
        }                                                            \
        else                                                         \
        {                                                            \
            LogAssertionFailureMsg(#expr, __FILE__, __LINE__, #msg); \
            DebugBreak();                                            \
        }                                                            \
    }
#else
#define CX_DEBUGASSERT(expr)
#endif

#else
#define CX_ASSERT(expr)
#define CX_DEBUGASSERT(expr)
#endif
