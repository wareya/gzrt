################################
# Zelda 64 Model Viewer Plugin #
################################

# Plugin name
PROJECT = nemu

# Target
ifeq ($(WIN32), 1)
	TARGET = $(PROJECT)
else
	TARGET = $(PROJECT)
endif

# Environment
CC = gcc

# Lib settings 
LDFLAGS += 

# Utils flags
CFLAGS  += -O3 -I. -g `pkg-config --cflags gtk+-2.0` -fPIC
LDFLAGS += -L. `pkg-config --libs gtk+-2.0`

# Lib destination
DEST	= ..

# Application pieces
PIECES	= nemu.o

# Make lib
$(TARGET): $(PIECES)
	$(CC) -o $(TARGET) $(PIECES) $(LDFLAGS)

# Cleanup
clean:
	rm -vf $(DEST)/$(TARGET) $(TARGET) *.o 
