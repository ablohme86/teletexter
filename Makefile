# MAKEFILE FOR TELETEXTER
# COPYRIGHT (C) 2024 ALEXANDER BLOHME <ALEXANDER@TELETEXTER.ORG>

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -pthread
LDFLAGS = -pthread -lncurses
LDLIBS = -lwiringPi -lwiringPiDev

SRCS = src/error_handler.c src/teletexter.c src/lcd_disp.c src/client.c src/server_commands.c src/console.c src/messages.c src/ident.c src/server.c src/config.c src/log.c src/utils.c
OBJS = $(SRCS:.c=.o)
TARGET = bin/teletexter

# Sjekk for --disablelcd flagg
ifdef DISABLE_LCD
    SRCS := $(filter-out src/lcd_disp.c, $(SRCS))
    LDLIBS :=
    CFLAGS += -DDISABLE_LCD
endif

all: teletexter post_build_clean

teletexter: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(OBJS) $(TARGET)

post_build_clean:
	rm -f $(OBJS)

.PHONY: all clean post_build_clean

