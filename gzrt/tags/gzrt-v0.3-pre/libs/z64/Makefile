#############################
# Zelda 64 Function Library #
#############################

PROJECT = z64

# Parts
PIECES = z64nt.o z64fs.o z64yaz0.o z64detect.o

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
CFLAGS  = `pkg-config --cflags glib-2.0` -O3 --std=gnu99 -g -fPIC -I. \
	-funsigned-char

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
