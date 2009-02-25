###############################
# Zelda Text Print (MQ Debug) #
###############################

# Name!
PROGRAM = z64t
PARTS   = $(PROGRAM).o

# N64 tree variables
N64ROOT = /home/spinout/n64/root
N64BIN  = ${N64ROOT}/bin

# Environment variables
CC      = $(N64BIN)/mips64-gcc
LD      = $(N64BIN)/mips64-ld
OBJCOPY = $(N64BIN)/mips64-objcopy
CHEAT   = $(N64BIN)/nemucheat
BINCODE = $(N64BIN)/bin2code

# Flags
LDFLAGS = -T z64t.x -L .
CFLAGS  = -fno-builtin -std=gnu99 -nodefaultlibs -march=vr4300 -mtune=vr4300 \
		  -I../include -I. $(NOBUILTIN) -Os -mabi=32 -mno-gpopt

# Compile binary
all: $(PROGRAM).bin
	$(BINCODE) $(PROGRAM).bin 80600000 codes.txt # Turn binary into GS code
	cat hook codes.txt | $(CHEAT) "Z64 Text" 1 > final.txt

$(PROGRAM).bin: $(PROGRAM).elf
	$(OBJCOPY) $(PROGRAM).elf $(PROGRAM).bin -O binary

$(PROGRAM).elf: $(PARTS)
	$(LD) -o $(PROGRAM).elf	$(PARTS) $(LDFLAGS) 
	
clean:
	rm *.o *.elf *~ *.bin final.txt codes.txt -vf
