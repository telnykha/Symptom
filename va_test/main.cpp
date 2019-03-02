#include "stdio.h"
#include "opencv\cv.h"
#include "opencv\highgui.h"

#define DISPLAY_WIDTH 800
#define DISPLAY_HEIHT 600

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
		cvShowImage("va_test", img);

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