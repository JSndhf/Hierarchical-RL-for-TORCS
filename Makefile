CC            =  g++
CPPFLAGS      = -std=c++11 -Wall -g

DRIVER_CLASS = HRLDriver
DRIVER_INCLUDE = '"$(DRIVER_CLASS).h"'
DRIVER_OBJ = $(DRIVER_CLASS).o

EXTFLAGS = -D __DRIVER_CLASS__=$(DRIVER_CLASS) -D __DRIVER_INCLUDE__=$(DRIVER_INCLUDE)
# Experiment setup
# Defaults
episodes = 25000
alpha = 0.8
alphaDecay = 0.000028
epsilon = 0.1
epsilonDecay = 0.0000004
abstractReward = -1000.0
staticRoot = 1
staticSpeed = 1
staticGear = 1
staticSteer = 0
pseudoRew = 0

EXTFLAGS += -D __HRL_EPISODES__=$(episodes)
EXTFLAGS += -D __HRL_ALPHA__=$(alpha) -D __HRL_ALPHA_DECAY__=$(alphaDecay)
EXTFLAGS += -D __HRL_EPSILON__=$(epsilon)  -D __HRL_EPSILON_DECAY__=$(epsilonDecay)
EXTFLAGS += -D __HRL_ABSTRACT_REWARD__=$(abstractReward)
ifeq ($(staticRoot), 1)
EXTFLAGS += -D __HRL_STATIC_ROOT__
endif
ifeq ($(staticSpeed), 1)
EXTFLAGS += -D __HRL_STATIC_SPEED__
endif
ifeq ($(staticGear), 1)
EXTFLAGS += -D __HRL_STATIC_GEAR__
endif
ifeq ($(staticSteer), 1)
EXTFLAGS += -D __HRL_STATIC_STEER__
endif
ifeq ($(pseudoRew), 1)
EXTFLAGS += -D __HRL_PR_ENABLED__
endif

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
