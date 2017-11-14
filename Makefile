include Makefile.arch

#
# stuff to make
#
SOURCES=$(wildcard *.cc)
OBJECTS=$(SOURCES:.cc=.o)
LIB=rooutil.so

#
# how to make it 
#

$(LIB): $(SOURCES)
	$(LD) $(CXXFLAGS) $(LDFLAGS) -fPIC -ITMultiDrawTreePlayer -Wunused-variable $(SOFLAGS) $^ $(ROOTLIBS) -lTMVA -lEG -lGenVector -lXMLIO -lMLP -lTreePlayer -o $@

#dictionary.cc:
#	cd TMultiDrawTreePlayer; \
#	rootcint -f dictionary.cc -c -p classes.h LinkDef.h; \
#	mv dictionary.cc ../

all: $(LIB) 
clean:
	rm -f TMultiDrawTreePlayer/dictionary* \
	rm -f dictionary.cc \
	rm -f *.o \
	rm -f *.d \
	rm -f *.so \
