#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#define MAX_LOG_MESSAGE_LENGTH 256

void log_sys_message(const char *fmt, ...);
void log_err_message(const char *fmt,...);

#endif // LOG_H
