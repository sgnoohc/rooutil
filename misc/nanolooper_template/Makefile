# Simple makefile

EXE=doAnalysis

SOURCES=$(wildcard *.cc) $(wildcard NanoTools/NanoCORE/*.cc) $(wildcard NanoTools/NanoCORE/Tools/*.cc)
OBJECTS=$(SOURCES:.cc=.o)
HEADERS=$(SOURCES:.cc=.h)

CC          = g++
CXX         = g++
CXXFLAGS    = -g -O2 -Wall -fPIC -Wshadow -Woverloaded-virtual
LD          = g++
LDFLAGS     = -g -O2
SOFLAGS     = -g -shared
CXXFLAGS    = -g -O2 -Wall -fPIC -Wshadow -Woverloaded-virtual
LDFLAGS     = -g -O2
ROOTLIBS    = $(shell root-config --libs)
ROOTCFLAGS  = $(shell root-config --cflags)
CXXFLAGS   += $(ROOTCFLAGS)
CFLAGS      = $(ROOTCFLAGS) -Wall -Wno-unused-function -g -O2 -fPIC -fno-var-tracking -INanoTools/NanoCORE -Irooutil/ -DLorentzVectorPtEtaPhiM4D
EXTRACFLAGS = $(shell rooutil-config)
EXTRAFLAGS  = -fPIC -ITMultiDrawTreePlayer -Wunused-variable -lTMVA -lEG -lGenVector -lXMLIO -lMLP -lTreePlayer -lMinuit $(shell rooutil-config)
ROOUTILDIR  = rooutil

all: $(ROOUTILDIR) $(EXE)

$(EXE): $(OBJECTS)
	$(LD) $(CXXFLAGS) $(LDFLAGS) $(OBJECTS) $(ROOTLIBS) $(EXTRAFLAGS) -o $@

%.o: %.cc
	$(CC) $(CFLAGS) $(EXTRACFLAGS) $< -c -o $@

clean:
	rm -f $(OBJECTS) $(EXE)

cleanall: clean
	$(MAKE) -C $(ROOUTILDIR) clean

$(ROOUTILDIR):
	$(MAKE) -C $(ROOUTILDIR)

.PHONY: $(ROOUTILDIR)
