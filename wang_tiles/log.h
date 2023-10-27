#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>


#ifdef DEBUG
void log_mes(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}
#else
void log_mes(const char *format, ...) {}
#endif

#endif