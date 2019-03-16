#include "motion.h"
#include "awpipl.h"
#include "math.h"
/*
 * always correct input image width to _MOTION_WIDTH_
 * */
#define _MOTION_WIDTH_ 320 

class MotionDetectorCNT
{
protected:
	double	m_scale;
	int 	m_detectedCount;
	CvRect* m_detectedRects;

	awpImage* m_src;  // current image
	awpImage* m_acc;  // accamulator
	awpImage* m_diff; // binary image (motion)
	
	void Close();
	int Init(awpImage* image);
	int ProcessHelper(awpImage* image, double sens, int mw, int mh);
	void FindRects(awpImage* image, int mw, int mh);
public:
	MotionDetectorCNT();
	~MotionDetectorCNT();
	
	int ProcessImage(awpImage* src, double sens, int mw, int mh);
	CvRect* GetDetectedRects(int& rectCount);
};

MotionDetectorCNT::MotionDetectorCNT()
{
	m_src = NULL;
	m_acc = NULL;
	m_diff = NULL;
	m_scale = 1;
	m_detectedCount = 0;
	m_detectedRects = NULL;
}

MotionDetectorCNT::~MotionDetectorCNT()
{
	Close();
}

void MotionDetectorCNT::Close()
{
	_AWP_SAFE_RELEASE_(m_src)
	_AWP_SAFE_RELEASE_(m_acc)
	_AWP_SAFE_RELEASE_(m_diff)
	_AWP_SAFE_FREE_(m_detectedRects)
}

int MotionDetectorCNT::Init(awpImage* image)
{
	AWPRESULT res = AWP_OK;

	Close();
	
	m_scale = (double)image->sSizeX / _MOTION_WIDTH_;
	int height = (int)floor((double)image->sSizeY / m_scale + 0.5);
	
	res = awpCreateImage(&m_src, _MOTION_WIDTH_, height,  1, AWP_DOUBLE);
	res = awpCreateImage(&m_acc, _MOTION_WIDTH_, height,  1, AWP_DOUBLE);
	res = awpCreateImage(&m_diff, _MOTION_WIDTH_, height, 1, AWP_DOUBLE);
	
	return res == AWP_OK ? VA_OK:VA_ERROR;
}

void MotionDetectorCNT::FindRects(awpImage* image, int mw, int mh)
{
	_AWP_SAFE_FREE_(m_detectedRects)
	m_detectedCount = 0;
	int NumStrokes = 0;
	awpStrokeObj* strokes = NULL;
    awpGetStrokes(image, &NumStrokes, &strokes, 128, NULL);
    if (NumStrokes == 0)
		return;
	int count = 0;
	for (int i = 0; i < NumStrokes; i++)
	{
		awpRect rect;
		awpCalcObjRect(&strokes[i], &rect);
		double w = (double)(rect.right - rect.left);
		double h = (double)(rect.bottom - rect.top);
		if (mw < w* 100. / (double)image->sSizeX &&
			mh < h * 100. / (double)image->sSizeY)
		{
			count++;
		}
	}
	m_detectedCount = count;
	if (m_detectedCount == 0)
		return;
	m_detectedRects = (CvRect*)malloc(m_detectedCount*sizeof(CvRect));
	count = 0;
	for (int i = 0; i < NumStrokes; i++)
	{
		awpRect rect;
		awpCalcObjRect(&strokes[i], &rect);
		double w = (double)(rect.right - rect.left);
		double h = (double)(rect.bottom - rect.top);
		if (mw < w* 100. / (double)image->sSizeX &&
			mh < h * 100. / (double)image->sSizeY)
		{
			m_detectedRects[count].x = (int)floor(m_scale*rect.left + 0.5);
			m_detectedRects[count].y = (int)floor(m_scale*rect.top + 0.5);
			m_detectedRects[count].width  = (int)floor(m_scale*w + 0.5);
			m_detectedRects[count].height = (int)floor(m_scale*h + 0.5);
			count++;
		}
	}
}


int MotionDetectorCNT::ProcessHelper(awpImage* image, double sens, int mw, int mh)
{
	awpImage* tmp = NULL;
	awpCopyImage(image, &tmp);

	// resize and copy 
	int height = (int)floor((double)tmp->sSizeY / m_scale + 0.5);
	awpResizeBilinear(tmp, _MOTION_WIDTH_, height);
	awpConvert(tmp, AWP_CONVERT_TO_DOUBLE);
	awpCopyImage(tmp, &m_src);
	// acc
	awpRunningAvg(m_src, m_acc, 0.02);
	// abs_diff
    awpAbsDiff(m_src, m_acc, &m_diff);
    
	double threshold = (100 - sens) * 1.8 + 15;
    
	awpMakeBinary(m_diff, &tmp, threshold, AWP_BINARY, 0, 255, NULL);
	awpConvert(tmp, AWP_CONVERT_TO_BYTE);
	FindRects(tmp, mw, mh);
	_AWP_SAFE_RELEASE_(tmp)
}

int MotionDetectorCNT::ProcessImage(awpImage* src, double sens, int mw, int mh)
{
	if (src == NULL)
		return VA_ERROR;

	if (m_src == NULL)
		return Init(src);
	else
		return ProcessHelper(src, sens, mw, mh);
}

CvRect* MotionDetectorCNT::GetDetectedRects(int& rectCount)
{
	rectCount = m_detectedCount;
	return m_detectedRects;
}
/*====================================================================*/
MOTION_API MotionDetectorCNT*  InitMotionDetector()
{
	return new MotionDetectorCNT();
}
MOTION_API void  AnalyzeMotionDetectorArgb(MotionDetectorCNT* detector, char* bmpImage, int bmpStride, int bmpWidth, int bmpHeight, int sensitivity, int minObjWidthPercent, int minObjHeightPercent)
{
	awpImage* tmp = NULL;
	if (awpCreateGrayImage(&tmp, bmpWidth, bmpHeight, bmpStride, (AWPBYTE*)bmpImage) != AWP_OK)
		return;
	detector->ProcessImage(tmp, sensitivity, minObjWidthPercent, minObjHeightPercent);
	_AWP_SAFE_RELEASE_(tmp)
}
MOTION_API CvRect*  GetMotionDetectorRects(MotionDetectorCNT* detector, int& rectCount)
{
	return detector->GetDetectedRects(rectCount);
}
MOTION_API void  FreeMotionDetector(MotionDetectorCNT* detector)
{
	if (detector)
	{
		delete detector;
	}
}
