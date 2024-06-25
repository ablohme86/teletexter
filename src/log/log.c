#include "../include/log.h"

#include <stdarg.h>

#include "../include/utils.h"
#include "../include/config/config.h"
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>




void log_sys_message(const char *fmt, ...)
{
    char date[11];
    char time_str[9];
    get_date(date);
    get_time(time_str);


    int log_filename_size = snprintf(NULL, 0, "%s/%s-teletexter-system.log", config.loggingConfig.systemLogPath, date) + 1;
    char *log_filename = malloc(log_filename_size);
    snprintf(log_filename, log_filename_size, "%s/%s-teletexter-system.log", config.loggingConfig.systemLogPath, date);

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

    int final_log_msg_output_size = snprintf(NULL,0,"%s\n",log_msg) +1;
    char *final_log_msg_output = malloc(final_log_msg_output_size);
    snprintf(final_log_msg_output,final_log_msg_output_size,"%s\n",log_msg);
    
    // Open log file for appending
    FILE *file = fopen(log_filename, "a");
    if (file)
    {
        fprintf(file, "%s", final_log_msg);
        fclose(file);
    }
    else
    {
        fprintf(stderr, "Failed to open log file '%s' for appending.\n", log_filename);
        exit(1);
    }

    // Print to console
    fprintf(stdout, "%s", final_log_msg_output);

    free(log_filename);
    free(log_msg);
    free(final_log_msg);
}
void log_err_message(const char *fmt, ...)
{
    char date[11];
    char time_str[9];
    get_date(date);
    get_time(time_str);


    int log_filename_size = snprintf(NULL, 0, "%s/%s-error-teletexter-system.log", config.loggingConfig.systemLogPath, date) + 1;
    char *log_filename = malloc(log_filename_size);
    snprintf(log_filename, log_filename_size, "%s/%s-error-teletexter-system.log", config.loggingConfig.systemLogPath, date);

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
    
    int final_log_msg_output_size = snprintf(NULL,0,"%s\n",log_msg) +1;
    char *final_log_msg_output = malloc(final_log_msg_output_size);
    snprintf(final_log_msg_output,final_log_msg_output_size,"%s\n", log_msg);

    // Open log file for appending
    FILE *file = fopen(log_filename, "a");
    if (file)
    {
        fprintf(file, "%s", final_log_msg);
        fclose(file);
    }
    else
    {
        fprintf(stderr, "Failed to open log file %s for appending.\n", log_filename);
        exit(1);
    }

    // Print to console
    fprintf(stderr, "%s", final_log_msg_output);

    free(log_filename);
    free(log_msg);
    free(final_log_msg);
}