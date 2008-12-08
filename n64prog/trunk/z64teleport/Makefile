#
# Standard N64 Makefile
#

# Name!
PRJNAME = z64teleport

# Vars
INC_FLAGS = -I. -I../include
LIB_FLAGS = -L. -T z64teleport.x
ROOT = ..
N64ROOT = $(ROOT)/root
N64PREFIX = mips64-
N64BIN = ${N64ROOT}/bin

# Flags
NOBUILTIN=-fno-builtin
CFLAGS=-std=gnu99 -nodefaultlibs -mtune=vr4300 -mabi=32 ${INC_FLAGS}  $(NOBUILTIN) -O3
LINK_FLAGS=${LIB_FLAGS} ${LIBS}

# Programs
CC      = mips64-gcc
LD      = mips64-ld
OBJCOPY = mips64-objcopy
CHEAT   = nemucheat
BINCODE = bin2code

# Awwright
all: $(PRJNAME).bin
	$(BINCODE) $(PRJNAME).bin 80400000 codes.txt # Turn binary into GS code
	cat hook codes.txt | $(CHEAT) "Zelda 64 Teleport" 1 > final.txt # Turn into Nemu cheat

$(PRJNAME).bin: $(PRJNAME).elf
	$(OBJCOPY) $(PRJNAME).elf $(PRJNAME).bin -O binary

$(PRJNAME).elf: $(PRJNAME).o $(PIECES)
	$(LD) -o $(PRJNAME).elf	$(PRJNAME).o $(LINK_FLAGS) 
	
clean:
	rm *.o *.elf *~ *.bin final.txt codes.txt -vf
