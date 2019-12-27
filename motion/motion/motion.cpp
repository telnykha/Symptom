#include "motion.h"
#include "awpipl.h"
#include "math.h"

unsigned long MGetTickCount()
{
#ifdef WIN32
	return GetTickCount();
#else
    struct timespec ts;
    unsigned theTick = 0U;
    clock_gettime( CLOCK_REALTIME, &ts );
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
#endif
}

/*
 * always correct input image width to _MOTION_WIDTH_
 * */
#define _MOTION_WIDTH_ 160 

class MotionDetectorCNT
{
private: 
	unsigned long m_delay;
protected:
	double	m_scale;
	int 	m_detectedCount;
	_CvRect* m_detectedRects;

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
	_CvRect* GetDetectedRects(int& rectCount);
};

MotionDetectorCNT::MotionDetectorCNT()
{
	m_src = NULL;
	m_acc = NULL;
	m_diff = NULL;
	m_scale = 1;
	m_detectedCount = 0;
	m_detectedRects = NULL;
	m_delay = 0;
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
	m_delay = 0;//MGetTickCount();
	
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
	{
		awpFreeStrokes(NumStrokes, &strokes);
		return;
	}
	m_detectedRects = (_CvRect*)malloc(m_detectedCount*sizeof(_CvRect));
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
	awpFreeStrokes(NumStrokes, &strokes);
}


int MotionDetectorCNT::ProcessHelper(awpImage* image, double sens, int mw, int mh)
{

	m_delay++;
	_AWP_SAFE_RELEASE_(m_src)
	awpImage* tmp = NULL;
	awpCopyImage(image, &tmp);

	// resize and copy 
	int height = (int)floor((double)tmp->sSizeY / m_scale + 0.5);
	awpResizeBilinear(tmp, _MOTION_WIDTH_, height);
	awpConvert(tmp, AWP_CONVERT_TO_DOUBLE);
	awpCopyImage(tmp, &m_src);
	_AWP_SAFE_RELEASE_(tmp)

	// acc
	awpRunningAvg(m_src, m_acc, 0.01);
	// abs_diff
    awpAbsDiff(m_src, m_acc, &m_diff);
    
	double threshold = (100 - sens)*1.05 + 15;
    
#ifdef _DEBUG 
	awpImage* tmp0 = NULL;
	awpCopyImage(m_acc, &tmp0);
	awpConvert(tmp0, AWP_CONVERT_TO_BYTE);
	awpSaveImage("acc.awp", tmp0);
	_AWP_SAFE_RELEASE_(tmp0)
	
	awpCopyImage(m_diff, &tmp0);
	awpConvert(tmp0, AWP_CONVERT_TO_BYTE);
	awpSaveImage("diff.awp", tmp0);
	_AWP_SAFE_RELEASE_(tmp0)

#endif 

	awpMakeBinary(m_diff, &tmp, threshold, AWP_BINARY, 0, 255, NULL);
	awpConvert(tmp, AWP_CONVERT_TO_BYTE);
#ifdef _DEBUG
	awpSaveImage("bin.awp", tmp);
#endif 
	FindRects(tmp, mw, mh);
	_AWP_SAFE_RELEASE_(tmp)
	
	return 0;
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

_CvRect* MotionDetectorCNT::GetDetectedRects(int& rectCount)
{
	//unsigned long t = MGetTickCount();
	if (m_delay < 200)
	{
		rectCount = 0;
		return NULL;
	}
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
	int bpp = bmpStride / bmpWidth;
	if (awpCreateGrayImage(&tmp, bmpWidth, bmpHeight, bpp, (AWPBYTE*)bmpImage) != AWP_OK)
		return;
	detector->ProcessImage(tmp, sensitivity, minObjWidthPercent, minObjHeightPercent);
	_AWP_SAFE_RELEASE_(tmp)
}
MOTION_API _CvRect*  GetMotionDetectorRects(MotionDetectorCNT* detector, int& rectCount)
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
