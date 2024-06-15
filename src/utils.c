#include "../include/utils.h"


void get_time(char *time_str) 
{
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(time_str, TIME_STR_LEN, "%H:%M", timeinfo);
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
