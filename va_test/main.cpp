#include "stdio.h"
#include "opencv\cv.h"
#include "opencv\highgui.h"

#include "motion.h"

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIHT 480

MotionDetectorCNT* motion = NULL;


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

extern "C"
{
	#pragma comment(lib, "opencv_world300.lib")
}

void Usage()
{
	printf("va_test -key -input [path]\n");
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
		IplImage* img = cvCreateImageHeader(CvSize(width, height), IPL_DEPTH_8U, 3); 
		img->imageData = (char*)data;
		
		int count = 0;
		_CvRect* r = GetMotionDetectorRects(m, count);
		printf("c = %i \n", count);

		// draw result 
		for (int i = 0; i < count; i++)
		{
			CvRect rr = cvRect(r[i].x, r[i].y, r[i].width, r[i].height);
			cvRectangle(img, CvPoint(rr.x, rr.y), CvPoint(rr.x + rr.width, rr.y + rr.height), CV_RGB(0, 255, 0), 1);
		}
		cvReleaseImageHeader(&img);
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


int main(int argc, char** argv)
{
	if (argc < 3)
	{
		Usage();
		return 0;
	}
	
	CvCapture* capture = NULL;
	capture = cvCaptureFromFile(argv[2]);
	if (capture == NULL)
	{
		printf("Cannot open video source: %s\n", argv[2]);
		return 0;
	}

	IplImage* img = NULL;

	//motion = InitMotionDetector();
	module = VideoAnalysisFactory(NULL, VA_MODULE_MOTION);
	if (module == NULL)
	{
		printf("Cannot create video analysis: %s\n", argv[2]);
		return 0;
	}
	module->InitModule(NULL);
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

		// process image 
		//AnalyzeMotionDetectorArgb(motion, img->imageData, 3, img->width, img->height, 75, 2, 2);
		module->ProcessData((unsigned char*)img->imageData,img->width, img->height, 3);
		module->DrawResult((unsigned char*)img->imageData, img->width, img->height, 3);
		//int count = 0;
		//_CvRect* r = GetMotionDetectorRects(motion, count);
		//printf("c = %i \n", count);

		//// draw result 
		//for (int i = 0; i < count; i++)
		//{
		//	CvRect rr = cvRect (r[i].x, r[i].y, r[i].width, r[i].height);
		//	cvRectangle(img, CvPoint(rr.x, rr.y), CvPoint(rr.x + rr.width, rr.y + rr.height), CV_RGB(0, 255, 0), 1);
		//}

		cvShowImage("va_test", img);

		int c;
		c = cvWaitKey(10);
		if ((char)c == 27)
			break;
	}
	cvReleaseImage(&img);
	cvDestroyAllWindows();
	cvReleaseCapture(&capture);
	//FreeMotionDetector(motion);
	module->ReleaseModule();
	return 0;
}