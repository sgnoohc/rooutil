include Makefile.arch

#
# stuff to make
#
SOURCES=$(wildcard *.cc)
OBJECTS=$(SOURCES:.cc=.o)
LIB=rooutil.so

TMULTISRCS=$(wildcard TMultiDrawTreePlayer/*.cxx)
TMULTIOBJS=$(TMULTISRCS:.cxx=.o)

#
# how to make it 
#
$(LIB): $(OBJECTS)  $(TMULTIOBJS)
	$(LD) $(LDFLAGS) $(SOFLAGS) $(OBJECTS) $(TMULTIOBJS) $(ROOTLIBS) -lTMVA -lEG -lGenVector -lXMLIO -lMLP -lTreePlayer -o $@

TMultiDrawTreePlayer/%.o: TMultiDrawTreePlayer/%.cxx
	$(CXX) -Wunused-variable $(CXXFLAGS) -ITMultiDrawTreePlayer -c $< -o $@

%.o:	%.cc
	$(CXX) -Wunused-variable $(CXXFLAGS) -c $< -o $@

#
# target to build
# likelihood id library
#

all: $(LIB) 
clean:
	rm -f *.o \
	rm -f *.d \
	rm -f *.so \
