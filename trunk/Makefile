##############################
# Zelda 64 ROM Tool Makefile #
##############################

PROJECT=gzrt

# Compiler options
CC=gcc

# Standard flags
CFLAGS += -g -DIN_GZRT -I. `pkg-config --cflags gtk+-2.0 gmodule-2.0` -O3 --std=gnu99 -funsigned-char

# Include search paths...
CFLAGS += -Ilibs/z64 -Ilibs/n64rom

# Error control flags
CFLAGS += -Wfatal-errors -Wall -Wno-char-subscripts

# Linker flags
LDFLAGS += `pkg-config --libs gtk+-2.0 gmodule-2.0` -Llibs -lz64 -ln64rom

# Parts - GUI
PARTS=$(PROJECT).o gui/gui.o gui/splash.o gui/error.o gui/filesel.o gui/main.o \
	gui/about.o gui/extract.o gui/decomp.o gui/crc.o gui/byteswap.o \
	gui/list.o gui/replace.o gui/debug.o gui/conf.o

# Parts - other
PARTS += app/generic.o
PARTS += app/plugins.o
PARTS += app/mem.o
PARTS += app/settings.o

# Debug!
ifeq ($(DEBUG), 1)
	CFLAGS += -DGZRT_DEBUG
endif

ifeq ($(MIMIC), 1)
	CFLAGS += -DGZRT_DEBUG_CONSOLE_MIMIC
endif

ifeq ($(WIN32), 1)
	CFLAGS += -DWIN32 -mwindows -mms-bitfields
endif

all: $(PARTS)
	make -C libs
	make -C plugins
	$(CC) $(CFLAGS) $(PARTS) $(LDFLAGS) -o $(PROJECT)
	
# For creating final Win32 package
win32: $(PARTS)
	make -C libs
	make -C plugins
	make -C launcher
	$(CC) $(CFLAGS) $(PARTS) $(LDFLAGS) -o $(PROJECT)
	cp -v $(PROJECT).exe /c/GZRT/resources/bin
	cp -v launcher/$(PROJECT).exe /c/GZRT/GZRT.exe
	cp -v plugins/*.dll /c/GZRT/plugins/
	cp -v libs/*.dll /c/GZRT/resources/bin/
	cp -v resources/gzrt.ini /c/GZRT/resources/

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
