#include "stdio.h"
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include "_LF.h"
#include "LFIplFBFlow.h"
#include "LFBorderLine.h"
#include "LFSensorLine.h"
#include "LFSTProcessor.h"

extern "C"
{
	#pragma comment(lib, "opencv_world300.lib")
}

#define DISPLAY_WIDTH 640	
const char _MODULE_[] = "research flow";
bool down = false;
CvFont g_font;


CFBFlow flow;
CSensorLine sensor;
CBorderLine border;
CLFSTProcessor processor(256, 256);
IplImage* LineData = NULL;

int usage()
{
	printf("FB02.exe file_name\n");
	return -1;
}

void on_mouse(int Event, int x, int y, int flags, void* param)
{
	CBorderLine* b = (CBorderLine*)param;
	if (Event == CV_EVENT_LBUTTONDOWN)
	{
		down = true;
		b->SetSp(cvPoint(x, y));
		b->SetEp(cvPoint(x, y));
		sensor.Init(flow.GetModule(), &border);
	}
	if (Event == CV_EVENT_MOUSEMOVE && down)
	{
		b->SetEp(cvPoint(x, y));
		sensor.Init(flow.GetModule(), &border);
	}
	if (Event == CV_EVENT_LBUTTONUP && down)
		down = false;
}

void DrawStatus(IplImage* img, CBorderLine* line)
{
	cvRectangle(img, cvPoint(0, 0), cvPoint(img->width, 30), CV_RGB(0, 0, 0), -1);
	
	double l = line->Lenght();
	bool status = l > 100;
	char buf[100];
	sprintf(buf, "length = %lf angle = %lf status = %i", l, line->Angle(), status);
	cvPutText(img, buf, cvPoint(10,20), &g_font, CV_RGB(128,128,128));
}

void DrawLineData(IplImage* img)
{
	if (img == NULL)
		return;
	if (sensor.DataLength() == 0)
		return;
	double* d = sensor.Data();
	cvZero(img);
	double dmax = 0;
	for (int i = 0; i < 255; i++)
	{
		double v = fabs(d[i]);
		if (dmax < v)
			dmax = v;
		CvPoint p1;
		CvPoint p2;

		p1.x = i;
		p1.y = img->height / 4 - 10*d[i];

		p2.x = i+1;
		p2.y = img->height / 4 - 10*d[i + 1];
		cvLine(img, p1, p2, CV_RGB(0, 255, 0));
	}

	char buf[100];
	sprintf(buf, "max = %2.2lf ", dmax);
	cvPutText(img, buf, cvPoint(10, 20), &g_font, CV_RGB(128, 128, 128));

	awpImage* vst = processor.GetVST();
	if (vst != NULL)
	{
		AWPBYTE* b = (AWPBYTE*)vst->pPixels;
		unsigned char* bb = (unsigned char*)img->imageData;
		for (int i = 0; i < 256; i++)
		{
			for (int j = 0; j < 256; j++)
			{
				bb[i*img->widthStep + 256 * 3 + 3 * j] = b[3 * i*vst->sSizeX + 3 * j];
				bb[i*img->widthStep + 256 * 3 + 3 * j + 1] = b[3 * i*vst->sSizeX + 3 * j + 1];
				bb[i*img->widthStep + 256 * 3 + 3 * j + 2] = b[3 * i*vst->sSizeX + 3 * j + 2];
			}
		}
		
		awpReleaseImage(&vst);
	}
	vst = processor.GetBSTIn();
	if (vst != NULL)
	{
		AWPBYTE* b = (AWPBYTE*)vst->pPixels;
		unsigned char* bb = (unsigned char*)img->imageData;
		for (int i = 0; i < 256; i++)
		{
			for (int j = 0; j < 256; j++)
			{
				bb[(i + 256)*img->widthStep + 256 * 3 + 3 * j] = b[i*vst->sSizeX + j];
				bb[(i + 256)*img->widthStep + 256 * 3 + 3 * j + 1] = 0;// b[i*vst->sSizeX + j];
				bb[(i + 256)*img->widthStep + 256 * 3 + 3 * j + 2] = 0;// b[i*vst->sSizeX + j];
			}
		}

		awpReleaseImage(&vst);
	}

	vst = processor.GetBSTOut();
	if (vst != NULL)
	{
		AWPBYTE* b = (AWPBYTE*)vst->pPixels;
		unsigned char* bb = (unsigned char*)img->imageData;
		for (int i = 0; i < 256; i++)
		{
			for (int j = 0; j < 256; j++)
			{
				bb[(i + 256)*img->widthStep + 3 * j + 2] = b[i*vst->sSizeX + j];
			}
		}

		awpReleaseImage(&vst);
	}
}

void PrintFileProps(CvCapture* c)
{
	if (c == NULL)
	{
		printf("video file is not open.\n");
		return;
	}

	double value;
	value = cvGetCaptureProperty(c, CV_CAP_PROP_FRAME_WIDTH);
	printf("Frame width = %lf\n", value);
	value = cvGetCaptureProperty(c, CV_CAP_PROP_FRAME_HEIGHT);
	printf("Frame height = %lf\n", value);
	value = cvGetCaptureProperty(c, CV_CAP_PROP_FPS);
	printf("FPS = %lf\n", value);
	value = cvGetCaptureProperty(c, CV_CAP_PROP_FOURCC);
	printf("FOURCC = %lf\n", value);
	value = cvGetCaptureProperty(c, CV_CAP_PROP_FRAME_COUNT);
	printf("Frame count = %lf\n", value);
}

int process(char* file_name)
{
	CvCapture* capture = NULL;
	bool paused = false;
	capture = cvCaptureFromFile(file_name);

	if (capture == NULL)
	{
		printf("Cannot open video source: %s\n", file_name);
		return -1;
	}

	PrintFileProps(capture);

	IplImage* img = NULL;
	
	cvNamedWindow(_MODULE_);
	cvSetMouseCallback(_MODULE_, on_mouse, &border);
	cvInitFont(&g_font, CV_FONT_HERSHEY_PLAIN, 1, 1);
	CvSize s = cvSize(512,512);
	LineData   = cvCreateImage(s,IPL_DEPTH_8U, 3);
//	s = cvSize(256,256);
//	LineDataST = cvCreateImage(s, IPL_DEPTH_8U, 3);

	while (true)
	{
		IplImage* frame = NULL;
		if (!paused)
		{
			frame = cvQueryFrame(capture);
			if (!frame)
				break;
			if (img == NULL)
			{
				int height = frame->height * DISPLAY_WIDTH / frame->width;
				img = cvCreateImage(CvSize(DISPLAY_WIDTH, height), IPL_DEPTH_8U, 3);
			}

			cvResize(frame, img);
			flow.Process(img);
			flow.DrawFlowWithLine(img, flow.GetModule(), flow.GetAngle(), 2, &border);
			if (flow.GetModule() != NULL)
			{
				if (sensor.DataLength() > 0)
				{
					sensor.Update(flow.GetModule(), flow.GetAngle());
					processor.AddData(sensor.Data());
				}
			}

			border.DrawBorderLine(img);
			DrawStatus(img, &border);
			DrawLineData(LineData);
			cvShowImage(_MODULE_, img);
			cvShowImage("Border data", LineData);
		}


		int c;
		c = cvWaitKey(10);
		if ((char)c == 27)
			break;
		if ((char)c == 'p')
			paused = !paused;
	}
	cvReleaseImage(&img);
	cvReleaseImage(&LineData);
	cvDestroyAllWindows();
	cvReleaseCapture(&capture);
	return 0;

}

int main(int argc, char** argv)
{
	if (argc < 2)
		return usage();
	
	return process(argv[1]);
}