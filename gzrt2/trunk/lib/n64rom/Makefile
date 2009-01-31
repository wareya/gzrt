###################
# N64 ROM Library #
###################

PROJECT = n64rom

# Parts
PIECES = n64rom.o n64crc.o swap.o

TARGET = lib$(PROJECT).a


# Preferred applications
CC = gcc
AR = ar

# Compiler settings
CFLAGS  = $(shell pkg-config --cflags glib-2.0) --std=gnu99 -O3 -I. \
	      -funsigned-char
	
$(TARGET): $(PIECES)
	$(AR) -rcs -o $(TARGET) $(PIECES)

clean:
	rm -fv $(TARGET) $(PIECES)

install: $(TARGET)
	cp -f n64rom.h ../inc
	cp -f $(TARGET) ..
