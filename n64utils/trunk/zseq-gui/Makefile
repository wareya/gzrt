
PROJECT = zseq-gui

# Environment
CC = gcc

# Utils flags
CFLAGS  += -O3 $(shell pkg-config --cflags gtk+-2.0)
LDFLAGS += $(shell pkg-config --libs gtk+-2.0)

# Application pieces
PIECES	= $(PROJECT).o

all: $(PIECES)
	$(CC) -o $(PROJECT) $(PIECES) $(LDFLAGS)

# Cleanup
clean:
	rm -vf $(PROJECT) *.o 
