#ifndef OVERTURE_LOG
#define OVERTURE_LOG

typedef enum {
    LEVEL_TRACE,
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_WARN,
    LEVEL_ERROR,
    LEVEL_FATAL
} log_level;

void print_log(log_level level, const char* file, int line, const char* fmt, ...);

#define TRACE(...) print_log(LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define DEBUG(...) print_log(LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define INFO(...) print_log(LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define WARN(...) print_log(LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define ERROR(...) print_log(LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define FATAL(...) print_log(LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#endif
