#
# N64 Raw binary disassembler
#

PRGRM=z64mv

# Compiler
CC=gcc

# Application pieces
PIECES=$(PRGRM).o

# C compiler flags
CFLAGS=-O3 -g `sdl-config --cflags` -Ilz64mv -fPIC 

# Linker flags
LDFLAGS = `sdl-config --libs` -shared -W1,-soname,lib$(PRGRM).so

# Lib destination
DEST=..

# Win32 options
ifeq ($(WIN32), 1)
	CFLAGS += -DWIN32 -mwindows -mms-bitfields
	LDFLAGS += -lopengl32 -lglu32
	TARGET = ../$(PRGRM).dll
else
	LDFLAGS += -lGL -lGLU
	TARGET = $(PRGRM).so
endif

$(TARGET): $(PIECES)
	make -C lz64mv
	$(CC) -o lib$(PRGRM).so $(PIECES) lz64mv/libz64mv.a $(LDFLAGS)
	cp -v lib$(PRGRM).so $(TARGET)
	rm -v lib$(PRGRM).so

clean:
	rm *.o *.so *~ -vf
