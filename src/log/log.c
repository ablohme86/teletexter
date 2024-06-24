#include "../include/log.h"

#include <stdarg.h>

#include "../include/utils.h"
#include "../include/config/config.h"
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>

void create_logs_dir_if_not_exists() 
{
    const char *dir_path = config.loggingConfig.messageLogPath;
    struct stat st = {0};

    if (stat(dir_path, &st) == -1) 
    {
        mkdir(dir_path, 0700);
    }
}


void log_sys_message(const char *fmt, ...)
{
    char date[11];
    char time_str[9];
    get_date(date);
    get_time(time_str);

    va_list args;
    va_start(args, fmt);
    int log_msg_size = vsnprintf(NULL, 0, fmt, args) + 1;
    va_end(args);
    char *log_msg = malloc(log_msg_size);

    va_start(args, fmt);
    vsnprintf(log_msg, log_msg_size, fmt, args);
    va_end(args);

    int final_log_msg_size = snprintf(NULL, 0, "%s-%s %s\n", date, time_str, log_msg) + 1;
    char *final_log_msg = malloc(final_log_msg_size);
    snprintf(final_log_msg, final_log_msg_size, "%s-%s %s\n", date, time_str, log_msg);

    
    fprintf(stdout, "%s", final_log_msg);

    free(log_msg);
    free(final_log_msg);
}
void log_err_message(const char *fmt, ...)
{
    char date[11];
    char time_str[9];
    get_date(date);
    get_time(time_str);

   
    va_list args;
    va_start(args, fmt);
    int log_msg_size = vsnprintf(NULL, 0, fmt, args) + 1;
    va_end(args);
    char *log_msg = malloc(log_msg_size);

    va_start(args, fmt);
    vsnprintf(log_msg, log_msg_size, fmt, args);
    va_end(args);

    int final_log_msg_size = snprintf(NULL, 0, "%s %s %s\n", date, time_str, log_msg) + 1;
    char *final_log_msg = malloc(final_log_msg_size);
    snprintf(final_log_msg, final_log_msg_size, "%s %s %s\n", date, time_str, log_msg);

  
    fprintf(stderr, "%s", final_log_msg);

    free(log_msg);
    free(final_log_msg);
}