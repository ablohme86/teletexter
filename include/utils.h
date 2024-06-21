#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>



#define TIME_STR_LEN 20

int is_numerical_cpy(const char *str, int *number);
int is_numerical(const char *str);
void get_time(char *time_str); // Eks: 13:28
void get_current_time(char current_time[15]);
void get_date(char *date_str); // Eks: 18.12.86
void get_short_weekday(char *weekday_str); // Eks: Fri (for fredag)
void strip_newline(char *str);
int check_valid_characters(char *str); // Checks if the string contains only a-z, A-Z, 0-9
char *strip_newline_return(char *str);
long get_unixtime();
void print_date(int unixtime, char *output_variable);
void print_time(int unixtime, char *output_variable);

// Andre funksjonsdeklarasjoner her...

#endif // UTILS_H
