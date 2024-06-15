#ifndef CONSOLE_H
#define CONSOLE_H

#include <ncurses.h>
#include <pthread.h>

// ANSI fargekoder
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Makroer for printf og fprintf
#define printf(...) print_to_console(__VA_ARGS__)
#define fprintf(stream, ...) \
do { \
if (stream == stdout) { \
print_to_console(__VA_ARGS__); \
} else { \
fprintf_real(stream, __VA_ARGS__); \
} \
} while(0)

void init_console();
void cleanup_console();
void print_to_console(const char *fmt, ...);
void sigwinch_handler(int signum);
void handle_echo(char *args);
void handle_exit(char *args);
// Den originale fprintf-funksjonen
int fprintf_real(FILE *stream, const char *fmt, ...);

#endif // CONSOLE_H
