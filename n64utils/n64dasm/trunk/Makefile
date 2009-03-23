############################
# Nintendo 64 Disassembler #
############################

# Project name
NAME = n64dasm

# Program parts
PARTS = opprintf.o decoder.o $(NAME).o 

# Environment
CC = gcc

# Standard flags
CFLAGS  += -D_GNU_SOURCE -std=gnu99 -Wall -O0 -g
LDFLAGS += 

# Default target
all: $(NAME)

# Build the program
$(NAME): $(PARTS)
	$(CC) -o $(NAME) $(PARTS) $(LDFLAGS)

clean:
	rm -fv $(NAME) $(PARTS)
