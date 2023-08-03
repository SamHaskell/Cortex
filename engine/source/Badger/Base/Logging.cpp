#include "Badger/Base/Logging.hpp"

static const char *logLevelLabels[6] = {
    "[FATAL]",
    "[ERROR]",
    "[WARN]",
    "[INFO]",
    "[DEBUG]",
    "[TRACE]",
};

static const char *logLevelColors[6] = {
    "\e[0;31m",
    "\e[0;31m",
    "\e[0;33m",
    "\e[0;32m",
    "\e[0;36m",
    "\e[0;37m",
};

void CoreLog(LogLevel verbosity, const char *msg, const char *file, i32 line, ...) {
    int buffer_size = 16000;
    char output[buffer_size];
    memset(output, 0, sizeof(output));

    __builtin_va_list p_args;
    va_start(p_args, line);
    vsnprintf(output, buffer_size, msg, p_args);
    va_end(p_args);
    printf("%s%-7s: %s Line: %i\n%-9s%s%s\n", logLevelColors[verbosity], logLevelLabels[verbosity], file, line, "", output, "\e[0m");
}