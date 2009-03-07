PROGRAM = yaz0enc

PARTS = $(PROGRAM).o

CC = gcc

CFLAGS  = -O3 $(shell pkg-config --cflags glib-2.0) --std=gnu99
LDFLAGS = $(shell pkg-config --libs glib-2.0)

$(PROGRAM): $(PARTS)
	$(CC) -o $(PROGRAM) $(PARTS) $(LDFLAGS)

install: $(PROGRAM)
	cp -v $(PROGRAM) ../yenc

