###########################
# MIPS R4000 Disassembler #
###########################

PROJECT = r4kdis

# Parts
PIECES = opprintf.o r4kdis.o

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
CFLAGS  = `pkg-config --cflags glib-2.0` --std=gnu99 -g -fPIC -O3 -I. \
    -funsigned-char

# Win32 again
ifeq ($(WIN32) ,1)
    CFLAGS += -mwindows -mms-bitfields -DWIN32
endif
    
$(TARGET): $(PIECES)
    $(CC) -o $(TARGET) $(PIECES) $(LDFLAGS)

../$(TARGET): $(TARGET)
    cp $(TARGET) .. -v

install: ../$(TARGET)

clean:
    rm -fv $(TARGET) *.o *~
