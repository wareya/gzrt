#############################
# Zelda 64 Function Library #
#############################

PROJECT = z64

# Parts
PIECES = z64.o z64at.o z64nt.o z64fs.o z64ot.o z64st.o z64yaz0.o z64detect.o

# Destination
TARGET = lib$(PROJECT).a

# Preferred applications
AR = ar
CC = gcc

# Compiler settings
CFLAGS  = $(shell pkg-config --cflags glib-2.0) --std=gnu99 -g -fPIC -I. \
	      -funsigned-char -I../n64rom
	
$(TARGET): $(PIECES)
	$(AR) -rcs -o $(TARGET) $(PIECES)

install: $(TARGET)
	cp -f $(TARGET) ..
	cp -f *.h ../inc

clean:
	rm -fv $(TARGET) $(PIECES)
