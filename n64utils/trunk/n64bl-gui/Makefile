
PROJECT = n64bl-gui

# Environment
CC = gcc

ifeq ($(OS), CYGWIN)
  CFLAGS += -D__CYGWIN__ -D__WIN32__
else
  ifeq ($(OS), WIN32)
	CFLAGS += -D__WIN32__
  endif
endif

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


