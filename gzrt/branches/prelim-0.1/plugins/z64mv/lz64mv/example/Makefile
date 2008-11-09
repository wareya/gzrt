PROJECT=zmvtest

PIECES=$(PROJECT).o

CC=gcc
LD=ld
AR=ar

LDFLAGS=-L.. `sdl-config --libs` -lGL -lGLU -lz64mv
CFLAGS=-g `sdl-config --cflags` -I.. -O3 -std=gnu99

$(PROJECT): $(PIECES)
	$(CC) -o $(PROJECT) $(PIECES) $(LDFLAGS)

clean:
	rm -vf lib$(PROJECT).a
	rm -vf `find . -name "*.o"` # Object files
	rm -vf `find . -name "*~"`  # Temporary files

