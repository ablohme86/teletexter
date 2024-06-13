# MAKEFILE FOR TELETEXTER
# COPYRIGHT (C) 2024 ALEXANDER BLOHME <ALEXANDER@TELETEXTER.ORG>


CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -pthread
LDFLAGS = -pthread

SRCS = src/teletexter.c src/client.c src/commands.c src/messages.c src/ident.c src/server.c src/lcd_disp.c src/config.c src/log.c src/utils.c
OBJS = $(SRCS:.c=.o)
TARGET = bin/teletexter
all: teletexter post_build_clean

teletexter: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS) -lwiringPi -lwiringPiDev

clean:
	rm -f $(OBJS) $(TARGET)

post_build_clean:
	rm -f $(OBJS)

.PHONY: all clean post_build_clean

