// awpFlowTracker.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "awpflowtracker.h"
#include "MedianFlowTracker.h"
/*
extern "C"
{
#ifdef _DEBUG
    #pragma comment(lib, "awpipl2d.lib")
#else
    #pragma comment(lib, "awpipl2.lib")
#endif
}
*/

using namespace tld;

static int awpImageDepthToCvDepth(awpImage* src)
{
    switch(src->dwType)
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
    switch(src->depth)
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
    switch(src->dwType)
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

static IplImage* awpConvertToIplImage(awpImage* src)
{
  IplImage* result = NULL;
  if (src == NULL)
    return result;
  CvSize size;
  if (awpCheckImage(src) != AWP_OK)
    return result;
  size.width  = src->sSizeX;
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
    memcpy(ipl,awp, awpStep);
    ipl += iplStep;
    awp += awpStep;
  }
  return result;
}


typedef struct
{
	int size;
	MedianFlowTracker* tracker;
}THETRACKER;

typedef struct
{
	int size;
	MedianFlowMultiTracker* tracker;
}THEMULTITRACKER;


extern "C" AWPFLOW_API HAWPFLOW awpflowCreate()
{
	THETRACKER* t = new THETRACKER;
	t->size = sizeof(THETRACKER);
	t->tracker = new MedianFlowTracker();
	
	
	return (HAWPFLOW)t;
}
extern "C" AWPFLOW_API HRESULT awpflowTrack(HAWPFLOW hflow, awpImage* image1, awpImage* image2,  awpRect* prevRect, awpRect* currRect)
{
	THETRACKER* t = (THETRACKER*)hflow;
	if (t->size != sizeof(THETRACKER))
		return E_FAIL;
	if (t->tracker == NULL)
		return E_FAIL;

	if (image1 == NULL || image2 == NULL || prevRect == NULL || currRect == NULL)
		return E_FAIL;
	if (image1->sSizeX != image2->sSizeX)
		return E_FAIL;
	if (image1->sSizeY != image2->sSizeY)
		return E_FAIL;
	if (image1->dwType != image2->dwType)
		return E_FAIL;
	if (image1->bChannels != image2->bChannels)
		return E_FAIL;
	if (image1->bChannels != 1)
		return E_FAIL;

	IplImage* img1 = awpConvertToIplImage(image1);
	IplImage* img2 = awpConvertToIplImage(image2);


	cv::Mat  mimg1(img1);
	cv::Mat  mimg2(img2);

	cv::Rect r(prevRect->left, prevRect->top, prevRect->right- prevRect->left, prevRect->bottom - prevRect->top);

	t->tracker->track(img1, img2, &r);


	cvReleaseImage(&img1);
	cvReleaseImage(&img2);

	if (t->tracker->trackerBB == NULL)
		return E_FAIL;

	currRect->left = t->tracker->trackerBB->x;
	currRect->top = t->tracker->trackerBB->y;
	currRect->right = t->tracker->trackerBB->x + t->tracker->trackerBB->width;
	currRect->bottom = t->tracker->trackerBB->y + t->tracker->trackerBB->height;

	return S_OK;
}

extern "C" AWPFLOW_API HRESULT awpflowRelease(HAWPFLOW hflow)
{
	THETRACKER* t = (THETRACKER*)hflow;
	if (t->size != sizeof(THETRACKER))
		return E_FAIL;
	if (t->tracker == NULL)
		return E_FAIL;
	delete t->tracker;
	delete t;
	return S_OK;
}


extern "C" AWPFLOW_API HAWPMFLOW awpmflowCreate()
{
	THEMULTITRACKER* t = new THEMULTITRACKER;
	t->size = sizeof(THEMULTITRACKER);
	t->tracker = new MedianFlowMultiTracker();
	
	return (HAWPMFLOW)t;
}

extern "C" AWPFLOW_API HRESULT  awpmflowTrack(HAWPMFLOW hflow, awpImage* image1, awpImage* image2,  awpID* prevID, awpID* currID, int num)
{
	
	if (num <= 0)
		return S_OK;
	
	THEMULTITRACKER* t = (THEMULTITRACKER*)hflow;
	if (t->size != sizeof(THEMULTITRACKER))
		return E_FAIL;
	if (t->tracker == NULL)
		return E_FAIL;

	if (image1 == NULL || image2 == NULL || prevID == NULL || currID == NULL)
		return E_FAIL;
	if (image1->sSizeX != image2->sSizeX)
		return E_FAIL;
	if (image1->sSizeY != image2->sSizeY)
		return E_FAIL;
	if (image1->dwType != image2->dwType)
		return E_FAIL;
	if (image1->bChannels != image2->bChannels)
		return E_FAIL;
	if (image1->bChannels != 1)
		return E_FAIL;

	IplImage* img1 = awpConvertToIplImage(image1);
	IplImage* img2 = awpConvertToIplImage(image2);

	cv::Mat  mimg1(img1);
	cv::Mat  mimg2(img2);

	UUID* ids = new UUID[num];
	int* s   = new int[num]; 
	cv::Rect*  bb = new cv::Rect[num];

	for (int i = 0; i < num; i++)
	{
		ids[i] = prevID[i].id;
		s[i]   = prevID[i].status;
		bb[i].x = prevID[i].rect.left;
		bb[i].y = prevID[i].rect.top;
		bb[i].width  = prevID[i].rect.right - prevID[i].rect.left;
		bb[i].height = prevID[i].rect.bottom - prevID[i].rect.top;
	}

	t->tracker->track(mimg1, mimg2, bb, ids, s, num);

	for (int i = 0; i < num; i++)
	{
		currID[i].id = t->tracker->m_id[i];
		currID[i].status = t->tracker->m_status[i];
		if (currID[i].status > 0)
		{
			currID[i].rect.left = t->tracker->trackerBB[i].x;
			currID[i].rect.top  = t->tracker->trackerBB[i].y;
			currID[i].rect.right = currID[i].rect.left + t->tracker->trackerBB[i].width; 
			currID[i].rect.bottom = currID[i].rect.top + t->tracker->trackerBB[i].height; 
		}
		else
		{
			currID[i].rect.left = prevID[i].rect.left;
			currID[i].rect.top  = prevID[i].rect.top;
			currID[i].rect.right = prevID[i].rect.right;
			currID[i].rect.bottom = prevID[i].rect.bottom;
		}
	}

	delete[] ids;
	delete[] s;
	delete[] bb;

	cvReleaseImage(&img1);
	cvReleaseImage(&img2);

	return S_OK;
}

extern "C" AWPFLOW_API HRESULT  awpmflowRelease(HAWPMFLOW hflow)
{
	THEMULTITRACKER* t = (THEMULTITRACKER*)hflow;
	if (t->size != sizeof(THEMULTITRACKER))
		return E_FAIL;
	if (t->tracker == NULL)
		return E_FAIL;
	delete t->tracker;
	delete t;
	return S_OK;
}


