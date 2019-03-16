#ifndef __motion_h__ 
#define __motion_h__ 

#include "va_common.h"


#ifdef WIN32
#ifdef MOTION_EXPORTS
#define MOTION_API __declspec(dllexport)
#else
#define MOTION_API __declspec(dllimport)
#endif
#else
#define MOTION_API 
#endif

// forward declaration
class MotionDetectorCNT;
struct CvRect
{
	int x;
	int y;
	int width;
	int height;
};

extern "C" 
{	
	MOTION_API MotionDetectorCNT*  InitMotionDetector();
	MOTION_API void  AnalyzeMotionDetectorArgb(MotionDetectorCNT* detector, char* bmpImage, int bmpStride, int bmpWidth, int bmpHeight, int sensitivity, int minObjWidthPercent, int minObjHeightPercent);
	MOTION_API CvRect*  GetMotionDetectorRects(MotionDetectorCNT* detector, int& rectCount);
	MOTION_API void  FreeMotionDetector(MotionDetectorCNT* detector);
}
#endif //__motion_h__ 
