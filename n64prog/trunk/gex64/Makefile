#######################################
# Zelda 64 Actor Spawner (OOT US 1.0) #
#######################################

# Name!
PROGRAM = gex64
PARTS   = crt0.o gex.o hook.o

# N64 tree variables
N64ROOT = /home/zzt32/MEDIA/Programming/n64/trunk/root
N64BIN  = ${N64ROOT}/bin

# Environment variables
CC      = mips64-gcc
LD      = mips64-ld
AS		= mips64-as
OBJCOPY = mips64-objcopy

# Flags
ASFLAGS = -mabi=32 -march=vr4300
LDFLAGS = -T $(PROGRAM).x
CFLAGS  = -fno-builtin -std=gnu99 -nodefaultlibs -march=vr4300 \
	  	  -mtune=vr4300 -I../include -I. -Os -mabi=32 -D_NO_MIPS_PREFIX

# Compile binary
all: $(PROGRAM).bin

$(PROGRAM).bin: $(PROGRAM).elf
	$(OBJCOPY) $(PROGRAM).elf $(PROGRAM).bin -O binary

$(PROGRAM).elf: $(PARTS)
	$(LD) -o $(PROGRAM).elf	$(PARTS) $(LDFLAGS) 
	
clean:
	rm *.o *.elf *~ *.bin final.txt codes.txt -vf
