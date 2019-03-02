#include <string.h>
#include "awpipl.h"
#include "opencv\cv.h"
#include "opencv\cxcore.h"
#include "opencv2\core\core.hpp"
#include "opencv2\core\utility.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\imgproc.hpp"

extern "C"
{
	#pragma comment(lib, "opencv_world300.lib")
}


static int awpImageDepthToCvDepth(awpImage* src)
{
	switch (src->dwType)
	{
	case AWP_BYTE:
		return IPL_DEPTH_8U;
	case AWP_SHORT:
		return IPL_DEPTH_8S;
	case AWP_FLOAT:
		return IPL_DEPTH_32F;
	case AWP_DOUBLE:
		return 64;
	default:
		return -1;
	}
}
//converts awpImage.awpType to openCV depth
//in the error case returns -1
static int awpCvDepthToImageDepth(IplImage* src)
{
	switch (src->depth)
	{
	case IPL_DEPTH_8U:
		return AWP_BYTE;
	case IPL_DEPTH_8S:
		return AWP_SHORT;
	case IPL_DEPTH_32F:
		return AWP_FLOAT;
	case 64:
		return AWP_DOUBLE;
	default:
		return -1;
	}
}
//returns line width of awpImage in BYTES
//in the error case returns -1
static int awpWidthStep(awpImage* src)
{
	int c = src->bChannels*src->sSizeX;
	switch (src->dwType)
	{
	case AWP_BYTE:
		return c*sizeof(AWPBYTE);
	case AWP_SHORT:
		return c*sizeof(short);
	case AWP_FLOAT:
		return c*sizeof(float);
	case AWP_DOUBLE:
		return c*sizeof(double);
	default:
		return -1;
	}
}
// awp to openCV image convertion
static IplImage* awpConvertToIplImage(awpImage* src)
{
	IplImage* result = NULL;
	if (src == NULL)
		return result;
	CvSize size;
	if (awpCheckImage(src) != AWP_OK)
		return result;
	size.width = src->sSizeX;
	size.height = src->sSizeY;
	int depth = awpImageDepthToCvDepth(src);
	if (depth == -1)
		return result;
	int awpStep = awpWidthStep(src);
	if (awpStep == -1)
		return result;
	result = cvCreateImage(size, depth, src->bChannels);
	if (result == NULL)
		return result;
	// copy image pixels
	AWPBYTE* awp = (AWPBYTE*)src->pPixels;
	AWPBYTE* ipl = (AWPBYTE*)result->imageData;

	int iplStep = result->widthStep;
	for (int i = 0; i < src->sSizeY; i++)
	{
		memcpy(ipl, awp, awpStep);
		ipl += iplStep;
		awp += awpStep;
	}
	return result;
}



//openCV to awp convertion
static awpImage* awpConvertToAwpImage(IplImage* src)
{
	awpImage* result = NULL;
	if (src == NULL)
		return result;
	//    if (src->nChannels != 1)
	//        return result;
	int type = awpCvDepthToImageDepth(src);
	if (type < 0)
		return result;
	if (awpCreateImage(&result, src->width, src->height, src->nChannels, type) != AWP_OK)
		return result;
	AWPBYTE* awp = (AWPBYTE*)result->pPixels;
	AWPBYTE* ipl = (AWPBYTE*)src->imageData;
	int awpStep = awpWidthStep(result);
	int iplStep = src->widthStep;
	for (int i = 0; i < result->sSizeY; i++)
	{
		memcpy(awp, ipl, awpStep);
		ipl += iplStep;
		awp += awpStep;
	}
	return result;

}



AWPRESULT awpcvFBFlow(awpImage* image1, awpImage* image2, awpImage* flow)
{
	if (image1 == NULL || image2 == NULL || flow == NULL)
		return AWP_BADARG;


	IplImage* img1 = awpConvertToIplImage(image1);
	IplImage* img2 = awpConvertToIplImage(image2);
	IplImage* img3 = awpConvertToIplImage(flow);

	if (img1 == NULL || img2 == NULL || img3 == NULL)
		return AWP_BADARG;

	// calc flow 
	cvCalcOpticalFlowFarneback(img1, img2, img3, 0.5, 3, 15, 3, 5, 1.2, 0);

	// copy flow 
	AWPFLOAT* af = (AWPFLOAT*)flow->pPixels;
	float*    ifl = (float*)img3->imageData;
	for (int i = 0; i < flow->sSizeY; i++)
	{
		for (int j = 0; j < flow->sSizeX; j++)
		{
			af[2 * i*flow->sSizeX + 2 * j] = ifl[i*img3->widthStep / sizeof(float) + 2 * j];
			af[2 * i*flow->sSizeX + 2 * j + 1] = ifl[i*img3->widthStep / sizeof(float) + 2 * j + 1];
		}
	}

	cvReleaseImage(&img1);
	cvReleaseImage(&img2);
	cvReleaseImage(&img3);
	return AWP_OK;
}

AWPRESULT awpcvFBFlow2(awpImage* image1, awpImage* image2, awpImage* module, awpImage* angle)
{
	AWPRESULT res = AWP_OK;
	awpImage* flow = NULL;
	res = awpCreateImage(&flow, image1->sSizeX, image1->sSizeY, 2, AWP_FLOAT);
	if (res != AWP_OK)
		return res;

	res = awpcvFBFlow(image1, image2, flow);
	if (res != AWP_OK)
	{
		_AWP_SAFE_RELEASE_(flow)
		return res;
	}

	AWPFLOAT* m_data = (AWPFLOAT*)module->pPixels;
	AWPFLOAT* a_data = (AWPFLOAT*)angle->pPixels;
	AWPFLOAT* f_data = (AWPFLOAT*)flow->pPixels;


	// преобразование изображения flow в изображения модуль - угол 
	for (int i = 0; i < image1->sSizeX*image1->sSizeY; i++)
	{
		float dx = f_data[2*i];
		float dy = f_data[2 * i + 1];
		m_data[i] = sqrt(dx*dx + dy*dy);
		a_data[i] = cv::fastAtan2(dy, dx);
	}

	_AWP_SAFE_RELEASE_(flow)

	return AWP_OK;
}

AWPRESULT awpcvDrawFlow(awpImage* src, awpImage* module, awpImage* angle)
{
	AWPRESULT res = AWP_OK;
	int sx = src->sSizeX / module->sSizeX;
	int sy = src->sSizeY / module->sSizeY;

	AWPDOUBLE* max = NULL;
	AWPDOUBLE* min = NULL;

	awpMinMax(module, &max, &min);

	return res;
}