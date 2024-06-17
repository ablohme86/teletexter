#include "../include/utils.h"
#include <ctype.h>

int check_valid_characters(char *str) {
    while (*str) {
        if (!isalnum((unsigned char)*str)) {
            return 0; // Return 0 if a non-alphanumeric character is found
        }
        str++;
    }
    return 1; // Return 1 if all characters are alphanumeric
}


void get_time(char *time_str) 
{
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(time_str, TIME_STR_LEN, "%H:%M", timeinfo);
}
char *strip_newline_return(char *str)
{
    strip_newline(str);
    return str;
}
void strip_newline(char *str)
{
    char *p = str;
    while (*p)
    {
        if (*p == '\r' || *p == '\n')
        {
            *p = '\0';
            break;
        }
        p++;
    }
}


void get_date(char *date_str) 
{
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(date_str, TIME_STR_LEN, "%d.%m.%y", timeinfo);
}

void get_short_weekday(char *weekday_str) 
{
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(weekday_str, TIME_STR_LEN, "%a", timeinfo);
}

// Andre hjelpefunksjoner kan implementeres her...
