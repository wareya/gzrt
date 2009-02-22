PROGRAM = n64bl

PARTS = $(PROGRAM).o crc.o

CC = gcc

CFLAGS = -O0 -g

$(PROGRAM): $(PARTS)
	$(CC) -o $(PROGRAM) $(PARTS)

clean:
	rm -fv *~ *.o *.bin $(PROGRAM)
