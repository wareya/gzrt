PROGRAM = ad

PARTS = $(PROGRAM).o

CC = gcc

CFLAGS = -g -Iz64 -In64rom `pkg-config --cflags glib-2.0`
LDFLAGS = -L. -ln64rom -lz64 `pkg-config --libs glib-2.0`

$(PROGRAM): $(PARTS)
	$(CC) -o $(PROGRAM) $(PARTS) $(LDFLAGS)
