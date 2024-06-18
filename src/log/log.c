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

void log_message(const char *ip, const char *nickname, const char *message) 
{
    char date[11];
    char time_str[9];
    get_date(date);
    get_time(time_str);

    // Create logs directory if it doesn't exist
    create_logs_dir_if_not_exists();

    char log_filename[MAX_PATH_LENGTH];
    snprintf(log_filename, sizeof(log_filename), "%s/%s-teletexter-messages.log",config.loggingConfig.messageLogPath, date);


    // Open log file for appending
    FILE *file = fopen(log_filename, "a");
    if (file) 
    {
        fprintf(file, "%s %s [%s] <%s>: %s\n", date, time_str, ip, nickname, message);
        fclose(file);
    }
    else 
    {
        fprintf(stderr, "Failed to open log file for appending.\n");
    }
}


void log_sys_message(const char *fmt, ...)
{
    char date[11];
    char time_str[9];
    get_date(date);
    get_time(time_str);

    // Create logs directory if it doesn't exist
    create_logs_dir_if_not_exists();

    char log_filename[MAX_PATH_LENGTH];
    char log_msg[MAX_LOG_MESSAGE_LENGTH];

    snprintf(log_filename, sizeof(log_filename), "%s/%s-teletexter-system.log", config.loggingConfig.messageLogPath, date);

    va_list args;
    va_start(args, fmt);
    vsnprintf(log_msg, sizeof(log_msg), fmt, args);
    va_end(args);

    char final_log_msg[MAX_LOG_MESSAGE_LENGTH];
    snprintf(final_log_msg, sizeof(final_log_msg), "%s-%s %s\n", date, time_str, log_msg);

    // Open log file for appending
    FILE *file = fopen(log_filename, "a");
    if (file)
    {
        fprintf(file, "%s", final_log_msg);
        fclose(file);
    }
    else
    {
        fprintf(stderr, "Failed to open log file for appending.\n");
        exit(1);
    }

    // Print to console
    printf("%s", final_log_msg);
}
