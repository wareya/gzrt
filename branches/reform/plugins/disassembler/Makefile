#
# N64 Raw binary disassembler
#

PRGRM=disassembler

PIECES=$(PRGRM).o

CC=gcc

CFLAGS=-O3 -g `pkg-config --cflags gtk+-2.0` -Idasm -fPIC
LDFLAGS=-shared -W1,-soname,lib$(PRGRM).so `pkg-config --libs gtk+-2.0`

DEST=..

# Win32 options
ifeq ($(WIN32), 1)
	TARGET = ../$(PRGRM).dll
else
	TARGET = ../$(PRGRM).so
endif

$(TARGET): $(PIECES)
	make -C dasm
	$(CC) -o $(PRGRM).so $(PIECES) dasm/libr4kdis.a $(LDFLAGS)
	cp -v $(PRGRM).so $(TARGET)

clean:
	rm *.o $(PRGRM) *~ -vf
