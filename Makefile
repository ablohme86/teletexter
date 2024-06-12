# Makefile for teletexter.c

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra

# Libraries
LIBS = -lwiringPi -lwiringPiDev

# Target executable
TARGET = teletexter

# Source files
SRCS = teletexter.c lcdlib.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Linking object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Compiling source files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean target
clean:
	rm -f $(OBJS) $(TARGET)
