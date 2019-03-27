#include "stdio.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "motion.h"
#include "sabotage.h"

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIHT 480

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
#define VA_MODULE_TRACK    14

extern "C"
{
	#pragma comment(lib, "opencv_world300.lib")
}

void Usage()
{
	printf("Usage: \n");
	printf("symptom -key -input [path]\n");
	printf("-key = \n");
	printf("--motion   	Motion detector\n");
	printf("--sabotage 	Sabotage detector\n");
	printf("--fire 		Fire detector\n");
	printf("--smoke 	Smoke detector\n");
	printf("--package	Leaving bags and packages detecor\n");
	printf("--track		BLOB tracking system\n");
	printf("--counter	BLOB counter system \n");
	printf("--crowd		Crowd objects counter\n");
	printf("--heatmap	Motion heatmap creator \n");
	printf("--face		Face detector\n");
	printf("--human		Human detector\n");
	printf("--car 		Car detector \n");
	printf("--trains	Train number detector\n");
	printf("--fight		Fight detector\n");
}
int GetModuleCode(char* key)
{
	if (strcmp(key, "--motion") == 0)
		return VA_MODULE_MOTION;
	else if (strcmp(key, "--sabotage") == 0)
		return VA_MODULE_SABOTAGE;
	else if (strcmp(key, "--fire") == 0)
		return VA_MODULE_FIRE;
	else if (strcmp(key, "--smoke") == 0)
		return VA_MODULE_SMOKE;
	else if (strcmp(key, "--package") == 0)
		return VA_MODULE_PACKAGE;
	else if (strcmp(key, "--track") == 0)
		return VA_MODULE_TRACK;
	else if (strcmp(key, "--counter") == 0)
		return VA_MODULE_COUNTER;
	else if (strcmp(key, "--crowd") == 0)
		return VA_MODULE_CROWD;
	else if (strcmp(key, "--heatmap") == 0)
		return VA_MODULE_HEATMAP;
	else if (strcmp(key, "--face") == 0)
		return VA_MODULE_FACE;
	else if (strcmp(key, "--human") == 0)
		return VA_MODULE_HUMAN;
	else if (strcmp(key, "--car") == 0)
		return VA_MODULE_CAR;
	else if (strcmp(key, "--trains") == 0)
		return VA_MODULE_TRAINS;
	else if (strcmp(key, "--fight") == 0)
		return VA_MODULE_FIGHT;
	else 
		return -1;
}

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
public:	
	CMotionModule(TVAInitParams* params) : IVideoAnalysis(params){}

	virtual void InitModule(TVAInitParams* params)
	{
		MotionDetectorCNT* m = InitMotionDetector();
		if (m == NULL)
		{
			printf("ERROR: cannot create module MOTION.\n");
			exit(-1);
		}
		m_module = (HANDLE)m;
	}
	virtual void ReleaseModule()
	{
		MotionDetectorCNT* m = (MotionDetectorCNT*)m_module;
		FreeMotionDetector(m);
	}

	virtual void ProcessData(unsigned char* data, int width, int height, int bpp)
	{
		MotionDetectorCNT* m = (MotionDetectorCNT*)m_module;
		AnalyzeMotionDetectorArgb(m, (char*)data, 3, width, height, 75, 2, 2);
	}
	virtual void DrawResult(unsigned char* data, int width, int height, int bpp)
	{
		MotionDetectorCNT* m = (MotionDetectorCNT*)m_module;
		IplImage* img = cvCreateImageHeader(CvSize(width, height), IPL_DEPTH_8U, 3); 
		img->imageData = (char*)data;
		
		int count = 0;
		_CvRect* r = GetMotionDetectorRects(m, count);
		printf("MOTION MODULE: count of moving objects = %i \n", count);

		// draw result 
		for (int i = 0; i < count; i++)
		{
			CvRect rr = cvRect(r[i].x, r[i].y, r[i].width, r[i].height);
			cvRectangle(img, CvPoint(rr.x, rr.y), CvPoint(rr.x + rr.width, rr.y + rr.height), CV_RGB(0, 255, 0), 1);
		}
		cvReleaseImageHeader(&img);
	}
};

class CSabotageModule : public IVideoAnalysis
{
public:
	CSabotageModule(TVAInitParams* params) : IVideoAnalysis(params){}
	
	virtual void InitModule(TVAInitParams* params)
	{
		m_module = (HANDLE)sabotageCreate(params);
		if (m_module == NULL)
		{
			printf("ERROR: cannot create module SABOTAGE.\n");
			exit(-1);
		}
	}
	
	virtual void ReleaseModule()
	{
		sabotageRelease (&m_module);
	}

	virtual void ProcessData(unsigned char* data, int width, int height, int bpp)
	{
		bool result = false;
		sabotageProcess(m_module,  width, height, bpp, data, &result);
	}

	virtual void DrawResult(unsigned char* data, int width, int height, int bpp)
	{
		int state = NO_SABOTAGE;
		sabotageState(m_module, &state);
		if (state != NO_SABOTAGE)
			printf("MODULE SABOTAGE: detect sabotage\n");
		else 
			printf("MODULE SABOTAGE: status ok\n");
	}
	

};

IVideoAnalysis* VideoAnalysisFactory(TVAInitParams* params, int VA_MODULE_ID)
{
	if (VA_MODULE_ID == VA_MODULE_MOTION)
		return new CMotionModule(params);
	else if (VA_MODULE_ID == VA_MODULE_SABOTAGE)
		return new CSabotageModule(params);
	else
		return NULL;
}

IVideoAnalysis* module = NULL;


int main(int argc, char** argv)
{
	if (argc < 3)
	{
		Usage();
		return 0;
	}
	
	int k = GetModuleCode(argv[1]);
	if (k < 0)
	{
		printf("ERROR: unknown module. \n");
		printf("---------------------- \n");
		Usage();
		return 0;
	}

	TVAInitParams params;
	params.EventSens 		= 0.5;
	params.EventTimeSens 	= 500;
	
	module = VideoAnalysisFactory(&params, k);
	if (module == NULL)
	{
		printf("ERROR: Cannot create video analysis: %s\n", argv[1]);
		Usage();
		return 0;
	}

	
	CvCapture* capture = NULL;
	capture = cvCaptureFromFile(argv[2]);
	if (capture == NULL)
	{
		printf("ERROR: Cannot open video source: %s\n", argv[2]);
		return 0;
	}

	IplImage* img = NULL;
	char msg[64];
	sprintf(msg, "symptom 3.0 %s", argv[1]);	
	
	module->InitModule(&params);
	while (true)
	{
		IplImage* frame = NULL;
		frame = cvQueryFrame(capture);
		if (!frame)
			break;
		if (img == NULL)
		{
			int height = frame->height * DISPLAY_WIDTH / frame->width;
			img = cvCreateImage(CvSize(DISPLAY_WIDTH, height), IPL_DEPTH_8U, 3);
		}
		cvResize(frame, img);

		module->ProcessData((unsigned char*)img->imageData,img->width, img->height, 3);
		module->DrawResult((unsigned char*)img->imageData, img->width, img->height, 3);
		cvShowImage(msg, img);

		int c;
		c = cvWaitKey(10);
		if ((char)c == 27)
			break;
	}
	cvReleaseImage(&img);
	cvDestroyAllWindows();
	cvReleaseCapture(&capture);
	module->ReleaseModule();
	return 0;
}
