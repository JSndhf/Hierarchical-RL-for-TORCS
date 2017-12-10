CC            =  g++
CPPFLAGS      = -std=c++11 -Wall -g

#Put here the name of your driver class
DRIVER_CLASS = HRLDriver
#Put here the filename of your driver class header
DRIVER_INCLUDE = '"$(DRIVER_CLASS).h"'
DRIVER_OBJ = $(DRIVER_CLASS).o

EXTFLAGS = -D __DRIVER_CLASS__=$(DRIVER_CLASS) -D __DRIVER_INCLUDE__=$(DRIVER_INCLUDE)

OBJECTS = WrapperBaseDriver.o SimpleParser.o CarState.o CarControl.o DiscreteFeatures.o TreeNodes.o EnvControl.o pugixml.o DataHandler.o $(DRIVER_OBJ)

all: $(OBJECTS) client


.SUFFIXES : .o .cpp .c

.cpp.o :
	$(CC) $(CPPFLAGS) $(EXTFLAGS) -c $<

.c.o :
	$(CC) $(CPPFLAGS) $(EXTFLAGS) -c $<


client: client.cpp $(OBJECTS)
		$(CC) $(CPPFLAGS) $(EXTFLAGS) -o client client.cpp $(OBJECTS)

clean:
	rm -f *.o client
