#makefile for symptom software 

# The name of your C compiler:

CC= cc

# You may need to adjust these cc options:
CFLAGS= -O

# Link-time cc options:
LDFLAGS= 

#output library path 
LIB    = lib/
#awpipl2 library path 
AWPLIB = ../awpipl/lib/

#library source path 
COUNTER_SRCPATH= /counter/counter/ 
CROWD_SRCPATH= /crowd/crowd 
TRACK_SRCPATH= /track/track 

#include path 
INC= -Iinclude -Icommon -I../awpipl/include -I../awplflib/include 

# source files: symptom library proper
LIBSOURCES_UTILS   =  utils/vautils/vautils.cpp 
LIBSOURCES_COUNTER =  counter/counter/counter.cpp counter/counter/lftcounter.cpp 
LIBSOURCES_CROWD   =  crowd/crowd/crowd.cpp crowd/crowd/LFCrowdModule.cpp 
LIBSOURCES_TRACK   =  track/track/LFTrack.cpp 	track/track/track.cpp

LIBOBJECTS= counter.o lftcounter.o 

all: symptom

symptom:
	$(CC)  -c $(INC) $(LIBSOURCES_COUNTER)
	$(CC)  -c $(INC) $(LIBSOURCES_CROWD)
	$(CC)  -c $(INC) $(LIBSOURCES_TRACK)
	$(CC)  -c $(INC) $(LIBSOURCES_UTILS)

