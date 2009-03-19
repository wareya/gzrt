LIB = libsrch.a

PARTS = search.o compares.o

# Compilers
CC = $(N64_BIN_DIR)mips64-gcc
LD = $(N64_BIN_DIR)mips64-ld
AS = $(N64_BIN_DIR)mips64-as
AR = $(N64_BIN_DIR)mips64-ar

# Instruction set options
CPU_OPTS = -march=vr4300 -mabi=32

# Include options
INC_LOCAL  = -I .
INC_EXTERN = 

# Binary flags
ASFLAGS = $(CPU_OPTS) $(INC_LOCAL)
CFLAGS  = -std=gnu99 -G 0 -Os -mtune=vr4300 -Wall -pedantic $(CPU_OPTS) $(INC_LOCAL) $(INC_EXTERN)
LDFLAGS = 


$(LIB): $(PARTS)
	$(AR) -rcsv -o $(LIB) $(PARTS)
