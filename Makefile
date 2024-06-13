CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -pthread
LDFLAGS = -pthread

SRCS = src/teletexter.c src/client.c src/commands.c src/messages.c src/ident.c src/server.c src/lcdlib.c
OBJS = $(SRCS:.c=.o)

all: teletexter post_build_clean

teletexter: $(OBJS)
	$(CC) $(CFLAGS) -o teletexter $(OBJS) $(LDFLAGS) -lwiringPi -lwiringPiDev

clean:
	rm -f $(OBJS) teletexter

post_build_clean:
	rm -f $(OBJS)

.PHONY: all clean post_build_clean

