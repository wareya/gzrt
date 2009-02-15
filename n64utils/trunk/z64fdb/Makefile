#######################################
# Zelda 64 Function Database Makefile #
#######################################

# Project name
NAME = zfdb

# Program parts
PARTS = $(NAME).o msg.o db.o func.o mips.o sqlite3.o

# Environment
CC = gcc

# Glib
CFLAGS  += $(shell pkg-config --cflags glib-2.0)
LDFLAGS += $(shell pkg-config --libs glib-2.0)

# Standard flags
CFLAGS  += -D_GNU_SOURCE -std=gnu99 -Wall -O0 -g
LDFLAGS += -ldl -lpthread 

# Default target
all: $(NAME)

# Build the program
$(NAME): $(PARTS)
	$(CC) -o $(NAME) $(PARTS) $(LDFLAGS)

clean:
	rm $(NAME) $(PARTS)
