##############################
# Zelda 64 ROM Tool Launcher #
##############################

PROJECT=gzrt

# Compiler options
CC=gcc

# Standard flags
CFLAGS += -DWIN32 -I. -Os -s --std=gnu99 -funsigned-char -mwindows -mms-bitfields

# Parts
PARTS=$(PROJECT).o

all: $(PARTS)
	echo '101 ICON DISCARDABLE "icon.ico"' > gzrt.rc
	windres gzrt.rc gzrt.obj
	$(CC) -o $(PROJECT) $(CFLAGS) $(PARTS) gzrt.obj
	rm gzrt.rc gzrt.obj
