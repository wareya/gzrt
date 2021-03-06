##############################
# Zelda 64 ROM Tool Makefile #
##############################

PROJECT=gzrt

# Compiler options
CC=gcc

# GTK flags
GTK_FLAGS	 = $(shell pkg-config gtk+-2.0 --cflags)
GTK_LIBS	 = $(shell pkg-config gtk+-2.0 --libs)
GMODULE_LIBS = $(shell pkg-config gmodule-2.0 --libs)

# Standard flags
CFLAGS += -g -DIN_GZRT $(GTK_FLAGS) -I. -O3 --std=gnu99 -funsigned-char

# Include search paths...
CFLAGS += -Ilibs/z64 -Ilibs/n64rom

# Error control flags
CFLAGS += -Wfatal-errors -Wall -Wno-char-subscripts

# Linker flags
LDFLAGS += $(GMODULE_LIBS) $(GTK_LIBS) -Llibs -lz64 -ln64rom

# Parts - GUI
PARTS=$(PROJECT).o gui/gui.o gui/splash.o gui/error.o gui/filesel.o gui/main.o \
	gui/about.o gui/extract.o gui/decomp.o gui/crc.o gui/byteswap.o \
	gui/list.o gui/replace.o gui/debug.o gui/conf.o gui/utility.o

# Parts - other
PARTS += app/generic.o app/plugins.o app/mem.o app/settings.o app/md5.o \
	app/cache.o
	
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

# Build main app 
all: $(PARTS)
	make -C libs
	make -C plugins
	$(CC) $(CFLAGS) $(PARTS) $(LDFLAGS) -o $(PROJECT)

# For creating final Win32 package
win32: $(PARTS)
	make -C libs
	make -C launcher
	$(CC) $(CFLAGS) $(PARTS) $(LDFLAGS) -o $(PROJECT)
	cp -v $(PROJECT).exe /c/GZRT/resources/bin
	cp -v launcher/$(PROJECT).exe /c/GZRT/GZRT.exe
	cp -v plugins/*.dll /c/GZRT/plugins/
	cp -v libs/*.dll /c/GZRT/resources/bin/

clean:
	rm -vf `find . -name "*.o"` # Object files
	rm -vf `find . -name "*~"`  # Temporary files
