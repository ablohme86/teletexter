#include "../include/log.h"
#include "../include/utils.h"
#include "../include/config.h"
#include "../include/console.h"
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
    snprintf(log_filename, sizeof(log_filename), "%s/%s-teletexter.log",config.loggingConfig.messageLogPath, date);

    
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
