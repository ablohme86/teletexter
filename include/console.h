#ifndef CONSOLE_H
#define CONSOLE_H

#include <ncurses.h>
#include <pthread.h>
#include <stdarg.h>

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

// Den originale fprintf-funksjonen
int fprintf_real(FILE *stream, const char *fmt, ...);

#endif // CONSOLE_H
