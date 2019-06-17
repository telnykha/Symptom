#include "LFFightModule.h"
#include "zones.cpp"
#include "LFFBFlow.h"

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



CBGWrapper::CBGWrapper()
{
	m_foreground = NULL;
	count = 0;
}
CBGWrapper::~CBGWrapper()
{
}

void CBGWrapper::apply(IplImage* src)
{
	if (m_foreground == NULL)
		m_foreground = cvCreateImage(CvSize(src->width, src->height), src->depth, src->nChannels);

	awpImage img;

	img.nMagic = AWP_MAGIC2;
	img.bChannels = 1;
	img.sSizeX = src->width;
	img.sSizeY = src->height;
	img.dwType = AWP_BYTE;
	img.pPixels = src->imageData;

	m_motion.SetSourceImage(&img, true);
	awpImage* img0 = m_motion.GetForeground();
	if (img0 != NULL)
		memcpy(m_foreground->imageData, img0->pPixels, img0->sSizeX*img0->sSizeY);
}

IplImage* CBGWrapper::GetForeground()
{
	return m_foreground;
}


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
	m_fgbg.apply(m_cur);
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
	return m_fgbg.GetForeground();
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
			if (mdata[x + module->width*y] < t)
				cvLine(img, p1, p2, CV_RGB(0, 255, 0));
			else if (mdata[x + module->width*y] > t && mdata[x + module->width*y] < 2*t)
				cvLine(img, p1, p2, CV_RGB(255, 255, 0));
			else
				cvLine(img, p1, p2, CV_RGB(255, 0, 0));
		}
	}
}


CFlowAnalysis::CFlowAnalysis()
{
	m_centroid.X = 0;
	m_centroid.Y = 0;

	m_teta = 0;
	m_mi = 0;
	m_ma = 0;
	m_result = 0;
}

int CFlowAnalysis::GetResult()
{
	return m_result;
}

static AWPDOUBLE __Log2(AWPDOUBLE value)
{
	return log(value) / log(2.);
}
void CFlowAnalysis::ProcessFlow(CFBFlow* flow, bool draw)
{
	if (flow == NULL)
		return;
	IplImage* mask = flow->GetFG();
	IplImage* angle = flow->GetAngle();
	IplImage* mask1 = flow->GetMask();

	if (mask == NULL || angle == NULL)
		return;
	awpImage img;

	img.nMagic = AWP_MAGIC2;
	img.bChannels = 1;
	img.sSizeX = mask->width;
	img.sSizeY = mask->height;
	img.dwType = AWP_BYTE;
	img.pPixels = mask->imageData;

	awpPoint p; p.X = 1; p.Y = 1;
	awpPoint p1; p1.X = 110; p1.Y = 110;
	awpGetCentroid(&img, &m_centroid);
	//awpGetOrientation(&img, &m_teta, &m_mi, &m_ma);
	//awpDrawEllipse2(&img, m_centroid, m_mi, m_ma, m_teta, 0, 200, 1);

	awpRect rect;
	rect.left = m_centroid.X - 30 < 0 ? 0 : m_centroid.X - 30;
	rect.right = m_centroid.X + 30 >= angle->width ? angle->width - 1 : m_centroid.X + 30;
	rect.top = m_centroid.Y - 30 < 0 ? 0 : m_centroid.Y - 30;
	rect.bottom = m_centroid.Y + 30 >= angle->height ? angle->height - 1 : m_centroid.Y + 30;

	awpDrawRect(&img, &rect, 0, 128, 1);

	img.dwType = AWP_FLOAT;
	img.pPixels = angle->imageData;

	awpImage* roi = NULL;
	awpImage* roi_mask = NULL;
	awpCopyRect(&img, &roi, &rect);
	if (roi != NULL)
	{
#ifdef _DEBUG
		AWPDOUBLE* min = new AWPDOUBLE[1];
		AWPDOUBLE* max = new AWPDOUBLE[1];
		awpMinMax(roi, &min, &max);
		printf("min = %lf\t max = %lf\n", min[0], max[0]);
#endif


		img.dwType = AWP_BYTE;
		img.pPixels = mask1->imageData;

		awpCopyRect(&img, &roi_mask, &rect);


		AWPDOUBLE hst[361];
		memset(hst, 0, sizeof(hst));
		AWPFLOAT* data = (AWPFLOAT*)roi->pPixels;
		AWPBYTE* mask_data = (AWPBYTE*)roi_mask->pPixels;
		AWPDOUBLE sum = 0;
		for (int i = 0; i < roi->sSizeX*roi->sSizeY; i++)
		{
			if (mask_data[i] > 0)
			{
				int value = floor(data[i] + 0.5);
				hst[value]++;
				sum++;
			}
		}
		if (sum > 0)
		{
			AWPDOUBLE max_value = hst[0] / sum;
			int max_angle = 0;
			AWPDOUBLE e = 0;
			for (int i = 1; i < 360; i++)
			{
				hst[i] /= sum;
				if (hst[i] > max_value)
				{
					max_value = hst[i];
					max_angle = i;
				}
				if (hst[i] > 0)
					e += hst[i] * __Log2(hst[i]);
			}
			e = -e;
			sum /= (AWPDOUBLE)(roi->sSizeX*roi->sSizeY);
			sum *= 100;
			int d = (e > 6.0 && sum > 70) ? 1 : 0;
			m_result = d;
			printf("angle = %i\t e = %lf\t sum = %lf\t d = %i\n", max_angle, e, sum, d);
		}
		else
		{
			double e = 0;
			double sum = 0;
			int d = 0;
			m_result = d;
			printf("angle = %i\t e = %lf\t sum = %lf\t d = %i\n", 0, e, sum, d);
		}

		_AWP_SAFE_RELEASE_(roi_mask)
		_AWP_SAFE_RELEASE_(roi)
	}
	else
		printf("invaild roi.\n");


}
awpPoint CFlowAnalysis::GetCentroid()
{
	return m_centroid;
}

CvPoint  CFlowAnalysis::GetCvCentroid()
{
	return CvPoint(m_centroid.X, m_centroid.Y);
}

double CFlowAnalysis::GetTeta()
{
	return m_teta;
}
double CFlowAnalysis::GetMi()
{
	return m_mi;
}
double CFlowAnalysis::GetMa()
{
	return m_ma;
}



CFightEngine::CFightEngine(TVAInitParams& params)
{
	//TLFZones* z = ConvertVAParamsToZones(params);
	m_params.EventSens = params.EventSens;
	m_params.EventTimeSens = params.EventTimeSens;
	m_params.NumZones = 0;
	m_params.Zones = NULL;
	//this->SetZones(z);

	double t = 0.5*(1 - m_params.EventSens) + 0.05;
	processor.SetThreshold(t);

	m_buffer = new TLFBuffer(m_params.EventTimeSens*20, 0);
	//delete z;
	m_start_time = 0;

	m_fight.Num = 1;
	m_fight.blobs = new TVABlob[1];
}
CFightEngine::~CFightEngine()
{
	delete m_fight.blobs;
	delete m_buffer;
}
void CFightEngine::Clear()
{

}
bool CFightEngine::LoadXML(TiXmlElement* parent)
{
	return false;
}
TiXmlElement*  CFightEngine::SaveXML()
{
	return NULL;
}
bool CFightEngine::DetectInRect(awpRect* rect)
{
	return false;
}
TVAInitParams* CFightEngine::GetParams()
{
	return NULL;
}

bool CFightEngine::FindObjects()
{
	awpImage* img = this->m_SourceImage.GetImage();
	if (img == NULL)
		return false;
	
	if (img->bChannels == 3)
		awpConvert(img, AWP_CONVERT_3TO1_BYTE);
	
	if (image.GetImage() == NULL)
	{
		IplImage* _image = NULL;
		_image = cvCreateImage(CvSize(img->sSizeX, img->sSizeY), IPL_DEPTH_8U, 1);
		image.SetImage(_image);
		cvReleaseImage(&_image);
	}

	if (image.GetImage() != NULL)
	{
		IplImage* _im = image.GetImage();
		memcpy(_im->imageData, img->pPixels, img->sSizeX*img->sSizeY);
	}

	processor.Process(image.GetImage());
	analysis.ProcessFlow(&processor, true);
	m_buffer->Push(analysis.GetResult());
	
	double avg = m_buffer->GetAvg();
	printf("avg = %lf\n", avg);
	if (avg >= 0.6)
	{
		// set fight state 
		m_start_time = LFGetTickCount();
		m_State = 1;
	}
	else
	{
		DWORD delta = LFGetTickCount() - m_start_time;
		if (delta > m_params.EventTimeSens * 1000 && avg < 0.1)
			m_State = 0;
	}
	awpRect rect;
	awpPoint m_centroid = analysis.GetCentroid();
	m_centroid.X *= processor.GetScale();
	m_centroid.Y *= processor.GetScale();
	rect.left = m_centroid.X - 100 < 0 ? 0 : m_centroid.X - 100;
	rect.right = m_centroid.X + 100 >= image.GetImage()->width ? image.GetImage()->width - 1 : m_centroid.X + 100;
	rect.top = m_centroid.Y - 100 < 0 ? 0 : m_centroid.Y - 100;
	rect.bottom = m_centroid.Y + 100 >= image.GetImage()->height ? image.GetImage()->height - 1 : m_centroid.Y + 50;

	if (m_State > 0)
	{
		m_fight.Num = 1;
		m_fight.blobs[0].XPos = rect.left;
		m_fight.blobs[0].YPos = rect.top;
		m_fight.blobs[0].Width = rect.right - rect.left;
		m_fight.blobs[0].Height = rect.bottom - rect.top;
	}

	return true;
}

void CFightEngine::OverlapsFilter(TLFSemanticImageDescriptor* descriptor)
{

}
void CFightEngine::InitDetectors()
{

}

CFlowAnalysis* CFightEngine::GetAnalyser()
{
	return &analysis;
}
CFBFlow*	   CFightEngine::GetFlow()
{
	return &processor;
}

TVAResult*     CFightEngine::GetFight()
{
	if (m_State == 0)
		return NULL;
	return &m_fight;
}
