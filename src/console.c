#include "../include/console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <ncurses.h>
#include <pthread.h>

static WINDOW *output_win;
static WINDOW *input_win;
static pthread_t input_thread_id;

void *input_thread(void *arg)
{
    char input[256];
    while (1)
    {
        wclear(input_win);
        wprintw(input_win, ">> ");
        wrefresh(input_win);
        wgetnstr(input_win, input, sizeof(input) - 1);

        // Handle the input command here
        if (strcmp(input, "exit") == 0)
        {
            endwin(); // End ncurses mode
            exit(0);
        }
        else
        {
            printf("%s: Invalid command!\n", input);
        }
        // Add more command handling here
    }
    return NULL;
}

void init_console()
{
    initscr(); // Initialize ncurses
    cbreak(); // Disable line buffering
    noecho(); // Disable echoing of characters
    keypad(stdscr, TRUE); // Enable special keys
    curs_set(1); // Show cursor

    int height, width;
    getmaxyx(stdscr, height, width);

    output_win = newwin(height - 1, width, 0, 0);
    input_win = newwin(1, width, height - 1, 0);

    scrollok(output_win, TRUE); // Enable scrolling for the output window

    pthread_create(&input_thread_id, NULL, input_thread, input_win);
}

void cleanup_console()
{
    endwin(); // End ncurses mode
}

void print_to_console(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vw_printw(output_win, fmt, args);
    va_end(args);
    wrefresh(output_win);
}

int fprintf_real(FILE *stream, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = vw_printw(output_win, fmt, args);
    va_end(args);
    wrefresh(output_win);
    return ret;
}
