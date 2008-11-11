#
# N64 Raw binary disassembler
#

PRGRM=disassembler

PIECES=$(PRGRM).o dump.o

CC=gcc

CFLAGS=-O3 -g `pkg-config --cflags gtk+-2.0` -I../../libs/r4kdis -I. -fPIC
LDFLAGS=-shared -W1,-soname,lib$(PRGRM).so `pkg-config --libs gtk+-2.0` \
	-L../../libs -lr4kdis

DEST=..

# Win32 options
ifeq ($(WIN32), 1)
	TARGET = ../$(PRGRM).dll
else
	TARGET = ../$(PRGRM).so
endif

$(TARGET): $(PIECES)
	$(CC) -o $(PRGRM).so $(PIECES) $(LDFLAGS)
	cp -v $(PRGRM).so $(TARGET)

clean:
	rm *.o $(PRGRM) *~ -vf
