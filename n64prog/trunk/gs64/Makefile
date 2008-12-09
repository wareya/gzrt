####################################
# Gameshark hack for OoT v1.0 (US) #
####################################

# Basics
PROGRAM = gs64
PARTS   = crt0.o $(PROGRAM).o code.o dma.o hook.o

# N64 tree variables
N64ROOT = ../root
N64BIN  = $(N64ROOT)/bin

# Environment variables
CC      = $(N64BIN)/mips64-gcc
LD      = $(N64BIN)/mips64-ld
OBJCOPY = $(N64BIN)/mips64-objcopy
CHEAT   = $(N64BIN)/nemucheat
BINCODE = $(N64BIN)/bin2code

# newsys
NSLIB	= $(N64ROOT)/mips64/lib
NSINC	= $(N64ROOT)/mips64/include

# Flags
ASFLAGS = -march=vr4300 -mabi=32
LDFLAGS = -T $(PROGRAM).x -L . -L $(NSLIB) -lc -lnosys
CFLAGS  = -fno-builtin -std=gnu99 -nodefaultlibs -march=vr4300 -mtune=vr4300 \
	  -I ../include -I $(NSINC) -Os -mabi=32

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
