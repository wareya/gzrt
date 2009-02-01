PROGRAM = gzrt

# App parts
PARTS = $(PROGRAM).o msg.o plugin.o gui.o func.o db.o file.o sig.o sqlite3.o

# GUI subparts
PARTS +=  gui/about.o gui/open.o gui/main.o

# Applications
CC = gcc

# Libraries
LIBS_EXTERN = $(shell pkg-config --libs gtk+-2.0)
LIBS_LOCAL  = -ln64rom -lz64

# Headers
INC_EXTERN = $(shell pkg-config --cflags gtk+-2.0)
INC_LOCAL  = -I. -I.. -Ilib/inc

# Defines
DEFINES = -DGZRT_DEBUG -D_GNU_SOURCE

CFLAGS  = -O0 -g --std=gnu99 $(INC_LOCAL) $(INC_EXTERN) $(DEFINES)
LDFLAGS = -Llib $(LIBS_LOCAL) $(LIBS_EXTERN)

all: libs $(PROGRAM)

$(PROGRAM): $(PARTS)
	$(CC) -o $(PROGRAM) $(PARTS) $(LDFLAGS)

libs:
	make -C lib/n64rom install
	make -C lib/z64    install

clean:
	make -C lib/n64rom clean
	make -C lib/z64    clean
	rm -fv $(PROGRAM) $(PARTS)
