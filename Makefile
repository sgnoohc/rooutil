include Makefile.arch

#
# stuff to make
#
SOURCES=$(wildcard src/*.cc)
OBJECTS=$(SOURCES:.cc=.o)
HEADERS=$(SOURCES:.cc=.h)
LIB=lib/rooutil.so
CFLAGS= $(ROOTCFLAGS) -Wall -Wno-unused-function -g -O2 -fPIC -fno-var-tracking

MKDIR_P = mkdir -p

#
# how to make it 
#

all: directories $(LIB) 

%.o: %.cc
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $< -c -o $@

$(LIB): $(OBJECTS) $(SOURCES) $(HEADERS)
	$(LD) $(LDFLAGS) -fPIC -ITMultiDrawTreePlayer -Wunused-variable $(SOFLAGS) $(OBJECTS) $(ROOTLIBS) -lTMVA -lEG -lGenVector -lXMLIO -lMLP -lTreePlayer -o $@
	ln -sf rooutil.so lib/librooutil.so

directories:
	$(MKDIR_P) lib/

clean:
	rm -f src/*.o \
	rm -f src/*.d \
	rm -rf lib/ \

.PHONY: directories
