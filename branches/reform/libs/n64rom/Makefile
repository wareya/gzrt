###################
# N64 ROM Library #
###################

PROJECT = n64rom

# Parts
PIECES = n64rom.o swap.o

# Win32 stuff
ifeq ($(WIN32), 1)
	TARGET = lib$(PROJECT).dll
else
	TARGET = lib$(PROJECT).so
endif

# Preferred applications
CC = gcc

# Compiler settings
LDFLAGS = -shared -W1,-soname,$(TARGET) `pkg-config --libs glib-2.0`
CFLAGS  = `pkg-config --cflags glib-2.0` --std=gnu99 -g \
	-funsigned-char -O3 -I.

# Win32 again
ifeq ($(WIN32) ,1)
	CFLAGS += -mwindows -mms-bitfields
endif
	
$(TARGET): $(PIECES)
	$(CC) -o $(TARGET) $(PIECES) $(LDFLAGS)

../$(TARGET): $(TARGET)
	cp $(TARGET) .. -v

install: ../$(TARGET)

clean:
	rm -fv $(TARGET) *.o *~
