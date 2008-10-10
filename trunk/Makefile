##############################
# Zelda 64 ROM Tool Makefile #
##############################

PROJECT=gzrt

# Compiler options
CC=gcc

# Standard flags
CFLAGS += -I. `pkg-config --cflags gtk+-2.0 gthread-2.0` -O3 --std=gnu99 -funsigned-char

# Error control flags
CFLAGS += -Wfatal-errors

# Parts - GUI
PARTS=$(PROJECT).o gui/gui.o gui/splash.o gui/error.o gui/filesel.o \
	gui/main.o gui/pbar.o gui/about.o gui/extract.o gui/decomp.o gui/crc.o \
	gui/byteswap.o gui/mwi.o gui/list.o

# Parts - Zelda 64
PARTS += z64/z_actortable.o z64/z_scenetable.o z64/z_filesystem.o \
	z64/z_nametable.o z64/z_detect.o z64/z_yaz0.o
	
# Parts - ROM library
PARTS += rom/rom.o rom/crc.o

# Parts - other
PARTS += generic/generic.o
PARTS += generic/plugins.o
PARTS += generic/mem.o

# Debug!
ifeq ($(DEBUG), 1)
	PARTS += gui/debug.o
	CFLAGS += -DGZRT_DEBUG -g
endif

ifeq ($(MIMIC), 1)
	CFLAGS += -DGZRT_DEBUG_CONSOLE_MIMIC
endif

ifeq ($(WIN32), 1)
	CFLAGS += -DWIN32 -mwindows -mms-bitfields
endif

all: $(PARTS)
	$(CC) $(CFLAGS) $(PARTS) `pkg-config --libs gtk+-2.0 gthread-2.0` -ldl -o $(PROJECT)
	
# For creating final Win32 package
complete: all
	cp -v $(PROJECT).exe /c/GZRT/resources/bin

clean:
	rm -vf `find . -name "*.o"` # Object files
	rm -vf `find . -name "*~"`  # Temporary files

lines:
	cat `find . -name "*.c"` `find . -name "*.h"` | wc -l

bytes:
	cat `find . -name "*.c"` `find . -name "*.c"` | wc -c
	
# Dependencies - GUI
gui/gui.o:		gui/gui.h
gui/error.o:	gui/error.h	
gui/splash.o:	gui/splash.h
gui/filesel.o:	gui/filesel.h
gui/main.o:		gui/main.h 	
gui/pbar.o:		gui/pbar.h
gui/about.o:	gui/about.h 
