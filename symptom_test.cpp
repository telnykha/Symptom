#include <stdio.h>
#include "include/sabotage.h"
#include "include/smoke.h"
#include "include/fire.h"
#include "include/package.h"
#include "include/crowd.h"
#include "include/counter.h"
#include "include/track.h"
#include "include/motion.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"


#define VA_MODULE_SABOTAGE 1
#define VA_MODULE_FIRE	   2
#define VA_MODULE_SMOKE    3
#define VA_MODULE_PACKAGE  4
#define VA_MODULE_MOTION   5
#define VA_MODULE_COUNTER  6
#define VA_MODULE_CROWD    7
#define VA_MODULE_HEATMAP  8
#define VA_MODULE_CAR      9
#define VA_MODULE_FACE     10
#define VA_MODULE_HUMAN    11
#define VA_MODULE_TRAINS   12
#define VA_MODULE_FIGHT    13


MotionDetectorCNT* motion = NULL;

HANDLE sabotage = NULL;
HANDLE smoke    = NULL;
HANDLE fire     = NULL;
HANDLE package  = NULL;
HANDLE crowd    = NULL;
HANDLE counter  = NULL;
HANDLE track    = NULL;

TVAInitParams  params;
TVAPackageInit package_params;


#define _CHECK_MODULE_(module, name)\
	if (module == NULL)\
	{\
		printf("Cannot create module: %s\n", name);\
		result = VA_ERROR;\
		goto cleanup;\
	}\
	

class IVideoAnalysis
{
protected:
	HANDLE		  m_module;
	TVAInitParams m_params;

public:
	IVideoAnalysis(TVAInitParams* params)
	{
		m_module = NULL;
	}
	virtual ~IVideoAnalysis()
	{
	}
	
	virtual void InitModule(TVAInitParams* params) = 0;
	virtual void ReleaseModule() = 0;
	virtual void ProcessData(unsigned char* data, int width, int height, int bpp) = 0;
	virtual void DrawResult(unsigned char* data, int width, int height, int bpp) = 0;
};

class CMotionModule : public IVideoAnalysis
{
protected:
	
	virtual void InitModule(TVAInitParams* params)
	{
		MotionDetectorCNT* m = InitMotionDetector();
		m_module = (HANDLE)m;
	}
	virtual void ReleaseModule()
	{
		MotionDetectorCNT* m = (MotionDetectorCNT*)m_module;
		FreeMotionDetector(m);
	}
public:
	CMotionModule(TVAInitParams* params) : IVideoAnalysis(params){}
	
	virtual void ProcessData(unsigned char* data, int width, int height, int bpp)
	{
		MotionDetectorCNT* m = (MotionDetectorCNT*)m_module;
		AnalyzeMotionDetectorArgb(m, (char*)data, 3, width, height, 75, 2, 2);
	}
	virtual void DrawResult(unsigned char* data, int width, int height, int bpp)
	{
		MotionDetectorCNT* m = (MotionDetectorCNT*)m_module;
//		IplImage* img = cvCreateImageHeader(CvSize(width, height), IPL_DEPTH_8U, 3); 
//		img->imageData = (char*)data;
		
		int count = 0;
		_CvRect* r = GetMotionDetectorRects(m, count);
		printf("c = %i \n", count);

		// draw result 
		for (int i = 0; i < count; i++)
		{
//			CvRect rr = cvRect(r[i].x, r[i].y, r[i].width, r[i].height);
//			cvRectangle(img, CvPoint(rr.x, rr.y), CvPoint(rr.x + rr.width, rr.y + rr.height), CV_RGB(0, 255, 0), 1);
		}
//		cvReleaseImageHeader(&img);
	}
};

IVideoAnalysis* VideoAnalysisFactory(TVAInitParams* params, int VA_MODULE_ID)
{
	if (VA_MODULE_ID == VA_MODULE_MOTION)
		return new CMotionModule(params);
	else
		return NULL;
}

IVideoAnalysis* module;


int main(int argc, const char** argv)
{
	int result = VA_OK;
	
	motion  = InitMotionDetector();
	_CHECK_MODULE_(motion, "MOTION")
	
	sabotage = sabotageCreate(&params);
	_CHECK_MODULE_(sabotage, "SABOTAGE")
	
	smoke = smokeCreate(&params);
	_CHECK_MODULE_(smoke, "SMOKE")
	
	fire     = fireCreate(&params);
	_CHECK_MODULE_(fire, "FIRE")

	package  = packageCreate(&params, &package_params);
	_CHECK_MODULE_(package, "PACKAGE")

	crowd    = crowdCreate(&params);
	_CHECK_MODULE_(crowd, "CROWD")

	counter  = counterCreate(&params);
	_CHECK_MODULE_(counter, "COUNTER")

	track    = trackCreate(&params);
	_CHECK_MODULE_(track, "TRACK")

cleanup:
	if (motion != NULL)
		FreeMotionDetector(motion);
	if (sabotage != NULL)
		sabotageRelease(&sabotage);
	if (smoke != NULL)
		smokeRelease(&smoke);
	if (fire != NULL)
		fireRelease(&fire);
	if (package != NULL)
		packageRelease(&package);
	if (crowd != NULL)
		crowdRelease(&crowd);
	if (counter != NULL)
		counterRelease(&counter);
	if (track != NULL)
		trackRelease(&track);
	return result;
}
