#makefile for symptom software 

# The name of your C compiler:

CC= g++

# You may need to adjust these cc options:
CFLAGS= -O

# Link-time cc options:
LDFLAGS= 

#output library path 
LIB    = lib/
#awpipl2 library path 
AWPLIB = ../awpipl/lib/

#awplflib library path 
AWPLF = ../awplflib/lib/

#library source path 
COUNTER_SRCPATH= /counter/counter/ 
CROWD_SRCPATH= /crowd/crowd 
TRACK_SRCPATH= /track/track 

#include path 
INC= -Iinclude -Icommon -I../awpipl/include -I../awplflib/include 

# source files: symptom library proper
LIBSOURCES_UTILS   =  utils/vautils/vautils.cpp 
#LIBSOURCES_COUNTER =  counter/counter/counter.cpp counter/counter/lftcounter.cpp counter/counter/SDCounter.cpp 
LIBSOURCES_CROWD   =  crowd/crowd/crowd.cpp crowd/crowd/LFCrowdModule.cpp 
LIBSOURCES_TRACK   =  track/track/LFTrack.cpp 	track/track/track.cpp track/track/LFAwpImageBuffer.cpp track/track/LFFeaturesVector.cpp track/track/LFForegroundDetector.cpp
LIBSOURCES_FIRE   =   fire/fire/LFFireModule.cpp 	fire/fire/fire.cpp
LIBSOURCES_SMOKE   =  smoke/smoke/smoke.cpp 	smoke/smoke/LFSmokeModule.cpp
LIBSOURCES_PACKAGE   = package/package/package.cpp
LIBSOURCES_SABOTAGE   =  sabotage/sabotage/sabotage.cpp
LIBSOURCES_MOTION    =  motion/motion/motion.cpp
LIBSOURCES_FACE   =  face/face/face.cpp 	face/face/LFFaceModule.cpp
 

LIBOBJECTS_UTILS = vautils.o
#LIBOBJECTS_COUNTER = counter.o lftcounter.o SDCounter.o
LIBOBJECTS_CROWD = crowd.o LFCrowdModule.o 
LIBOBJECTS_TRACK = LFTrack.o track.o LFAwpImageBuffer.o LFFeaturesVector.o LFForegroundDetector.o
LIBOBJECTS_FIRE= LFFireModule.o fire.o 
LIBOBJECTS_SMOKE= smoke.o LFSmokeModule.o 
LIBOBJECTS_PACKAGE= package.o 
LIBOBJECTS_SABOTAGE= sabotage.o 
LIBOBJECTS_MOTION= motion.o 
LIBOBJECTS_FACE= face.o LFFaceModule.o 

all: symptom clean
symptom:
	$(CC)  -fPIC -O3 -c  $(INC) $(LIBSOURCES_SABOTAGE)
	#$(CC)  -fPIC -O3 -c  $(INC) $(LIBSOURCES_COUNTER)
	$(CC)  -fPIC -O3 -c  $(INC) $(LIBSOURCES_CROWD)
	$(CC)  -fPIC -O3  -c  $(INC) $(LIBSOURCES_TRACK)
	$(CC)  -fPIC -O3  -c  $(INC) $(LIBSOURCES_UTILS)
	$(CC)  -fPIC -O3  -c  $(INC) $(LIBSOURCES_FIRE)
	$(CC)  -fPIC -O3  -c  $(INC) $(LIBSOURCES_PACKAGE)
	$(CC)  -fPIC -O3  -c  $(INC) $(LIBSOURCES_SMOKE)
	$(CC)  -fPIC -O3  -c  $(INC) $(LIBSOURCES_MOTION)
	$(CC)  -fPIC -O3  -c  $(INC) $(LIBSOURCES_FACE)

	$(CC)  -shared -o lib/libsabotage.so $(LIBOBJECTS_UTILS) $(LIBOBJECTS_SABOTAGE) $(AWPLF)awplflib.a $(AWPLIB)awpipl2.a   -ljpeg -luuid -ltinyxml
	$(CC)  -shared -o lib/libsmoke.so $(LIBOBJECTS_UTILS) $(LIBOBJECTS_SMOKE)    $(AWPLF)awplflib.a  $(AWPLIB)awpipl2.a  -ljpeg -luuid -ltinyxml
	$(CC)  -shared -o lib/libfire.so    $(LIBOBJECTS_UTILS) $(LIBOBJECTS_FIRE)   $(AWPLF)awplflib.a $(AWPLIB)awpipl2.a -ljpeg -luuid -ltinyxml
	$(CC)  -shared -o lib/libpackage.so  $(LIBOBJECTS_UTILS) $(LIBOBJECTS_PACKAGE)  $(AWPLF)awplflib.a $(AWPLIB)awpipl2.a   -ljpeg -luuid -ltinyxml
	#$(CC)  -shared -o lib/libcounter.so    $(LIBOBJECTS_UTILS) $(LIBOBJECTS_COUNTER) $(AWPLF)awplflib.a  $(AWPLIB)awpipl2.a -ljpeg -luuid -ltinyxml
	$(CC)  -shared -o lib/libtrack.so    $(LIBOBJECTS_UTILS) $(LIBOBJECTS_TRACK)  $(AWPLF)awplflib.a $(AWPLIB)awpipl2.a -ljpeg -luuid -ltinyxml
	$(CC)  -shared -o lib/libcrowd.so    $(LIBOBJECTS_UTILS) $(LIBOBJECTS_CROWD) $(AWPLF)awplflib.a $(AWPLIB)awpipl2.a -ljpeg -luuid -ltinyxml
	$(CC)  -shared -o lib/libmotion.so   $(LIBOBJECTS_MOTION) $(AWPLIB)awpipl2.a -ljpeg 
	$(CC)  -c $(INC) va_test/main.cpp
	$(CC)  -shared -o lib/libface.so    $(LIBOBJECTS_UTILS) $(LIBOBJECTS_FACE) $(AWPLF)awplflib.a $(AWPLIB)awpipl2.a -ljpeg -luuid -ltinyxml

	$(CC)  main.o  -Ofast -L. $(LIB)libmotion.so $(LIB)libsabotage.so $(LIB)libsmoke.so  $(LIB)libfire.so $(LIB)libpackage.so   $ $(LIB)libtrack.so $(LIB)libcrowd.so $(LIB)libface.so `pkg-config --cflags --libs opencv` `sdl2-config --cflags --libs` -o symptom 

clean:
	rm -f *.o 


