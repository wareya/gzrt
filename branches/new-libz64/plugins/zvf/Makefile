################################
# Zelda 64 Model Viewer Plugin #
################################

# Plugin name
PROJECT = zvf

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
CFLAGS  += -O3 -I. -g -fPIC
LDFLAGS += -L. `pkg-config --libs gtk+-2.0` `sdl-config --libs` -lpthread

# Lib destination
DEST	= ..

# Win32 options
ifeq ($(WIN32), 1)
	CFLAGS	+=	-mwindows -mms-bitfields -DWIN32 
endif

# Application pieces
PIECES	= plugin.o

# Make lib
$(DEST)/$(TARGET): $(PIECES)
	$(CC) -o $(TARGET) $(PIECES) $(LDFLAGS)
	cp -v $(TARGET) $(DEST)
	
# Cleanup
clean:
	rm -vf $(DEST)/$(TARGET) $(TARGET) *.o 
