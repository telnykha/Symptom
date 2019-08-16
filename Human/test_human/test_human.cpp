// test_human.cpp : Defines the entry point for the console application.
//
#include "opencv\cv.h"
#include <opencv2/highgui.hpp>
#include "human.h"
#include "vautils.h"
#include <string>
#include <iostream>

const char _MODULE_[] = "test_human.";
const int  cNumObjects = 10;

using namespace cv;
using namespace std;

extern "C"
{
	#pragma comment(lib, "C:\\_alt\\_proj\\_thridparty\\opencv342\\opencv\\build\\x64\\vc14\\lib\\opencv_world342.lib")
}

int main(int argc, char* argv[])
{
	string str;
	TVAInitParams params;
	params.EventSens = 0.5;
	TVAResult result;// 
	result.blobs = new TVABlob[cNumObjects];
	result.Num     = 0;

	HANDLE hHuman = humanCreate(&params, false);
	if (!hHuman)
		return 0;

	VideoCapture cap;
	str = argv[1];
	cap.open(str);

	Mat frame1;
	while (waitKey(1) < 0)
	{
		// get frame from the video
		cap >> frame1;
		if (frame1.empty()) {
			cout << "Done processing !!!" << endl;
			waitKey(3000);
			break;
		}
		IplImage* frame = cvCloneImage(&(IplImage)frame1);

		humanProcess(hHuman, frame->width, frame->height, frame->nChannels, (unsigned char*)frame->imageData, &result);
		if (result.Num > 0)
		{

			for (int i = 0; i < result.Num; i++)
			{
				
				CvScalar color;
				color = Scalar(200, 200, 200);

				CvPoint p1,p2;

				p1.x = result.blobs[i].XPos;
				p1.y = result.blobs[i].YPos;

				p2.x = p1.x + result.blobs[i].Width;
				p2.y = p1.y + result.blobs[i].Height;

				cvRectangle(frame, p1, p2, color);
			}
		}

		cvShowImage(_MODULE_, frame);
		cvReleaseImage(&frame);
	}
	cap.release();
	humanRelease(&hHuman);
	free(result.blobs);
	return 0;
}

