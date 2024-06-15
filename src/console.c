#include "../include/console.h"
#include "../include/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>

static WINDOW *output_win;
static WINDOW *input_win;
static pthread_t input_thread_id;

typedef struct {
    char cmd_arg[20];
    void (*function)(char *);
} console_commands_t;


// Define commands and their corresponding functions
console_commands_t console_commands[] = {
    {"echo", handle_echo},
    {"exit", handle_exit},
    // Add more commands here as needed
    {"", NULL} // Null-terminate the array
};

// Funksjon for å håndtere "echo" kommandoen
void handle_echo(char *args)
{
    printf("Echo: %s\n", args);
}
void handle_exit(char *args)
{
    cleanup_console();

    exit(1);
}

void sigwinch_handler(int signum)
{
    // Handle window resize signal if needed
}

void *input_thread(void *arg)
{
    char input[256] = ""; // Buffer for user input
    int input_len = 0;    // Length of user input

    while (1)
    {
        wclear(input_win);
        wprintw(input_win, ">> %s", input); // Print prompt and current input
        wrefresh(input_win);

        int ch = wgetch(input_win); // Read a single character from input_win

        if (ch == ERR) // If there is no keyboard input
            continue;

        if (ch == '\n') // If the user presses ENTER
        {
            input[input_len] = '\0'; // Null-terminate the input
            input_len = 0;           // Reset the input length for the next input

            // Check if the input matches any command
            char *cmd = strtok(input, " ");
            char *args = strtok(NULL, "\0");
            int cmd_found = 0;
            for (int i = 0; console_commands[i].function != NULL; ++i)
            {
                if (strcmp(console_commands[i].cmd_arg, cmd) == 0)
                {
                    cmd_found = 1;
                    console_commands[i].function(args);
                    break;
                }
            }
            if (cmd_found == 0)
            {
                printf("%s: Unknown command, use 'help' to list all available commands\n", cmd);
            }

            // Clear input buffer for the next command
            memset(input, 0, sizeof(input));
        }
        else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) // Handle backspace
        {
            if (input_len > 0)
            {
                input[--input_len] = '\0'; // Remove the last character from the buffer
            }
        }
        else if (input_len < sizeof(input) - 1) // Check if there is space in the buffer
        {
            input[input_len++] = ch; // Add the character to the buffer
        }
    }

    return NULL;
}

void init_console()
{
    signal(SIGWINCH, sigwinch_handler); // Set your own signal handler

    initscr();  // Initialize ncurses
    cbreak();   // Disable line buffering
    noecho();   // Disable echoing of characters
    keypad(stdscr, TRUE); // Enable special keys
    curs_set(2); // Show cursor

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
