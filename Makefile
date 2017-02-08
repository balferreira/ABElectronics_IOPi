
### Build tools and build flags for all targets

CC = gcc
CFLAGS = -fPIC
LDFLAGS = -shared
LIB_NAME = libABEIOPI.so

INCLUDES = -I ./
SOURCES = ABEIOPI.c

OBJECTS	= $(SOURCES:.c=.o)

all: 
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SOURCES)
	ld $(LDFLAGS) -o $(LIB_NAME) $(OBJECTS)
	@echo ''
	@echo 'Dont forget to execute make install...'

install:
	mkdir -p /usr/include/ha/libs/
	cp $(LIB_NAME) /usr/lib/
	cp ABEIOPI.h /usr/include/ha/libs/

clean : 
	rm -f $(OBJECTS) $(LIB_NAME)

