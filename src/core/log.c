#include "core/log.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <libgen.h>

const char *levels[] = {
  "[TRACE]:", 
  "[DEBUG]:", 
  "[INFO]:", 
  "[WARN]:", 
  "[ERROR]:", 
  "[FATAL]:"
};

const char *colors[] = {
  "\x1b[90m", 
  "\x1b[36m", 
  "\x1b[32m", 
  "\x1b[33m", 
  "\x1b[31m", 
  "\x1b[35m"
};

void print_log(log_level_t level, const char *file, int line, const char *fmt, ...) {
    time_t timer;
    char buffer[26];
    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%F %T", tm_info);

    fprintf(stderr, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", buffer, colors[level], levels[level], basename((char *)file), line);

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
    fflush(stderr);
}
