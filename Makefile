include Makefile.arch

#
# stuff to make
#
SOURCES=$(wildcard src/*.cc) src/LinkDef.cc
OBJECTS=$(SOURCES:.cc=.o)
HEADERS=$(SOURCES:.cc=.h)
CFLAGS= $(ROOTCFLAGS) -Wall -Wno-unused-function -g -O2 -fPIC -fno-var-tracking -DLorentzVectorPtEtaPhiM4D

MKDIR_P = mkdir -p

#
# how to make it 
#

all: lib/librooutil.so

%.o: %.cc
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $< -c -o $@

lib/librooutil.so: $(OBJECTS) $(SOURCES) $(HEADERS)
	$(MKDIR_P) lib/
	$(LD) $(LDFLAGS) -fPIC -ITMultiDrawTreePlayer -Wunused-variable $(SOFLAGS) $(OBJECTS) $(ROOTLIBS) -lTMVA -lEG -lGenVector -lXMLIO -lMLP -lTreePlayer -o $@

src/LinkDef.cc: src/LinkDef.h
	$(MKDIR_P) lib/
	rootcling -f src/LinkDef.cc -rmf lib/librooutil.rootmap -rml lib/librooutil.so src/LinkDef.h
	cp src/LinkDef_rdict.pcm lib/

clean:
	rm -f src/LinkDef.cc \
	rm -f src/LinkDef_rdict.pcm \
	rm -f src/*.o \
	rm -f src/*.d \
	rm -rf lib/ \

.PHONY: all
