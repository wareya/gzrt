#
# N64 Raw binary disassembler
#

PRGRM=disassembler

PIECES=$(PRGRM).o

CC=gcc

CFLAGS=-O3 -g `pkg-config --cflags gtk+-2.0` -Idasm -fPIC
LDFLAGS=-shared -W1,-soname,lib$(PRGRM).so

DEST=..

lib$(PRGRM).so: $(PIECES)
	make -C dasm
	$(CC) -o lib$(PRGRM).so $(PIECES) dasm/libr4kdis.a $(LDFLAGS)
	cp -v lib$(PRGRM).so $(DEST)

clean:
	rm *.o $(PRGRM) *~ -vf
