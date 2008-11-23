PROGRAM = ghe

PARTS = $(PROGRAM).o func.o ops.o debug.o file.o

CC = gcc
CFLAGS  = -I. -g -pedantic -ansi -Wall
LDFLAGS = 

$(PROGRAM): $(PARTS)
	$(CC) -o $(PROGRAM) $(PARTS) $(LDFLAGS)
	
clean:
	rm $(PARTS) $(PROGRAM) *~ -fv
