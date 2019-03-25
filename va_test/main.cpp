#include "stdio.h"
#include "opencv\cv.h"
#include "opencv\highgui.h"

#include "motion.h"

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIHT 480


MotionDetectorCNT* motion = NULL;

extern "C"
{
	#pragma comment(lib, "opencv_world300.lib")
}

void Usage()
{
	printf("va_test -key -input [path]\n");
}

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

	motion = InitMotionDetector();
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
		AnalyzeMotionDetectorArgb(motion, img->imageData, 3, img->width, img->height, 75, 2, 2);
		int count = 0;
		_CvRect* r = GetMotionDetectorRects(motion, count);
		printf("c = %i \n", count);

		// draw result 
		for (int i = 0; i < count; i++)
		{
			CvRect rr = cvRect (r[i].x, r[i].y, r[i].width, r[i].height);
			cvRectangle(img, CvPoint(rr.x, rr.y), CvPoint(rr.x + rr.width, rr.y + rr.height), CV_RGB(0, 255, 0), 1);
		}

		cvShowImage("va_test", img);

		int c;
		c = cvWaitKey(10);
		if ((char)c == 27)
			break;
	}
	cvReleaseImage(&img);
	cvDestroyAllWindows();
	cvReleaseCapture(&capture);
	FreeMotionDetector(motion);
	return 0;
}