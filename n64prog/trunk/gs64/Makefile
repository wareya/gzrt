####################################
# Gameshark hack for OoT v1.0 (US) #
####################################

# Basics
PROGRAM = gs64
PARTS   = crt0.o $(PROGRAM).o code.o dma.o

# N64 tree variables
N64ROOT = ../root
N64BIN  = 

# Environment variables
CC      = mips64-gcc
LD      = mips64-ld
OBJCOPY = mips64-objcopy
CHEAT   = nemucheat
BINCODE = bin2code

# Flags
ASFLAGS = -march=vr4300 -mabi=32
LDFLAGS = -T $(PROGRAM).x -L .
CFLAGS  = -fno-builtin -std=gnu99 -nodefaultlibs -march=vr4300 -mtune=vr4300 \
		  -I../include -Os -mabi=32

# Compile binary
all: $(PROGRAM).bin

# Compile util
util: 
	gcc -o patcher patcher.c -g -O0

$(PROGRAM).bin: $(PROGRAM).elf
	$(OBJCOPY) $(PROGRAM).elf $(PROGRAM).bin -O binary

$(PROGRAM).elf: $(PARTS)
	$(LD) -o $(PROGRAM).elf	$(PARTS) $(LDFLAGS) 
	
clean:
	rm *.o *.elf *~ *.bin final.txt codes.txt -vf
