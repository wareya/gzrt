#
# N64 Raw binary disassembler
#

PRGRM=z64mv

PIECES=$(PRGRM).o

CC=gcc

CFLAGS=-O3 -g `sdl-config --cflags` -Ilz64mv -fPIC 
LDFLAGS=`sdl-config --libs` -lGL -lGLU -shared -W1,-soname,lib$(PRGRM).so

DEST=..

lib$(PRGRM).so: $(PIECES)
	make -C lz64mv
	$(CC) $(LDFLAGS) -o lib$(PRGRM).so $(PIECES) lz64mv/libz64mv.a
	cp -v lib$(PRGRM).so $(DEST)

clean:
	rm *.o *.so *~ -vf
