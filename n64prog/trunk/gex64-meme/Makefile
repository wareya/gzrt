##############################
# Memory modifier for Gex 64 #
##############################

# Name!
PROGRAM = gex64
PARTS   = crt0.o hook.o time.o time64.o meme.o

# N64 tree variables
N64ROOT = /c/n64tree-win32/root/bin
N64BIN  = ${N64ROOT}/bin

# Environment variables
CC      = mips64-gcc
LD      = mips64-ld
AS	    = mips64-as
OBJCOPY = mips64-objcopy

# Flags
ASFLAGS = -mabi=32 -march=vr4300
LDFLAGS = -T $(PROGRAM).x -lc -lnosys
CFLAGS  = -fno-builtin -std=gnu99 -nodefaultlibs -Os -I../include -I. \
	  -D__MIPS_NO_PREFIX

# Conform with N64's ABI/CPU build flag
CFLAGS += -mabi=32 -mips2

# Compile binary
all: $(PROGRAM).bin

$(PROGRAM).bin: $(PROGRAM).elf
	$(OBJCOPY) $(PROGRAM).elf $(PROGRAM).bin -O binary

$(PROGRAM).elf: $(PARTS)
	$(LD) -o $(PROGRAM).elf	$(PARTS) $(LDFLAGS) 
	
clean:
	rm *.o *.elf *~ *.bin final.txt codes.txt -vf
