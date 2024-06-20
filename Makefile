# MAKEFILE FOR TELETEXTER
# COPYRIGHT (C) 2024 ALEXANDER BLOHME <ALEXANDER@TELETEXTER.ORG>

CC = gcc
CFLAGS = -Wall -Wunused-variable -Wunused-but-set-variable -Wextra -Iinclude -pthread 
LDFLAGS = -pthread -lncurses -lsqlite3
LDLIBS = -lwiringPi -lwiringPiDev

SRCS = src/user/user.c src/teletexter.c src/db/db_handler.c src/message/message.c src/gpio/gpio_handler.c src/lcd/lcd_disp.c src/server/handle_client.c src/server/server_commands.c src/server/handle_msg.c src/server/handle_ident.c src/server/server.c src/config/config.c src/log/log.c src/utils.c
OBJS = $(SRCS:.c=.o)
TARGET = bin/teletexter

# Sjekk for --DISABLE_LCD flagg
ifdef DISABLE_LCD
    SRCS := $(filter-out src/lcd/lcd_disp.c, $(SRCS))
    LDLIBS :=
    CFLAGS += -DDISABLE_LCD
endif

ifdef DEBUG
   CFLAGS += -DDEBUG
endif

ifdef DISABLE_SQL
   SRCS := $(filter-out src/db/db_handler.c, $(SRCS))
endif


all: teletexter post_build_clean

teletexter: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(OBJS) $(TARGET)

post_build_clean:
	rm -f $(OBJS)

.PHONY: all clean post_build_clean
