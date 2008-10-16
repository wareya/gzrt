################################
# Zelda 64 Model Viewer Plugin #
################################

# Plugin name
PROJECT = z64v

# Target
ifeq ($(WIN32), 1)
    TARGET = $(PROJECT).dll
else
    TARGET = $(PROJECT).so
endif

# Environment
CC = gcc

# Lib settings 
LDFLAGS += -shared -W1,-soname,$(TARGET) 

# Utils flags
CFLAGS  += -O3 -I. -g `sdl-config --cflags` `pkg-config --cflags gtk+-2.0` -fPIC
LDFLAGS += -L. `pkg-config --libs gtk+-2.0` `sdl-config --libs` -lpthread

# Lib destination
DEST    = ..

# Win32 options
ifeq ($(WIN32), 1)
    CFLAGS  +=  -mwindows -mms-bitfields -DWIN32 
    LDFLAGS +=  -lmingw32 -lglu32 -lglew32 -lopengl32
else
    LDFLAGS += -lGL -lGLU -lGLEW
endif

# Application pieces
PIECES  = zglv-plugin.o dlist.o z64viewer.o

# Make lib
$(DEST)/$(TARGET): $(PIECES)
    $(CC) -o $(TARGET) $(PIECES) $(LDFLAGS)
    cp -v $(TARGET) $(DEST)

# Cleanup
clean:
    rm -vf $(DEST)/$(TARGET) $(TARGET) *.o 
