# MAKEFILE FOR TELETEXTER
# COPYRIGHT (C) 2024 ALEXANDER BLOHME <ALEXANDER@TELETEXTER.ORG>

CC = gcc
CFLAGS = -Wall -Wunused-variable -Wunused-but-set-variable -Wextra -Iinclude -pthread 
LDFLAGS = -pthread -lncurses -lsqlite3
# LDLIBS = -lwiringPi -lwiringPiDev

SRCS = src/server/handle_admin.c src/lcd/lcd_txt.c src/server/handle_lcdtxt.c src/user/user.c src/signals.c src/teletexter.c src/db/db_handler.c src/message/message.c src/gpio/gpio_handler.c src/lcd/lcd_disp.c src/server/handle_client.c src/server/server_commands.c src/server/handle_msg.c src/server/handle_ident.c src/server/server.c src/config/config.c src/log/log.c src/utils.c
OBJS = $(SRCS:.c=.o)
TARGET = bin/teletexter

# Directories
BINDIR = /usr/local/sbin
CONFDIR = /etc/teletexter
CONFIG_FILES = configs/teletexter.cfg
SYSTEMDDIR = /etc/systemd/system
SERVICE_FILE = service/teletexter.service


# Sjekk for --DISABLE_LCD flagg
ifdef DISABLE_LCD
    SRCS := $(filter-out src/lcd/lcd_txt.c src/lcd/lcd_disp.c, $(SRCS))
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
	rm -f bin/*.db
post_build_clean:
	rm -f $(OBJS)

# Install target
install: $(TARGET)
	install -d $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)
	install -d $(CONFDIR)
	install -m 644 $(CONFIG_FILES) $(CONFDIR)
	install -m 644 $(SERVICE_FILE) $(SYSTEMDDIR)
	systemctl enable teletexter.service
	systemctl start teletexter.service

# Uninstall target
uninstall:
	rm -f $(BINDIR)/$(TARGET)
	rm -rf $(CONFDIR)
	systemctl stop teletexter.service
	systemctl disable teletexter.service
	rm -f $(SYSTEMDDIR)/teletexter.service

.PHONY: all clean install uninstall
