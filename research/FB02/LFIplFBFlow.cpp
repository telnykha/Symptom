#include "LFIplFBFlow.h"

CFBFlow::CFBFlow(double scale)
{
	m_scale = scale;

	m_cur = NULL;
	m_prev = NULL;
	m_dst = NULL;

	m_module = NULL;
	m_angle = NULL;
	m_vflow = NULL;
	m_hflow = NULL;
	m_mask = NULL;

	m_pyrScale = 0.5;
	m_levels = 5;
	m_winSize = 25;
	m_iterations = 3;
	m_poly_n = 5;
	m_polySigma = 1.2;
	m_flags = 0;
	m_threshold = 0.8;
	m_moveCount = 0;
}

CFBFlow::~CFBFlow()
{
	FreeImages();
}

void CFBFlow::FreeImages()
{
	if (m_cur != NULL)
		cvReleaseImage(&m_cur);
	if (m_prev != NULL)
		cvReleaseImage(&m_prev);
	if (m_dst != NULL)
		cvReleaseImage(&m_dst);

	if (m_module != NULL)
		cvReleaseImage(&m_module);
	if (m_angle != NULL)
		cvReleaseImage(&m_angle);
	if (m_vflow != NULL)
		cvReleaseImage(&m_vflow);
	if (m_hflow != NULL)
		cvReleaseImage(&m_hflow);
	if (m_mask != NULL)
		cvReleaseImage(&m_mask);

}

void CFBFlow::Process(IplImage* image)
{
	if (image == NULL)
		return;

	IplImage* img0 = cvCreateImage(CvSize(image->width / m_scale, image->height / m_scale), IPL_DEPTH_8U, image->nChannels);
	cvResize(image, img0);
	if (m_prev == NULL)
	{
		m_prev = cvCreateImage(CvSize(img0->width, img0->height), IPL_DEPTH_8U, 1);
		m_cur = cvCreateImage(CvSize(img0->width, img0->height), IPL_DEPTH_8U, 1);
		m_dst = cvCreateImage(CvSize(img0->width, img0->height), IPL_DEPTH_32F, 2);
		if (img0->nChannels > 1)
			cvCvtColor(img0, m_cur, CV_BGR2GRAY);
		else
			cvCopy(img0, m_cur);

		cvCopy(m_cur, m_prev);
		cvReleaseImage(&img0);
		return;
	}

	if (img0->nChannels > 1)
		cvCvtColor(img0, m_cur, CV_BGR2GRAY);
	else
		cvCopy(img0, m_cur);
	cvCalcOpticalFlowFarneback(m_prev, m_cur, m_dst, m_pyrScale, m_levels, m_winSize, m_iterations, m_poly_n, m_polySigma, 0);
	m_buffer.AddImage(m_dst);
	cvCopy(m_cur, m_prev);
	MakeMoule();
	MakeMask();
	cvReleaseImage(&img0);
}

IplImage* CFBFlow::GetVFlow()
{
	if (m_dst == NULL)
		return NULL;
	if (this->m_vflow == NULL)
		this->m_vflow = cvCreateImage(CvSize(m_dst->width, m_dst->height), IPL_DEPTH_32F, 1);

	IplImage* result = m_vflow;
	float* src = (float*)m_dst->imageData;
	float* dst = (float*)result->imageData;
	for (int i = 0; i < m_dst->width*m_dst->height; i++)
		dst[i] = src[2 * i];
	return result;
}

IplImage* CFBFlow::GetHFlow()
{
	if (m_dst == NULL)
		return NULL;
	if (this->m_hflow == NULL)
		this->m_hflow = cvCreateImage(CvSize(m_dst->width, m_dst->height), IPL_DEPTH_32F, 1);

	IplImage* result = m_hflow;
	float* src = (float*)m_dst->imageData;
	float* dst = (float*)result->imageData;
	for (int i = 0; i < m_dst->width*m_dst->height; i++)
		dst[i] = src[2 * i + 1];
	return result;
}
void CFBFlow::MakeMoule()
{
	if (m_dst == NULL)
		return;
	if (this->m_module == NULL)
		this->m_module = cvCreateImage(CvSize(m_dst->width, m_dst->height), IPL_DEPTH_32F, 1);

	IplImage* result = m_module;
	IplImage* a = m_buffer.Average();
	if (a == NULL)
		return;
	float* src = (float*)a->imageData;
	float* dst = (float*)result->imageData;
	for (int i = 0; i < m_dst->width*m_dst->height; i++)
		dst[i] = sqrt(src[2 * i + 1] * src[2 * i + 1] + src[2 * i] * src[2 * i]);
	cvReleaseImage(&a);
}

IplImage* CFBFlow::GetModule()
{

	return m_module;
}
IplImage* CFBFlow::GetAngle()
{
	if (m_dst == NULL)
		return NULL;
	if (m_angle == NULL)
		m_angle = cvCreateImage(CvSize(m_dst->width, m_dst->height), IPL_DEPTH_32F, 1);
	IplImage* result = m_angle;
	IplImage* a = m_buffer.Average();
	if (a == NULL)
		return NULL;
	float* src = (float*)a->imageData;
	float* dst = (float*)result->imageData;
	for (int i = 0; i < m_dst->width*m_dst->height; i++)
		dst[i] = ::cvFastArctan(src[2 * i + 1], src[2 * i]);
	cvReleaseImage(&a);
	return result;
}
void CFBFlow::MakeMask()
{
	if (m_mask == NULL)
		m_mask = cvCreateImage(CvSize(m_dst->width, m_dst->height), IPL_DEPTH_8U, 1);

	IplImage* module = GetModule();
	if (module == NULL)
		return;
	IplImage* result = m_mask;

	float* src = (float*)m_module->imageData;
	unsigned char* dst = (unsigned char*)result->imageData;
	for (int i = 0; i < module->width*module->height; i++)
		dst[i] = src[i] < m_threshold ? 0 : 255;

	m_moveCount = 100.*(double)cvCountNonZero(result) / (double)(module->width*module->height);
}

IplImage* CFBFlow::GetMask()
{
	return m_mask;
}

IplImage* CFBFlow::GetFG()
{
	//return m_fgbg.GetForeground();
	return NULL;
}


double CFBFlow::GetThreshold()
{
	return m_threshold;
}
void   CFBFlow::SetThreshold(double value)
{
	m_threshold = value;
}

double CFBFlow::GetMoveCount()
{
	return m_moveCount;
}

double   CFBFlow::GetScale()
{
	return m_scale;
}


void CFBFlow::DrawFlow(IplImage* img, IplImage* module, IplImage* angle, double t, int result)
{
	if (img == NULL || module == NULL || angle == NULL)
		return;

	double xscale = (double)img->width / (double)module->width;
	double yscale = (double)img->height / (double)module->height;

	unsigned char* data = (unsigned char*)img->imageData;
	float*         mdata = (float*)module->imageData;
	float*         adata = (float*)angle->imageData;


	for (int y = 0; y < module->height; y += 4)
	{
		int _y = y*yscale;
		for (int x = 0; x < module->width; x += 4)
		{
			int _x = x*xscale;
			CvPoint p1 = cvPoint(_x, _y);
			CvPoint p2 = cvPoint(_x + 10 * mdata[x + module->width*y] * cos(2 * 3.14159*adata[x + module->width*y] / 360.), _y + 10 * mdata[x + module->width*y] * sin(2 * 3.14159*adata[x + module->width*y] / 360.));
			if (t > 0)
			{
				if (mdata[x + module->width*y] < t)
				{
					int a = 1;
					//cvLine(img, p1, p2, CV_RGB(0, 255, 0));
				}
					
				else if (mdata[x + module->width*y] > t && mdata[x + module->width*y] < 2 * t)
					cvLine(img, p1, p2, CV_RGB(255, 255, 0));
				else
					cvLine(img, p1, p2, CV_RGB(255, 0, 0));
			}
			else
			{
				if (adata[x + module->width*y] < 180)
					cvLine(img, p1, p2, CV_RGB(0, 255, 0));
				else
					cvLine(img, p1, p2, CV_RGB(0, 128, 0));
			}
				
		}
	}
}

void CFBFlow::DrawFlowWithLine(IplImage* img, IplImage* module, IplImage* angle, double t, CBorderLine* line)
{
	// setup line length threshold 
	if (line == NULL || line->Lenght() < 10)
		return;

	if (img == NULL || module == NULL || angle == NULL)
		return;

	double xscale = (double)img->width / (double)module->width;
	double yscale = (double)img->height / (double)module->height;

	unsigned char* data = (unsigned char*)img->imageData;
	float*         mdata = (float*)module->imageData;
	float*         adata = (float*)angle->imageData;

	for (int y = 0; y < module->height; y += 4)
	{
		int _y = y*yscale;
		for (int x = 0; x < module->width; x += 4)
		{
			int _x = x*xscale;
			CvPoint p1 = cvPoint(_x, _y);
			CvPoint p2 = cvPoint(_x + 10 * mdata[x + module->width*y] * cos(2 * 3.14159*adata[x + module->width*y] / 360.), _y + 10 * mdata[x + module->width*y] * sin(2 * 3.14159*adata[x + module->width*y] / 360.));
			
			if (line->Direction(adata[x + module->width*y]) == 1)
			{
				// blue +
				if (mdata[x + module->width*y] < t)
					cvLine(img, p1, p2, CV_RGB(0, 0, 50 * mdata[x + module->width*y]));
				else if (mdata[x + module->width*y] > t && mdata[x + module->width*y] < 2 * t)
					cvLine(img, p1, p2, CV_RGB(0, 0, 50 * mdata[x + module->width*y]));
				else
					cvLine(img, p1, p2, CV_RGB(0, 0, 50 * mdata[x + module->width*y]));
			}
			else
			{
				// red - 
				if (mdata[x + module->width*y] < t)
					cvLine(img, p1, p2, CV_RGB(50 * mdata[x + module->width*y], 0, 0));
				else if (mdata[x + module->width*y] > t && mdata[x + module->width*y] < 2 * t)
					cvLine(img, p1, p2, CV_RGB(50 * mdata[x + module->width*y], 0, 0));
				else
					cvLine(img, p1, p2, CV_RGB(50 * mdata[x + module->width*y], 0, 0));
			}
		}
	}
}
