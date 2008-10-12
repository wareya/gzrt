PROJECT=z64mv

PIECES=$(PROJECT).o dlist.o graphics.o

CC=gcc
LD=ld
AR=ar

LDFLAGS=`sdl-config --libs` -lGL -lGLU
CFLAGS=-g `sdl-config --cflags` -I. -O3 -std=gnu99 -fPIC

lib$(PROJECT).a: $(PIECES)
	$(AR) -rcs -o lib$(PROJECT).a $(PIECES) 

clean:
	rm -vf lib$(PROJECT).a
	rm -vf `find . -name "*.o"` # Object files
	rm -vf `find . -name "*~"`  # Temporary files

