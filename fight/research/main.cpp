#include "stdio.h"
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include "opencv2\video.hpp"
#include "opencv2\video\background_segm.hpp"
#include "_LF.h"
#include "..\fight\LFFightModule.h"
#include "fight.h"
#define DISPLAY_WIDTH 640	

extern "C"
{
	#pragma comment(lib, "opencv_world300.lib")

}

void Usage()
{
	printf("fight_research filename\n");
}

int DoReasearch(int argc, char** argv)
{
	if (argc < 2)
	{
		Usage();
		return 0;
	}

	CvCapture* capture = NULL;
	capture = cvCaptureFromFile(argv[1]);
	if (capture == NULL)
	{
		printf("Cannot open video source: %s\n", argv[1]);
		return 0;
	}

	IplImage* img = NULL;

	TVAInitParams params;
	params.EventSens = 0.75;
	params.EventTimeSens = 3;

	CFightEngine* engine = new CFightEngine(params);
	CFlowAnalysis* analysis = engine->GetAnalyser();
	CFBFlow* processor = engine->GetFlow();
	awpImage* awp = NULL;
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
		cvRectangle(img, CvPoint(0, 0), CvPoint(DISPLAY_WIDTH - 1, 50), CV_RGB(0, 0, 0), -1);
		if (awp == NULL)
			awpCreateImage(&awp, img->width, img->height, 3, AWP_BYTE);
		memcpy(awp->pPixels, img->imageData, img->widthStep*img->height);
		engine->SetSourceImage(awp, true);

		//if (processor.GetMoveCount() > 2)
		{
			CvPoint p1 = analysis->GetCvCentroid();
			p1.x *= processor->GetScale();
			p1.y *= processor->GetScale();
			CvScalar color;
			if (engine->GetState() > 0)
				color = CV_RGB(255, 0, 0);
			else
				color = CV_RGB(0, 255, 0);
			cvCircle(img, p1, 100, color, 2);
			cvShowImage("fight research mask", processor->GetMask());
		}
		cvShowImage("fight research flow", img);
		cvShowImage("fight research foreground", processor->GetFG());


		int c;
		c = cvWaitKey(10);
		if ((char)c == 27)
			break;
	}
	cvReleaseImage(&img);
	cvDestroyAllWindows();
	cvReleaseCapture(&capture);
	return 0;
}


int DoTest(int argc, char** argv)
{
	if (argc < 2)
	{
		Usage();
		return 0;
	}
	CvCapture* capture = NULL;
	capture = cvCaptureFromFile(argv[1]);
	if (capture == NULL)
	{
		printf("Cannot open video source: %s\n", argv[1]);
		return 0;
	}

	IplImage* img = NULL;

	TVAInitParams params;
	params.EventSens = 0.75;
	params.EventTimeSens = 3;

	HANDLE fight = fightCreate(&params);
	TVAResult result;
	result.blobs = new TVABlob[1];
	if (fight == NULL)
	{
		printf("cannot create fight!\n");
		return 1;
	}


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
		cvRectangle(img, CvPoint(0, 0), CvPoint(DISPLAY_WIDTH - 1, 50), CV_RGB(0, 0, 0), -1);
		
		HRESULT res = fightProcess(fight, img->width, img->height, 3, (unsigned char*)img->imageData, &result);

		if (res != S_OK)
		{
			printf("fight engine error!\n");
			break;
		}

		if (result.Num > 0)
		{
			cvRectangle(img, CvPoint(result.blobs[0].XPos, result.blobs[0].YPos), CvPoint(result.blobs[0].XPos + result.blobs[0].Width, 
				result.blobs[0].YPos + result.blobs[0].Height), CV_RGB(255, 0, 0), 3);
		}

		cvShowImage("fight research flow", img);

		int c;
		c = cvWaitKey(10);
		if ((char)c == 27)
			break;
	}
	cvReleaseImage(&img);
	cvDestroyAllWindows();
	cvReleaseCapture(&capture);
	fightRelease(&fight);
	return 0;

}

int main(int argc, char** argv)
{
	//return DoReasearch(argc, argv);
	return DoTest(argc, argv);
}