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
LIBSOURCES_FIRE   =   fire/fire/LFFireModule.cpp 	fire/fire/fire.cpp
LIBSOURCES_SMOKE   =  smoke/smoke/smoke.cpp 	smoke/smoke/LFSmokeModule.cpp
LIBSOURCES_PACKAGE   = package/package/package.cpp
LIBSOURCES_SABOTAGE   =  sabotage/sabotage/sabotage.cpp 

LIBOBJECTS= counter.o lftcounter.o 

all: symptom

symptom:
	$(CC)  -c $(INC) $(LIBSOURCES_COUNTER)
	$(CC)  -c $(INC) $(LIBSOURCES_CROWD)
	$(CC)  -c $(INC) $(LIBSOURCES_TRACK)
	$(CC)  -c $(INC) $(LIBSOURCES_UTILS)
	$(CC)  -c $(INC) $(LIBSOURCES_FIRE)
	$(CC)  -c $(INC) $(LIBSOURCES_PACKAGE)
	$(CC)  -c $(INC) $(LIBSOURCES_SABOTAGE)
	$(CC)  -c $(INC) $(LIBSOURCES_SMOKE)
	rm -f *.o *.awp 

clean:
	rm -f *.o 


