#include "../include/utils.h"
#include <ctype.h>
#include <limits.h>
#include <errno.h>


void strip_spaces(char *str) 
{
    char *read = str;
    char *write = str;

    while (*read != '\0') 
    {
        if (*read != ' ') 
        {
            *write = *read;
            write++;
        }
        read++;
    }
    *write = '\0';
}
int is_numerical_cpy(const char *str, int *number)
{
    char *endptr;
    errno = 0; // tilbakestill errno før kall til strtol
    long val = strtol(str, &endptr, 10);

    // Sjekk for feil
    if (errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
    {
        return 0; // Underflow eller overflow funnet
    }

    if (endptr == str)
    {
        return 0; // Ingen tegn ble konvertert
    }

    // Sjekk for ytterligere tegn etter nummeret
    while (*endptr != '\0')
    {
        if (!isspace((unsigned char)*endptr))
        {
            return 0; // Det finnes ikke-numeriske tegn
        }
        endptr++;
    }

    *number = (int)val;
    return 1; // Gyldig nummer funnet og konvertert
}

int is_numerical(const char *str)
{
    if (str == NULL || *str == '\0')
    {
        return 0;
    }

    char *endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);

    if (errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
    {
        return 0;
    }

    if (endptr == str)
    {
        return 0;
    }

    while (*endptr != '\0') {
        if (!isspace((unsigned char)*endptr))
        {
            return 0;
        }
        endptr++;
    }

    return 1;
}


int check_valid_characters(char *str)
{
    while (*str)
    {
        if (!isalnum((unsigned char)*str))
        {
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
void strtoupper(char *str)
{
    while (*str)
    {
        *str = toupper((unsigned char)*str);
        str++;
    }
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

void print_date(int unixtime, char *output_variable) 
{
    // Konverter Unix-tiden til strukturen 'tm'
    time_t t = (time_t)unixtime;
    struct tm *tm_info = localtime(&t);

    // Formater datoen som "YYYY-MM-DD"
    strftime(output_variable, 20, "%Y-%m-%d", tm_info);
}

void print_datetime_short(int unixtime, char *output_variable)
{
    // Konverter Unix-tiden til strukturen 'tm'
    time_t t = (time_t)unixtime;
    struct tm *tm_info = localtime(&t);

    // Formater tiden som "HH:MM:SS"
    strftime(output_variable, 20, "%a %H:%M", tm_info);
}

long get_unixtime() {
    time_t current_time;

    // Få gjeldende tid
    current_time = time(NULL);

    return (long)current_time;
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
