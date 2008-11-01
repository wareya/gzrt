PROGRAM = hexe

PARTS = $(PROGRAM).o

CC = gcc
CFLAGS = -funroll-loops -g -pedantic -ansi `pkg-config --cflags glib-2.0` -Wall
LDFLAGS = `pkg-config --libs glib-2.0`

$(PROGRAM): $(PARTS)
	$(CC) -o $(PROGRAM) $(PARTS) $(LDFLAGS)
	
clean:
	rm $(PARTS) $(PROGRAM) *~ -fv
