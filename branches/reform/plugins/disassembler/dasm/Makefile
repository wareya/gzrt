###########################
# MIPS R4000 Disassembler #
###########################

# Program name 
PRGRM=r4kdis

# Apps
CC   = gcc		# GNU C compiler
LD   = ld		# Linker
AR   = ar		# Archiver

# Build flags
CFLAGS  = -std=gnu99 -O3 -fPIC $(INC)

# Application parts
PIECES = $(PRGRM).o opprintf.o

# Lib
lib$(PRGRM).a: $(PIECES)
	ar -rcsv -o lib$(PRGRM).a *.o
	
# Clean up junk
clean:
	rm -vf `find . -name "*.o"`   # Object files
	rm -vf `find . -name "*~"`    # Temporary files
	rm -vf `find . -name "*.a"`   # Libs
	rm -vf `find . -name "a.out"` # Junk
