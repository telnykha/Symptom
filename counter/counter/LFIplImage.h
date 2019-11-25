#ifndef _lf_ipl_imge_h_
#define _lf_ipl_imge_h
#include "opencv\cv.h"
#include "_LF.h"

class CIplImage : public TLFObject
{
private:
	IplImage* m_pImage;
public:
	CIplImage();
	virtual ~CIplImage();

	void SetImage(IplImage* image);
	IplImage* GetImage();
};

class CImageBuffer : public TLFImageList
{
private:
	int m_bufferSize;
public:
	CImageBuffer(int bufferSize = 10);
	void AddImage(CIplImage* image);
	void AddImage(IplImage* image);
	CIplImage* GetImage(int index);
	IplImage*  Average();
};

#endif // _lf_ipl_imge_h