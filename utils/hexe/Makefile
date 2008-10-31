PROGRAM = hexe

PARTS = $(PROGRAM).o

CC = gcc
CFLAGS = -funroll-loops -g -pedantic -ansi

$(PROGRAM): $(PARTS)
	$(CC) -o $(PROGRAM) $(PARTS)
