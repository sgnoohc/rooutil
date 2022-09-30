include Makefile.arch

#
# stuff to make
#
SOURCES=$(wildcard src/*.cc)
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

%.o: %.cxx
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $< -c -o $@

lib/librooutil.so: src/LinkDef.cxx src/LinkDef.o $(OBJECTS) $(SOURCES) $(HEADERS)
	$(MKDIR_P) lib/
	$(LD) $(LDFLAGS) -fPIC -ITMultiDrawTreePlayer -Wunused-variable $(SOFLAGS) $(OBJECTS) src/LinkDef.o $(ROOTLIBS) -lTMVA -lEG -lGenVector -lXMLIO -lMLP -lTreePlayer -o $@

src/LinkDef.cxx: src/LinkDef.h
	$(MKDIR_P) lib/
	rootcling -f src/LinkDef.cxx src/LinkDef.h
	cp src/LinkDef_rdict.pcm lib/

clean:
	rm -f src/LinkDef.cxx \
	rm -f src/LinkDef_rdict.pcm \
	rm -f src/*.o \
	rm -f src/*.d \
	rm -rf lib/ \

.PHONY: all
