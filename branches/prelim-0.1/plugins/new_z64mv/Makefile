################################
# Zelda 64 Model Viewer Plugin #
################################

# Plugin name
PROJECT = z64mv

# Target
ifeq ($(WIN32), 1)
	TARGET = $(PROJECT).dll
else
	TARGET = $(PROJECT).so
endif

# Environment
CC = gcc

# Utils flags
CFLAGS	= -I. -O3 -g -fPIC `sdl-config --cflags` `pkg-config --cflags glib-2.0`
LDFLAGS = -shared -W1,-soname,$(TARGET) `sdl-config --libs` `pkg-config --libs glib-2.0` -lm

# Lib destination
DEST	= ..

# Win32 options
ifeq ($(WIN32), 1)
	CFLAGS	+=	-mwindows -mms-bitfields
	LDFLAGS	+=	-lopengl32 -lglu32
else
	LDFLAGS += -lGL -lGLU
endif

# Application pieces
PIECES	= $(PROJECT).o dlist.o graphics.o rdp.o

# Make lib
$(DEST)/$(TARGET): $(PIECES)
	$(CC) -o $(TARGET) $(PIECES) $(LDFLAGS)
	cp -v $(TARGET) $(DEST)
	
# Cleanup
clean:
	rm -vf $(DEST)/$(TARGET) $(TARGET) *.o *.dll *.so *~
