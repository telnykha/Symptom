#include "LFIplImage.h"

CIplImage::CIplImage()
{
	m_pImage = NULL;
}

CIplImage::~CIplImage()
{
	if (m_pImage != NULL)
		cvReleaseImage(&m_pImage);
}

void CIplImage::SetImage(IplImage* image)
{
	if (m_pImage != NULL)
		cvReleaseImage(&m_pImage);
	if (image == NULL)
		return;
	m_pImage = cvCloneImage(image);
}
IplImage* CIplImage::GetImage()
{
	return m_pImage;
}

//////////////////////////////////////////
CImageBuffer::CImageBuffer(int bufferSize)
{
	m_bufferSize = bufferSize;
}
void CImageBuffer::AddImage(CIplImage* image)
{
	if (GetCount() < m_bufferSize)
		Add(image);
	else
	{
		Delete(0);
		Add(image);
	}
}
////////////////////////////////////////////
void CImageBuffer::AddImage(IplImage* image)
{
	CIplImage* img = new CIplImage();
	img->SetImage(image);
	this->AddImage(img);
}
////////////////////////////////////////////
CIplImage* CImageBuffer::GetImage(int index)
{
	CIplImage* img = (CIplImage*)Get(index);
	return img;
}
////////////////////////////////////////////
IplImage* CImageBuffer::Average()
{
	if (GetCount() == 0)
		return NULL;
	IplImage* average = NULL;
	IplImage* div = NULL;
	int c = GetCount();
	for (int i = 0; i < c; i++)
	{
		CIplImage* _img = GetImage(i);
		if (_img)
		{
			if (average == NULL)
			{
				average = cvCloneImage(_img->GetImage());
				div = cvCreateImage(CvSize(average->width, average->height), average->depth, average->nChannels);
				cvZero(div);
			}
			cvAdd(average, _img->GetImage(), average);
		}
	}
	cvAddS(div, CvScalar(c, c), div);
	cvDiv(average, div, average);
	cvReleaseImage(&div);
	return average;
}
