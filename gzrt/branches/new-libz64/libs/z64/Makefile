#############################
# Zelda 64 Function Library #
#############################

PROJECT = z64

# Parts
PIECES = z64.o z64at.o z64nt.o z64fs.o z64ot.o z64st.o z64yaz0.o z64detect.o

# Win32 stuff
ifeq ($(WIN32), 1)
	TARGET = lib$(PROJECT).dll
else
	TARGET = lib$(PROJECT).so
endif

# Preferred applications
CC = gcc

# Compiler settings
LDFLAGS = -shared -W1,-soname,$(TARGET) `pkg-config --libs glib-2.0` \
	-L.. -ln64rom
CFLAGS  = `pkg-config --cflags glib-2.0` --std=gnu99 -g -fPIC -I. \
	-funsigned-char -I../n64rom -Wall

# Win32 again
ifeq ($(WIN32) ,1)
	CFLAGS += -mwindows -mms-bitfields
endif
	
$(TARGET): $(PIECES)
	$(CC) -o $(TARGET) $(PIECES) $(LDFLAGS)
	cp $(TARGET) /mingw/bin -v

../$(TARGET): $(TARGET)
	cp $(TARGET) .. -v

install: ../$(TARGET)

clean:
	rm -fv $(TARGET) *.o *~
