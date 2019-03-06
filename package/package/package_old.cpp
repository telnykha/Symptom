// package.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


static bool IsPointInContour(int x, int y, TVAZone* contour)
{
	int count = 0;
	for (int i = 0; i < contour->NumPoints; i++)
	{
		int j = (i + 1) % contour->NumPoints;
		//горизонтальный отрезок. 
		if ((int)contour->Points[i].Y == (int)contour->Points[j].Y)
			continue;
		//отрезок выше от луча
		else if (contour->Points[i].Y > y && contour->Points[j].Y > y)
			continue;
		//отрезок ниже от луча
		else if (contour->Points[i].Y < y && contour->Points[j].Y < y)
			continue;
		// отрезок справа от луча
		else if ((int)AWP_MIN(contour->Points[i].X, contour->Points[j].X) > x)
			continue;
		//нижняя граница на луче
		else if ((int)AWP_MIN(contour->Points[i].Y, contour->Points[j].Y) == (int)y)
			continue;
		//верхняя граница отрезка на луче
		else if ((int)AWP_MAX(contour->Points[i].Y, contour->Points[j].Y) == (int)y)
		{
			double x1 = contour->Points[i].Y > contour->Points[j].Y ? contour->Points[i].X : contour->Points[j].X;
			if (x > x1)
				count++;
		}
		else
		{
			double k, b;
			k = (contour->Points[j].Y - contour->Points[i].Y) / (contour->Points[j].X - contour->Points[i].X);
			b = contour->Points[j].Y - k*contour->Points[j].X;
			// точка пересечения. 
			double t;
			t = (y - b) / k;
			if (t < x)
				count++;
		}
	}
	return count & 1;
}

static bool IsPointInZone(int x, int y, TVAInitParams* params)
{
	//принадлежность точки прямоугольнику
	for (int i = 0; i < params->NumZones; i++)
	{
		if (params->Zones[i].IsRect)
		{

			if (x > params->Zones[i].Rect.LeftTop.X && x < params->Zones[i].Rect.RightBottom.X &&
				y > params->Zones[i].Rect.LeftTop.Y && y < params->Zones[i].Rect.RightBottom.Y)
				return true;
		}
		else
			return IsPointInContour(x, y, &params->Zones[i]);
	}

	return true;
}


class CStrokeDetecor 
{
protected:
	int m_minWidth;
	int m_minHeight;
	void Clear();
	awpRect* m_rects;
	int      m_count;
public:
	CStrokeDetecor();
	CStrokeDetecor(int minw, int minh);
	
	int		 Detect(awpImage* img);
	int		 GetRectsCount();
	awpRect* GetRect(int index);
};
//==================================================================================
CStrokeDetecor::CStrokeDetecor()
{
	m_rects = NULL;
	m_count  = 0;
	m_minWidth = 4;
	m_minHeight = 4;
}
CStrokeDetecor::CStrokeDetecor(int minw, int minh)
{
	m_rects = NULL;
	m_count  = 0;
	m_minWidth = minw;
	m_minHeight = minh;
}
void CStrokeDetecor::Clear()
{
	if (m_rects != NULL)
	{
		free(m_rects);
		m_rects = NULL;
		m_count  = 0;
	}
}

int  CStrokeDetecor::GetRectsCount()
{
	return this->m_count;
}
awpRect* CStrokeDetecor::GetRect(int index)
{
	if (index < 0 || index >= m_count)
		return NULL;
	return &m_rects[index];
}

int CStrokeDetecor::Detect(awpImage* img)
{
	if (img == NULL || img->bChannels != 1 || img->dwType != AWP_BYTE)
		return 0;

	int num = 0;
	awpStrokeObj* str = NULL;

	if (awpGetStrokes(img, &num, &str, 64, NULL) != AWP_OK)
		return 0;

	if (num == 0)
		return 0;
	int count = 0;
	for (int i = 0; i < num; i++)
	{
		awpStrokeObj* so = &str[i];
		awpRect r;
		awpCalcObjRect(so, &r);
		//фильтр 
		int rw, rh; 
		rw = r.right - r.left;
		rh = r.bottom - r.top;
		if (rw <= m_minWidth || rh <= m_minHeight)
		{
			awpDrawStrokeObj(img, so, 0);
			continue;
		}
		count++;
		m_rects = (awpRect*)realloc(m_rects, count*sizeof(awpRect));
		m_rects[count-1] = r;
	}
	m_count = count;
	awpFreeStrokes(num, &str);
	return m_count;
}

typedef struct 
{
	double data[255];
} HST;
typedef struct
{
	double data[32];
}HST_b;
int compare(const void* a, const void* b)
{
	unsigned char aa = *(unsigned char*)a;
	unsigned char bb = *(unsigned char*)b;
	if (aa > bb)
		return 1;
	else if (aa < bb)
		return -1;
	else
		return 0;
}
static void _awpMedian(awpImage* img, awpImage* img1)
{
	unsigned char buf[25];
	unsigned char* p = (unsigned char*) img->pPixels;
	unsigned char* p1 = (unsigned char*) img1->pPixels;
	int c = 0;
	for (int y = 3; y < img->sSizeY -3; y++)
	{
		for (int x = 3; x < img->sSizeX-3; x++)
		{
			c = 0;
			for (int yy = y-1; yy <= y+1; yy++)
			{
				for (int xx = x -1; xx <= x +1; xx++)
				{
					buf[c] = p[xx+yy*img->sSizeX];	
					c++;
				}
			}
			qsort(buf, 9, sizeof(unsigned char), compare);
			p1[x + img1->sSizeX*y] = buf[5];

		}
	}
}
static void _awpLBPTransform (awpImage* pSrcImage, awpImage* pDstImage, awpImage* pModel, int FrameNum, awpImage* pProb, awpRect* pRect)
{
	if (pSrcImage == NULL || pDstImage == NULL)
		return;
	if (pSrcImage->sSizeX != pDstImage->sSizeX || pSrcImage->sSizeY != pDstImage->sSizeY ||
		pSrcImage->dwType != pDstImage->dwType || pSrcImage->bChannels != pDstImage->bChannels)
		return;
	::awpFill(pDstImage, 0.);
	int sx,sy,ex,ey;
	if (pRect != NULL)
	{
		sx = pRect->left;
		sy = pRect->top;
		ex = pRect->right;
		ey = pRect->bottom;
	}
	else
	{
		sx = 1;
		sy = 1;
		ex = pSrcImage->sSizeX-1;
		ey = pSrcImage->sSizeY-1;
	}
	BYTE cv  = 0;
	BYTE* src = (BYTE*)pSrcImage->pPixels;
	BYTE* dst = (BYTE*)pDstImage->pPixels;
	int c = 0;
	int w = pSrcImage->sSizeX;
	int h = pSrcImage->sSizeY;
	int ww=0;
	BYTE v[8] = {1,2,4,8,16,32,64,128};
	HST* hstData = (HST*)pModel->pPixels;

	for (int y = sy; y < ey; y++)
	{
		ww = y*w;
		for (int x = sx; x <ex; x++)
		{
			cv = src[x + ww];
			dst[x+ww] = 0;
			int c = 0;
			BYTE lbp = 0;
			for (int yy = y - 1; yy <= y+1; yy++)
			{
				for (int xx = x - 1; xx <= x+1; xx++)
				{
					if (yy == y && xx == x)
						continue;
					
					if (src[xx+w*yy]  > src[x + ww])
						lbp |= v[c];
					c++;
				}
			}
			hstData[x+ww].data[lbp]++;
			double v = hstData[x + ww].data[lbp] / (double)FrameNum;

			// вероятностный порог. 
			if (v < 0.005)
				dst[x + ww] = 255;
			else
				dst[x + ww] = 0;

			if (dst[x+ww] == 255)
			{
			//	hstData[x+ww].data[lbp]--;
			}
		}
	}
}
// пересечение прямоугольников
float _awpRectsOverlap(awpRect r1, awpRect r2)
{

 int width = r1.right - r1.left;
 int height = r1.bottom - r1.top;
 int _width = r2.right - r2.left;
 int _height = r2.bottom - r2.top;
 if (r2.left > r1.left + width)
    return 0;
 if (r2.top > r1.top + height)
    return 0;
 if (r2.left + _width < r1.left)
    return 0;
 if (r1.top + _height < r1.top) 
    return 0;

 float colInt = AWP_MIN(r2.left + _width, r1.left + width) - AWP_MAX(r1.left, r1.left);
 float rowInt = AWP_MIN(r2.top + _height, r1.top + height) - AWP_MAX(r2.top, r1.top);

 float intersection = colInt*rowInt;
 float area1 = _width*_height;
 float area2 = width*height;

 return intersection / (area1+area2-intersection);
}

// абстрактный детектор переднего плана. 
// детектор переднего плана формирует бинарное изображение переднего плана (отличие от фона)
class IFGBGDetector
{
protected:
	double	  m_threshold;
	int		  m_FrameCounter;
	int		  m_coef;

	int		  m_delta_t;
	int		  m_local_t;
	int		  m_trainig_t;

	int		  m_mode;

	awpImage* m_pCurrentImage;
	awpImage* m_pForeground;
	
	virtual void FixBackground()	= 0;
	virtual void EventBackGround()  = 0;
	virtual void ContBackground()   = 0;

	virtual void DataTransform();

	virtual void AllocImages(awpImage* src)  = 0;
	virtual void Clear() = 0;

public:
	IFGBGDetector();

	virtual bool Detect(awpImage* img) = 0;
	// Получение данных 
	awpImage* GetLastIamge();
	awpImage* GetForeground();
	double	  GetThreshold();
	int		  GetFrameCounter();
	int		  GetCoef();
	int		  GetDelta_t();
	int		  GetMode();
	// Установка параметров
	void SetThreshold(double v);
	void SetDelta_t(int v);
	void SetMode(int v);
};

IFGBGDetector::IFGBGDetector()
{
	m_threshold = 0.5;
	m_FrameCounter = 0;
	m_coef = 4;

	m_delta_t = 5000;
	m_local_t = 0;
	m_trainig_t = 50;

	m_mode = PACKAGE_CONT_BACKGROUND;

	m_pCurrentImage = NULL;
	m_pForeground   = NULL;
}
void IFGBGDetector::DataTransform()
{
	switch (m_mode)
	{
	case PACKAGE_FIX_BACKGROUND:
		FixBackground();
		break;
	case PACKAGE_EVENT_BACKGROUND:
		EventBackGround();
		break;
	case PACKAGE_CONT_BACKGROUND:
		ContBackground();
		break;
	}
}
awpImage* IFGBGDetector::GetLastIamge()
{
	return this->m_pCurrentImage;
}
awpImage* IFGBGDetector::GetForeground()
{
	return this->m_pForeground;
}
double	  IFGBGDetector::GetThreshold()
{
	return this->m_threshold;
}
int		  IFGBGDetector::GetFrameCounter()
{
	return this->m_FrameCounter;
}
int		  IFGBGDetector::GetCoef()
{
	return this->m_coef;
}
int		  IFGBGDetector::GetDelta_t()
{
	return this->m_delta_t;
}
void IFGBGDetector::SetThreshold(double v)
{
	if (v >= 0 && v <= 1)
		this->m_threshold = v;
}
void IFGBGDetector::SetDelta_t(int v)
{
	if (v >= 1000 && v <= 600000)
		this->m_delta_t = v;
}
void IFGBGDetector::SetMode(int v)
{
	if (v >= 0 && v < 3)
		this->m_mode = v;
}
int IFGBGDetector::GetMode()
{
	return this->m_mode;
}

class TLImageList : public TLFObjectList
{
public: 
	TLFImage* GetImage(int index)
	{
		if (index >= 0 && index < GetCount())
			return (TLFImage*)Get(index);
		else
			return NULL;
	}
};
// Детектор оставленных объектов  основанный на "карусели фона".
// карусель фона это список изображений, хранящих фон за последние N*m_delta_t 
// 
const int c_nNumImages = 6;
class CFGBGSwirl : public IFGBGDetector
{
protected:
	TLImageList m_BG; // модель фона, состоящая из N изображений. 

	virtual void FixBackground();
	virtual void EventBackGround();
	virtual void ContBackground();

	virtual void AllocImages(awpImage* src);
	virtual void Clear();

public: 
	CFGBGSwirl();
	~CFGBGSwirl();
	
	virtual bool Detect(awpImage* img);
};

CFGBGSwirl::CFGBGSwirl()
{
	this->m_pCurrentImage = NULL;
	this->m_pForeground = NULL;
	this->m_FrameCounter = 0;
	m_coef = 4;
	this->m_threshold = 0.5;
	for (int i = 0; i < c_nNumImages; i++)
	{
		TLFImage* img = new TLFImage();
		m_BG.Add(img);
	}
}

CFGBGSwirl::~CFGBGSwirl()
{
	
	Clear();
}

void CFGBGSwirl::FixBackground()
{
	double thr = 0.003 - 0.0025*this->m_threshold;

	awpImage* m_F1 = this->m_BG.GetImage(0)->GetImage();
	awpImage* m_pModel = this->m_BG.GetImage(1)->GetImage();
	awpImage* norm = NULL;
	awpNormalize(this->m_pCurrentImage, &norm, AWP_NORM_L2);

	AWPDOUBLE* s = (AWPDOUBLE*)norm->pPixels;
	AWPDOUBLE* d = (AWPDOUBLE*)m_F1->pPixels;
	// накопление
	for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY*m_F1->bChannels; i++)
		d[i] += s[i];

	if (m_FrameCounter > m_trainig_t)
	{
		//m_local_t++;
		int t = GetTickCount() - m_local_t;
		if (t > m_delta_t)
		{
			// формирование переднего плана
			m_local_t = GetTickCount();
			awpImage* acc_norm = NULL;
			awpNormalize(m_F1, &acc_norm, AWP_NORM_L2);
			s = (AWPDOUBLE*)acc_norm->pPixels;
			d = (AWPDOUBLE*)m_pModel->pPixels;
			AWPBYTE* b = (AWPBYTE*)m_pForeground->pPixels;
			for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
			{
				
				bool result = false;
				for (int k = 0; k < m_F1->bChannels; k++)
				{
					if (fabs(s[m_F1->bChannels*i + k] - d[m_F1->bChannels*i + k]) > thr)
						result = true;
				}

				b[i] = result ? 255 : 0;

			}
			awpZeroImage(m_F1);
			_AWP_SAFE_RELEASE_(acc_norm);
		}

	}
	else if (m_FrameCounter == m_trainig_t)
	{

		m_local_t = GetTickCount();

		// формирование модели
		TLFImage* img = this->m_BG.GetImage(1);
		_AWP_SAFE_RELEASE_(m_pModel);
		awpNormalize(m_F1, &m_pModel, AWP_NORM_L2);
		img->SetImage(m_pModel);
	}

	_AWP_SAFE_RELEASE_(norm);

}
void CFGBGSwirl::EventBackGround()
{
	double thr = 0.003 - 0.0025*this->m_threshold;

	awpImage* m_F1 = this->m_BG.GetImage(0)->GetImage();
	awpImage* m_pModel = this->m_BG.GetImage(1)->GetImage();

	awpImage* norm = NULL;
	awpNormalize(this->m_pCurrentImage, &norm, AWP_NORM_L2);

	AWPDOUBLE* s = (AWPDOUBLE*)norm->pPixels;
	AWPDOUBLE* d = (AWPDOUBLE*)m_F1->pPixels;
	// накопление
	for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY*m_F1->bChannels; i++)
		d[i] += s[i];

	if (m_FrameCounter > m_trainig_t)
	{
		//m_local_t++;
		int t = GetTickCount() - m_local_t;
		if (t > m_delta_t)
		{
			// формирование переднего плана
			m_local_t = GetTickCount();
			awpImage* acc_norm = NULL;
			awpNormalize(m_F1, &acc_norm, AWP_NORM_L2);
			s = (AWPDOUBLE*)acc_norm->pPixels;
			d = (AWPDOUBLE*)m_pModel->pPixels;
			AWPBYTE* b = (AWPBYTE*)m_pForeground->pPixels;
			for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
			{
				bool result = false;
				for (int k = 0; k < m_F1->bChannels; k++)
				{
					if (fabs(s[m_F1->bChannels*i + k] - d[m_F1->bChannels*i + k]) > thr)
						result = true;
				}

				b[i] = result ? 255 : 0;
			}

			_AWP_SAFE_RELEASE_(m_pModel);
			awpNormalize(m_F1, &m_pModel, AWP_NORM_L2);

			awpZeroImage(m_F1);
			_AWP_SAFE_RELEASE_(acc_norm);
		}

	}
	else if (m_FrameCounter == m_trainig_t)
	{

		m_local_t = GetTickCount();
		TLFImage* img = this->m_BG.GetImage(1);
		_AWP_SAFE_RELEASE_(m_pModel);
		awpNormalize(m_F1, &m_pModel, AWP_NORM_L2);
		img->SetImage(m_pModel);

		// формирование модели
		//_AWP_SAFE_RELEASE_(m_pModel);
		//awpNormalize(m_F1, &m_pModel, AWP_NORM_L2);
	}

	_AWP_SAFE_RELEASE_(norm);
}
void CFGBGSwirl::ContBackground()
{
	double thr = 0.003 - 0.0025*this->m_threshold;

	awpImage* m_F1 = this->m_BG.GetImage(0)->GetImage();
	awpImage* m_pModel = this->m_BG.GetImage(1)->GetImage();
	if (awpCheckImage(m_pModel) != AWP_OK)
		return;
	awpImage* norm = NULL;
	awpNormalize(this->m_pCurrentImage, &norm, AWP_NORM_L2);

	AWPDOUBLE* s = (AWPDOUBLE*)norm->pPixels;
	AWPDOUBLE* d = (AWPDOUBLE*)m_F1->pPixels;
	AWPDOUBLE* m = (AWPDOUBLE*)m_pModel->pPixels;
	// накопление
	for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY*m_F1->bChannels; i++)
	{
		d[i] += s[i];
		m[i] += s[i];
	}

	if (m_FrameCounter > m_trainig_t)
	{
		int t = GetTickCount() - m_local_t;
		if (t > m_delta_t)
		{
			// формирование переднего плана
			m_local_t = GetTickCount();
			awpImage* acc_norm = NULL;
			awpImage* model_norm = NULL;
			awpImage* model = NULL;
			awpCreateImage(&model, m_F1->sSizeX, m_F1->sSizeY, m_F1->bChannels, AWP_DOUBLE);
			AWPDOUBLE* mpix = (AWPDOUBLE*)model->pPixels;

			for (int k = 1; k < m_BG.GetCount(); k++)
			{
				awpImage* img = m_BG.GetImage(k)->GetImage();
				AWPDOUBLE* img_pix = (AWPDOUBLE*)img->pPixels;
				for (int i = 0; i < model->sSizeX*model->sSizeY*model->bChannels; i++)
					mpix[i] += img_pix[i];

			}

			awpNormalize(m_F1, &acc_norm, AWP_NORM_L2);
			awpNormalize(model, &model_norm, AWP_NORM_L2);
			s = (AWPDOUBLE*)acc_norm->pPixels;
			d = (AWPDOUBLE*)model_norm->pPixels;

			AWPBYTE* b = (AWPBYTE*)m_pForeground->pPixels;
			for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
			{
				bool result = false;
				for (int k = 0; k < m_F1->bChannels; k++)
				{
					if (fabs(s[m_F1->bChannels*i + k] - d[m_F1->bChannels*i + k]) > thr)
						result = true;
				}

				b[i] = result ? 255 : 0;
			}
			_AWP_SAFE_RELEASE_(acc_norm);
			_AWP_SAFE_RELEASE_(model_norm);
			_AWP_SAFE_RELEASE_(model);

			for (int i = 1; i < m_BG.GetCount() - 1; i++)
			{
				TLFImage* i1 = m_BG.GetImage(i);
				TLFImage* i2 = m_BG.GetImage(i+1);
				i2->SetImage(i1->GetImage());
			}
			awpZeroImage(m_F1);
			awpZeroImage(m_pModel);
		}

	}
	else if (m_FrameCounter == m_trainig_t)
	{

		m_local_t = GetTickCount();

		// формирование модели
//		_AWP_SAFE_RELEASE_(m_pModel);
//		awpNormalize(m_F1, &m_pModel, AWP_NORM_L2);
	}

	_AWP_SAFE_RELEASE_(norm);

}

void CFGBGSwirl::AllocImages(awpImage* src)
{
	Clear();
	m_coef = src->sSizeX / 64;
	awpCreateImage(&this->m_pCurrentImage, src->sSizeX / m_coef, src->sSizeY / m_coef, src->bChannels, AWP_BYTE);
	awpCreateImage(&this->m_pForeground, src->sSizeX / m_coef, src->sSizeY   / m_coef, 1, AWP_BYTE);
	for (int i = 0; i < c_nNumImages; i++)
	{
		awpImage* img = NULL;
		awpCreateImage(&img, src->sSizeX / m_coef, src->sSizeY / m_coef, src->bChannels, AWP_DOUBLE);
		if (img != NULL)
		{
			TLFImage* img_lf = m_BG.GetImage(i);
			if (img_lf)
				img_lf->SetImage(img);
			awpReleaseImage(&img);
		}
	}

}
void CFGBGSwirl::Clear()
{
	_AWP_SAFE_RELEASE_(m_pCurrentImage);
	_AWP_SAFE_RELEASE_(m_pForeground);
	for (int i = 0; i < c_nNumImages; i++)
	{
		TLFImage* img = m_BG.GetImage(i);
		if (img != NULL)
			img->FreeImages();
	}
	m_FrameCounter = 0;
}

bool CFGBGSwirl::Detect(awpImage* img)
{
	awpImage* m_pModel = m_BG.GetImage(0)->GetImage();

	if (m_pModel == NULL || m_pCurrentImage == NULL || m_pForeground == NULL ||
		m_pModel->sSizeX != img->sSizeX / m_coef || m_pModel->sSizeY != img->sSizeY / m_coef || m_pModel->bChannels != img->bChannels)
		this->AllocImages(img);

	awpImage* src = NULL;
	awpCopyImage(img, &src);

	awpResize(src, src->sSizeX / m_coef, src->sSizeY / m_coef);

	awpCopyImage(src, &this->m_pCurrentImage);
	this->m_FrameCounter++;
	this->DataTransform();
	awpImage* ti = NULL;

	_AWP_SAFE_RELEASE_(src);
	return true;
}

// Детектор оставленных объектов, основанный на распределении LBP признака в пикселе
class CFGBGDetector : public IFGBGDetector
{
protected:
	awpImage* m_F1;
	awpImage* m_pModel;
protected:

	virtual void FixBackground();
	virtual void EventBackGround();
	virtual void ContBackground();

	virtual void AllocImages(awpImage* src);
	virtual void Clear();
public:
	CFGBGDetector();
	~CFGBGDetector();

	// action
	virtual bool Detect(awpImage* img);
};
CFGBGDetector::CFGBGDetector() : IFGBGDetector()
{
	this->m_pCurrentImage = NULL;
	this->m_F1 = NULL;
	this->m_pForeground = NULL; 
	this->m_FrameCounter = 0;
	this->m_pModel = NULL;
	m_coef = 4;
	this->m_threshold = 0.5;
}
CFGBGDetector::~CFGBGDetector()
{
	_AWP_SAFE_RELEASE_(m_pCurrentImage);
	_AWP_SAFE_RELEASE_(m_pForeground);
	_AWP_SAFE_RELEASE_(m_pModel);
	_AWP_SAFE_RELEASE_(m_F1);
	m_FrameCounter = 0;
}
void CFGBGDetector::AllocImages(awpImage* src)
{
	Clear();
	m_coef = src->sSizeX / 256;
	awpCreateImage(&this->m_pCurrentImage, src->sSizeX/m_coef, src->sSizeY/m_coef, 1, AWP_BYTE);
	awpCreateImage(&this->m_F1, src->sSizeX/m_coef, src->sSizeY/m_coef, 1, AWP_BYTE);
	awpCreateImage(&this->m_pForeground, src->sSizeX/m_coef, src->sSizeY/m_coef, 1, AWP_BYTE);
	awpCreateImage(&this->m_pModel, src->sSizeX/m_coef, src->sSizeY/m_coef, 255, AWP_DOUBLE);
}
void CFGBGDetector::Clear()
{
	_AWP_SAFE_RELEASE_(m_pCurrentImage);
	_AWP_SAFE_RELEASE_(m_pForeground);
	_AWP_SAFE_RELEASE_(m_pModel);
	_AWP_SAFE_RELEASE_(m_F1);
	m_FrameCounter = 0;
}
bool CFGBGDetector::Detect(awpImage* img)
{
	if (m_pModel == NULL || m_pCurrentImage == NULL || m_pForeground == NULL ||
		m_pModel->sSizeX != img->sSizeX/m_coef || m_pModel->sSizeY != img->sSizeY/m_coef)
		this->AllocImages(img);

	awpImage* src = NULL;

	awpCopyImage(img, &src);
	awpConvert(src, AWP_CONVERT_3TO1_BYTE);
	awpResize(src, src->sSizeX/m_coef, src->sSizeY/m_coef);

	//awpGaussianBlur(src, src, 1);

	awpCopyImage(src, &this->m_pCurrentImage);
	this->m_FrameCounter++;
//	this->LBPtransform(this->m_pCurrentImage, this->m_F1);
	this->DataTransform();

	_AWP_SAFE_RELEASE_(src);
	return true;
}
void CFGBGDetector::FixBackground()
{
	_awpLBPTransform(m_pCurrentImage, m_F1, m_pModel, this->m_FrameCounter, NULL, NULL);
	_awpMedian(m_F1, this->m_pForeground);

}
void CFGBGDetector::EventBackGround()
{
	_awpLBPTransform(m_pCurrentImage, m_F1, m_pModel, this->m_FrameCounter, NULL, NULL);
	_awpMedian(m_F1, this->m_pForeground);

}
void CFGBGDetector::ContBackground()
{
	_awpLBPTransform(m_pCurrentImage, m_F1, m_pModel, this->m_FrameCounter, NULL, NULL);
	_awpMedian(m_F1, this->m_pForeground);

}

// детектор оставленных объектов, основанный на гистограмме распределения яркости в пикселе 
class CFGBGDetector_b : public CFGBGDetector
{
public:
	virtual void LBPtransform(awpImage* src, awpImage* dst);
	virtual void AllocImages(awpImage* src);
};
void CFGBGDetector_b::LBPtransform(awpImage* src, awpImage* dst)
{
	if (src == NULL || dst == NULL)
		return;
	if (src->sSizeX != dst->sSizeX || src->sSizeY != dst->sSizeY ||
		src->dwType != dst->dwType || src->bChannels != dst->bChannels)
		return;
	::awpFill(dst, 0.);
	int sx, sy, ex, ey;
	sx = 1;
	sy = 1;
	ex = src->sSizeX - 1;
	ey = src->sSizeY - 1;

	BYTE cv = 0;
	BYTE* src1 = (BYTE*)src->pPixels;
	BYTE* dst1 = (BYTE*)dst->pPixels;
	int c = 0;
	int w = src->sSizeX;
	int h = src->sSizeY;
	int ww = 0;
	HST* hstData = (HST*)this->m_pModel->pPixels;

	for (int y = sy; y < ey; y++)
	{
		ww = y*w;
		for (int x = sx; x <ex; x++)
		{
			cv = src1[x + ww];
			dst1[x + ww] = 0;
			
			hstData[x + ww].data[cv]++;
			double v = hstData[x + ww].data[cv] / (double)this->m_FrameCounter;

			// вероятностный порог. 
			if (v < 0.003)
				dst1[x + ww] = 255;
			else
				dst1[x + ww] = 0;

			// получена текущая карта переднего плана. 
			// 
			
			if (dst1[x + ww] == 255)
			{
				hstData[x + ww].data[cv ]--;
			}
		}
	}

	_awpMedian(m_F1, this->m_pForeground);
}
void CFGBGDetector_b::AllocImages(awpImage* src)
{
	Clear();
	m_coef = src->sSizeX / 256;
	awpCreateImage(&this->m_pCurrentImage, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_BYTE);
	awpCreateImage(&this->m_F1, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_BYTE);
	awpCreateImage(&this->m_pForeground, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_BYTE);
	awpCreateImage(&this->m_pModel, src->sSizeX / m_coef, src->sSizeY / m_coef, 255, AWP_DOUBLE);
}

// детектор оставленных объектов, основанный на аккамуляторе яркости. 
class CFGBGDetector_bacc : public CFGBGDetector
{
protected:
	virtual void FixBackground();
	virtual void EventBackGround();
	virtual void ContBackground();
	virtual void AllocImages(awpImage* src);
public:
	CFGBGDetector_bacc();
	virtual bool Detect(awpImage* img);
};

CFGBGDetector_bacc::CFGBGDetector_bacc() :CFGBGDetector()
{
	m_delta_t = 250;
	m_trainig_t = 100;
	m_local_t = 0;
} 
void CFGBGDetector_bacc::ContBackground()
{
	double thr = 0.003 - 0.0025*this->m_threshold;

	
	awpImage* norm = NULL;
	awpNormalize(this->m_pCurrentImage, &norm, AWP_NORM_L2);

	AWPDOUBLE* s = (AWPDOUBLE*)norm->pPixels;
	AWPDOUBLE* d = (AWPDOUBLE*)this->m_F1->pPixels;
	AWPDOUBLE* m = (AWPDOUBLE*)this->m_pModel->pPixels;
	// накопление
	for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
	{
		d[i] += s[i];
		m[i] += s[i];
	}

	if (m_FrameCounter > m_trainig_t)
	{
		int t = GetTickCount() - m_local_t;
		if (t > m_delta_t)
		{
			// формирование переднего плана
			m_local_t = GetTickCount();
			awpImage* acc_norm = NULL;
			awpImage* model_norm = NULL;
			awpNormalize(m_F1, &acc_norm, AWP_NORM_L2);
			awpNormalize(m_pModel, &model_norm, AWP_NORM_L2);
			s = (AWPDOUBLE*)acc_norm->pPixels;
			d = (AWPDOUBLE*)model_norm->pPixels;

			AWPBYTE* b = (AWPBYTE*)m_pForeground->pPixels;
			for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
			{
				if (fabs(s[i] - d[i]) > thr)
					b[i] = 255;
				else
					b[i] = 0;
			}
			awpZeroImage(m_F1);
			_AWP_SAFE_RELEASE_(acc_norm);
			_AWP_SAFE_RELEASE_(model_norm);
		}
		
	}
	else if (m_FrameCounter == m_trainig_t)
	{
		
		m_local_t = GetTickCount();
		
		// формирование модели
		_AWP_SAFE_RELEASE_(m_pModel);
		awpNormalize(m_F1, &m_pModel, AWP_NORM_L2);
	}

	_AWP_SAFE_RELEASE_(norm);
}
void CFGBGDetector_bacc::EventBackGround()
{
	double thr = 0.003 - 0.0025*this->m_threshold;


	awpImage* norm = NULL;
	awpNormalize(this->m_pCurrentImage, &norm, AWP_NORM_L2);

	AWPDOUBLE* s = (AWPDOUBLE*)norm->pPixels;
	AWPDOUBLE* d = (AWPDOUBLE*)this->m_F1->pPixels;
	// накопление
	for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
		d[i] += s[i];

	if (m_FrameCounter > m_trainig_t)
	{
		//m_local_t++;
		int t = GetTickCount() - m_local_t;
		if (t > m_delta_t)
		{
			// формирование переднего плана
			m_local_t = GetTickCount();
			awpImage* acc_norm = NULL;
			awpNormalize(m_F1, &acc_norm, AWP_NORM_L2);
			s = (AWPDOUBLE*)acc_norm->pPixels;
			d = (AWPDOUBLE*)m_pModel->pPixels;
			AWPBYTE* b = (AWPBYTE*)m_pForeground->pPixels;
			for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
			{
				if (fabs(s[i] - d[i]) > thr)
					b[i] = 255;
				else
					b[i] = 0;
			}

			_AWP_SAFE_RELEASE_(m_pModel);
			awpNormalize(m_F1, &m_pModel, AWP_NORM_L2);

			awpZeroImage(m_F1);
			_AWP_SAFE_RELEASE_(acc_norm);
		}

	}
	else if (m_FrameCounter == m_trainig_t)
	{

		m_local_t = GetTickCount();

		// формирование модели
		_AWP_SAFE_RELEASE_(m_pModel);
		awpNormalize(m_F1, &m_pModel, AWP_NORM_L2);
	}

	_AWP_SAFE_RELEASE_(norm);
}
void CFGBGDetector_bacc::FixBackground() 
{
	double thr = 0.003 - 0.0025*this->m_threshold;


	awpImage* norm = NULL;
	awpNormalize(this->m_pCurrentImage, &norm, AWP_NORM_L2);

	AWPDOUBLE* s = (AWPDOUBLE*)norm->pPixels;
	AWPDOUBLE* d = (AWPDOUBLE*)this->m_F1->pPixels;
	// накопление
	for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
		d[i] += s[i];

	if (m_FrameCounter > m_trainig_t)
	{
		//m_local_t++;
		int t = GetTickCount() - m_local_t;
		if (t > m_delta_t)
		{
			// формирование переднего плана
			m_local_t = GetTickCount();
			awpImage* acc_norm = NULL;
			awpNormalize(m_F1, &acc_norm, AWP_NORM_L2);
			s = (AWPDOUBLE*)acc_norm->pPixels;
			d = (AWPDOUBLE*)m_pModel->pPixels;
			AWPBYTE* b = (AWPBYTE*)m_pForeground->pPixels;
			for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
			{
				if (fabs(s[i] - d[i]) > thr)
					b[i] = 255;
				else
					b[i] = 0;
			}
			awpZeroImage(m_F1);
			_AWP_SAFE_RELEASE_(acc_norm);
		}

	}
	else if (m_FrameCounter == m_trainig_t)
	{

		m_local_t = GetTickCount();

		// формирование модели
		_AWP_SAFE_RELEASE_(m_pModel);
		awpNormalize(m_F1, &m_pModel, AWP_NORM_L2);
	}

	_AWP_SAFE_RELEASE_(norm);
}
void CFGBGDetector_bacc::AllocImages(awpImage* src)
{
	Clear();
	m_coef = src->sSizeX / 256;
	awpCreateImage(&this->m_pCurrentImage, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_BYTE);
	awpCreateImage(&this->m_F1, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_DOUBLE);
	awpCreateImage(&this->m_pForeground, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_BYTE);
	awpCreateImage(&this->m_pModel, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_DOUBLE);
}
bool CFGBGDetector_bacc::Detect(awpImage* img)
{
	if (m_pModel == NULL || m_pCurrentImage == NULL || m_pForeground == NULL ||
		m_pModel->sSizeX != img->sSizeX / m_coef || m_pModel->sSizeY != img->sSizeY / m_coef)
		this->AllocImages(img);

	awpImage* src = NULL;
	awpImage* src1 = NULL;
	awpCopyImage(img, &src);
	if (src->bChannels == 3)
		awpConvert(src, AWP_CONVERT_3TO1_BYTE);
	awpResize(src, src->sSizeX / m_coef, src->sSizeY / m_coef);
	awpCopyImage(src, &src1);
	// первичное шумоподавление 
	
	awpGaussianBlur(src1, src, 1);

	awpCopyImage(src, &this->m_pCurrentImage);
	this->m_FrameCounter++;
	this->DataTransform();

	_AWP_SAFE_RELEASE_(src1);
	_AWP_SAFE_RELEASE_(src);
	return true;
}

// детектор оставленных объектов, основанный на аккамуляторе яркости цветовых компонент rgb.
class CFGBGDetector_rgbacc : public CFGBGDetector_bacc
{
protected:
	TLFObjectList m_images;
	awpImage* m_F1_1;
	awpImage* m_pModel1;
	awpImage* m_F1_2;
	awpImage* m_pModel2;
	
	virtual void FixBackground();
	virtual void EventBackGround();
	virtual void ContBackground();

//	virtual void LBPtransform(awpImage* src, awpImage* dst);
	virtual void AllocImages(awpImage* src);
	virtual void Clear();
public:
	CFGBGDetector_rgbacc();
	virtual bool Detect(awpImage* img);

};
CFGBGDetector_rgbacc::CFGBGDetector_rgbacc() : CFGBGDetector_bacc()
{
	m_F1_1    = NULL;
	m_pModel1 = NULL;
	m_F1_2    = NULL;
	m_pModel2 = NULL;
}
void CFGBGDetector_rgbacc::Clear()
{
	_AWP_SAFE_RELEASE_(m_pCurrentImage);
	_AWP_SAFE_RELEASE_(m_pForeground);
	_AWP_SAFE_RELEASE_(m_pModel);
	_AWP_SAFE_RELEASE_(m_F1);
	_AWP_SAFE_RELEASE_(m_pModel1);
	_AWP_SAFE_RELEASE_(m_F1_1);
	_AWP_SAFE_RELEASE_(m_pModel2);
	_AWP_SAFE_RELEASE_(m_F1_2);
	m_FrameCounter = 0;
}

void CFGBGDetector_rgbacc::FixBackground()
{
	double thr = 0.0035 - 0.0025*this->m_threshold;

	awpImage* ri = NULL;
	awpImage* gi = NULL;
	awpImage* bi = NULL;

	awpGetChannel(this->m_pCurrentImage, &ri, 0);
	awpGetChannel(this->m_pCurrentImage, &gi, 1);
	awpGetChannel(this->m_pCurrentImage, &bi, 2);

	awpImage* norm_r = NULL;
	awpImage* norm_g = NULL;
	awpImage* norm_b = NULL;

	awpNormalize(ri, &norm_r, AWP_NORM_L2);
	awpNormalize(gi, &norm_g, AWP_NORM_L2);
	awpNormalize(bi, &norm_b, AWP_NORM_L2);

	AWPDOUBLE* sr = (AWPDOUBLE*)norm_r->pPixels;
	AWPDOUBLE* sg = (AWPDOUBLE*)norm_g->pPixels;
	AWPDOUBLE* sb = (AWPDOUBLE*)norm_b->pPixels;

	AWPDOUBLE* dr = (AWPDOUBLE*)m_F1->pPixels;
	AWPDOUBLE* dg = (AWPDOUBLE*)m_F1_1->pPixels;
	AWPDOUBLE* db = (AWPDOUBLE*)m_F1_2->pPixels;
	// накопление
	for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
	{
		dr[i] += sr[i];
		dg[i] += sg[i];
		db[i] += sb[i];
	}

	if (m_FrameCounter > m_trainig_t)
	{
		//m_local_t++;
		int t = GetTickCount() - m_local_t;
		if (t > m_delta_t)
		{
			// формирование переднего плана
			m_local_t = GetTickCount();
			awpImage* acc_norm_r = NULL;
			awpImage* acc_norm_g = NULL;
			awpImage* acc_norm_b = NULL;
			awpNormalize(m_F1, &acc_norm_r, AWP_NORM_L2);
			awpNormalize(m_F1_1, &acc_norm_g, AWP_NORM_L2);
			awpNormalize(m_F1_2, &acc_norm_b, AWP_NORM_L2);
			sr = (AWPDOUBLE*)acc_norm_r->pPixels;
			dr = (AWPDOUBLE*)m_pModel->pPixels;
			sg = (AWPDOUBLE*)acc_norm_g->pPixels;
			dg = (AWPDOUBLE*)m_pModel1->pPixels;
			sb = (AWPDOUBLE*)acc_norm_b->pPixels;
			db = (AWPDOUBLE*)m_pModel2->pPixels;
			AWPBYTE* b = (AWPBYTE*)m_pForeground->pPixels;
			for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
			{
				if (fabs(sr[i] - dr[i]) > thr || fabs(sg[i] - dg[i]) > thr || fabs(sb[i] - db[i]) > thr)
					b[i] = 255;
				else
					b[i] = 0;
			}
			awpImage* ti = NULL;
//			awpCopyImage(m_pForeground, &ti);
//			awpGaussianBlur(ti, this->m_pForeground, 3);
//			awpReleaseImage(&ti);
			awpZeroImage(m_F1);
			awpZeroImage(m_F1_1);
			awpZeroImage(m_F1_2);
			_AWP_SAFE_RELEASE_(acc_norm_r);
			_AWP_SAFE_RELEASE_(acc_norm_g);
			_AWP_SAFE_RELEASE_(acc_norm_b);
		}

	}
	else if (m_FrameCounter == m_trainig_t)
	{
		// формирование модели

		m_local_t = GetTickCount();

		_AWP_SAFE_RELEASE_(m_pModel);
		_AWP_SAFE_RELEASE_(m_pModel1);
		_AWP_SAFE_RELEASE_(m_pModel2);

		awpNormalize(m_F1,   &m_pModel,  AWP_NORM_L2);
		awpNormalize(m_F1_1, &m_pModel1, AWP_NORM_L2);
		awpNormalize(m_F1_2, &m_pModel2, AWP_NORM_L2);
	}

	_AWP_SAFE_RELEASE_(norm_r);
	_AWP_SAFE_RELEASE_(norm_g);
	_AWP_SAFE_RELEASE_(norm_b);
	_AWP_SAFE_RELEASE_(ri);
	_AWP_SAFE_RELEASE_(gi);
	_AWP_SAFE_RELEASE_(bi);
}
void CFGBGDetector_rgbacc::EventBackGround()
{
	double thr = 0.0035 - 0.0025*this->m_threshold;

	awpImage* ri = NULL;
	awpImage* gi = NULL;
	awpImage* bi = NULL;

	awpGetChannel(this->m_pCurrentImage, &ri, 0);
	awpGetChannel(this->m_pCurrentImage, &gi, 1);
	awpGetChannel(this->m_pCurrentImage, &bi, 2);

	awpImage* norm_r = NULL;
	awpImage* norm_g = NULL;
	awpImage* norm_b = NULL;

	awpNormalize(ri, &norm_r, AWP_NORM_L2);
	awpNormalize(gi, &norm_g, AWP_NORM_L2);
	awpNormalize(bi, &norm_b, AWP_NORM_L2);

	AWPDOUBLE* sr = (AWPDOUBLE*)norm_r->pPixels;
	AWPDOUBLE* sg = (AWPDOUBLE*)norm_g->pPixels;
	AWPDOUBLE* sb = (AWPDOUBLE*)norm_b->pPixels;

	AWPDOUBLE* dr = (AWPDOUBLE*)m_F1->pPixels;
	AWPDOUBLE* dg = (AWPDOUBLE*)m_F1_1->pPixels;
	AWPDOUBLE* db = (AWPDOUBLE*)m_F1_2->pPixels;
	// накопление
	for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
	{
		dr[i] += sr[i];
		dg[i] += sg[i];
		db[i] += sb[i];
	}

	if (m_FrameCounter > m_trainig_t)
	{
		//m_local_t++;
		int t = GetTickCount() - m_local_t;
		if (t > m_delta_t)
		{
			// формирование переднего плана
			m_local_t = GetTickCount();
			awpImage* acc_norm_r = NULL;
			awpImage* acc_norm_g = NULL;
			awpImage* acc_norm_b = NULL;
			awpNormalize(m_F1, &acc_norm_r, AWP_NORM_L2);
			awpNormalize(m_F1_1, &acc_norm_g, AWP_NORM_L2);
			awpNormalize(m_F1_2, &acc_norm_b, AWP_NORM_L2);
			sr = (AWPDOUBLE*)acc_norm_r->pPixels;
			dr = (AWPDOUBLE*)m_pModel->pPixels;
			sg = (AWPDOUBLE*)acc_norm_g->pPixels;
			dg = (AWPDOUBLE*)m_pModel1->pPixels;
			sb = (AWPDOUBLE*)acc_norm_b->pPixels;
			db = (AWPDOUBLE*)m_pModel2->pPixels;
			AWPBYTE* b = (AWPBYTE*)m_pForeground->pPixels;
			for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
			{
				if (fabs(sr[i] - dr[i]) > thr || fabs(sg[i] - dg[i]) > thr || fabs(sb[i] - db[i]) > thr)
					b[i] = 255;
				else
					b[i] = 0;
			}
			awpImage* ti = NULL;
			//			awpCopyImage(m_pForeground, &ti);
			//			awpGaussianBlur(ti, this->m_pForeground, 3);
			//			awpReleaseImage(&ti);

			_AWP_SAFE_RELEASE_(m_pModel);
			awpNormalize(m_F1, &m_pModel, AWP_NORM_L2);
			_AWP_SAFE_RELEASE_(m_pModel1);
			awpNormalize(m_F1_1, &m_pModel1, AWP_NORM_L2);
			_AWP_SAFE_RELEASE_(m_pModel2);
			awpNormalize(m_F1_2, &m_pModel2, AWP_NORM_L2);


			awpZeroImage(m_F1);
			awpZeroImage(m_F1_1);
			awpZeroImage(m_F1_2);
			_AWP_SAFE_RELEASE_(acc_norm_r);
			_AWP_SAFE_RELEASE_(acc_norm_g);
			_AWP_SAFE_RELEASE_(acc_norm_b);
		}

	}
	else if (m_FrameCounter == m_trainig_t)
	{
		// формирование модели

		m_local_t = GetTickCount();

		_AWP_SAFE_RELEASE_(m_pModel);
		_AWP_SAFE_RELEASE_(m_pModel1);
		_AWP_SAFE_RELEASE_(m_pModel2);

		awpNormalize(m_F1, &m_pModel, AWP_NORM_L2);
		awpNormalize(m_F1_1, &m_pModel1, AWP_NORM_L2);
		awpNormalize(m_F1_2, &m_pModel2, AWP_NORM_L2);
	}

	_AWP_SAFE_RELEASE_(norm_r);
	_AWP_SAFE_RELEASE_(norm_g);
	_AWP_SAFE_RELEASE_(norm_b);
	_AWP_SAFE_RELEASE_(ri);
	_AWP_SAFE_RELEASE_(gi);
	_AWP_SAFE_RELEASE_(bi);
}
void CFGBGDetector_rgbacc::ContBackground()
{
	double thr = 0.0035 - 0.0025*this->m_threshold;

	awpImage* ri = NULL;
	awpImage* gi = NULL;
	awpImage* bi = NULL;

	awpGetChannel(this->m_pCurrentImage, &ri, 0);
	awpGetChannel(this->m_pCurrentImage, &gi, 1);
	awpGetChannel(this->m_pCurrentImage, &bi, 2);

	awpImage* norm_r = NULL;
	awpImage* norm_g = NULL;
	awpImage* norm_b = NULL;

	awpNormalize(ri, &norm_r, AWP_NORM_L2);
	awpNormalize(gi, &norm_g, AWP_NORM_L2);
	awpNormalize(bi, &norm_b, AWP_NORM_L2);

	AWPDOUBLE* sr = (AWPDOUBLE*)norm_r->pPixels;
	AWPDOUBLE* sg = (AWPDOUBLE*)norm_g->pPixels;
	AWPDOUBLE* sb = (AWPDOUBLE*)norm_b->pPixels;

	AWPDOUBLE* dr = (AWPDOUBLE*)m_F1->pPixels;
	AWPDOUBLE* dg = (AWPDOUBLE*)m_F1_1->pPixels;
	AWPDOUBLE* db = (AWPDOUBLE*)m_F1_2->pPixels;

	AWPDOUBLE* mr = (AWPDOUBLE*)this->m_pModel->pPixels;
	AWPDOUBLE* mg = (AWPDOUBLE*)this->m_pModel1->pPixels;
	AWPDOUBLE* mb = (AWPDOUBLE*)this->m_pModel2->pPixels;

	// накопление
	for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
	{
		dr[i] += sr[i];
		dg[i] += sg[i];
		db[i] += sb[i];
		mr[i] += sr[i];
		mg[i] += sg[i];
		mb[i] += sb[i];
	}

	if (m_FrameCounter > m_trainig_t)
	{
		//m_local_t++;
		int t = GetTickCount() - m_local_t;
		if (t > m_delta_t)
		{
			// формирование переднего плана
			m_local_t = GetTickCount();
			awpImage* acc_norm_r = NULL;
			awpImage* acc_norm_g = NULL;
			awpImage* acc_norm_b = NULL;

			awpImage* m_norm_r = NULL;
			awpImage* m_norm_g = NULL;
			awpImage* m_norm_b = NULL;

			awpNormalize(m_F1, &acc_norm_r, AWP_NORM_L2);
			awpNormalize(m_F1_1, &acc_norm_g, AWP_NORM_L2);
			awpNormalize(m_F1_2, &acc_norm_b, AWP_NORM_L2);

			awpNormalize(m_pModel, &m_norm_r, AWP_NORM_L2);
			awpNormalize(m_pModel1, &m_norm_g, AWP_NORM_L2);
			awpNormalize(m_pModel2, &m_norm_b, AWP_NORM_L2);


			sr = (AWPDOUBLE*)acc_norm_r->pPixels;
			dr = (AWPDOUBLE*)m_norm_r->pPixels;
			sg = (AWPDOUBLE*)acc_norm_g->pPixels;
			dg = (AWPDOUBLE*)m_norm_g->pPixels;
			sb = (AWPDOUBLE*)acc_norm_b->pPixels;
			db = (AWPDOUBLE*)m_norm_b->pPixels;
			AWPBYTE* b = (AWPBYTE*)m_pForeground->pPixels;
			for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
			{
				if (fabs(sr[i] - dr[i]) > thr || fabs(sg[i] - dg[i]) > thr || fabs(sb[i] - db[i]) > thr)
					b[i] = 255;
				else
					b[i] = 0;
			}
			//awpImage* ti = NULL;
			//awpCopyImage(m_pForeground, &ti);
			//awpGaussianBlur(ti, this->m_pForeground, 1.5);
			//awpReleaseImage(&ti);

			awpZeroImage(m_F1);
			awpZeroImage(m_F1_1);
			awpZeroImage(m_F1_2);

			_AWP_SAFE_RELEASE_(acc_norm_r);
			_AWP_SAFE_RELEASE_(acc_norm_g);
			_AWP_SAFE_RELEASE_(acc_norm_b);

			_AWP_SAFE_RELEASE_(m_norm_r);
			_AWP_SAFE_RELEASE_(m_norm_g);
			_AWP_SAFE_RELEASE_(m_norm_b);

		}

	}
	else if (m_FrameCounter == m_trainig_t)
	{
		// формирование модели

		m_local_t = GetTickCount();

		_AWP_SAFE_RELEASE_(m_pModel);
		_AWP_SAFE_RELEASE_(m_pModel1);
		_AWP_SAFE_RELEASE_(m_pModel2);

		awpNormalize(m_F1, &m_pModel, AWP_NORM_L2);
		awpNormalize(m_F1_1, &m_pModel1, AWP_NORM_L2);
		awpNormalize(m_F1_2, &m_pModel2, AWP_NORM_L2);
	}

	_AWP_SAFE_RELEASE_(norm_r);
	_AWP_SAFE_RELEASE_(norm_g);
	_AWP_SAFE_RELEASE_(norm_b);
	_AWP_SAFE_RELEASE_(ri);
	_AWP_SAFE_RELEASE_(gi);
	_AWP_SAFE_RELEASE_(bi);
}
void CFGBGDetector_rgbacc::AllocImages(awpImage* src)
{
	Clear();
	m_coef = src->sSizeX / 256;
	awpCreateImage(&this->m_pCurrentImage, src->sSizeX / m_coef, src->sSizeY / m_coef, 3, AWP_BYTE);
	awpCreateImage(&this->m_F1, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_DOUBLE);
	awpCreateImage(&this->m_pForeground, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_BYTE);
	awpCreateImage(&this->m_pModel, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_DOUBLE);

	awpCreateImage(&this->m_pModel1, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_DOUBLE);
	awpCreateImage(&this->m_F1_1, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_DOUBLE);

	awpCreateImage(&this->m_pModel2, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_DOUBLE);
	awpCreateImage(&this->m_F1_2, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_DOUBLE);
}
bool CFGBGDetector_rgbacc::Detect(awpImage* img)
{
	if (m_pModel == NULL || m_pCurrentImage == NULL || m_pForeground == NULL ||
		m_pModel->sSizeX != img->sSizeX / m_coef || m_pModel->sSizeY != img->sSizeY / m_coef)
		this->AllocImages(img);

	awpImage* src = NULL;
	awpCopyImage(img, &src);

	awpResize(src, src->sSizeX / m_coef, src->sSizeY / m_coef);

	awpCopyImage(src, &this->m_pCurrentImage);
	this->m_FrameCounter++;
	this->DataTransform();

	_AWP_SAFE_RELEASE_(src);
	return true;
}

// 
class CBlob : public TLFObject
{
private:
	TVACamera m_Camera;
	TVASize	  m_Size;
protected:
	UUID m_id;
	bool m_isActive;
	int  m_Age;			// число кадров, на которых виден объект
	int  m_Invisible;	// число кадров, на которых объект не виден
	int  m_status;
	awpRect m_Bounds;	// прямоугольник на изображении
	bool m_isPackage;   // потерянная сумка. 
	double  m_dist;		// удаление от камеры. 
	double  m_shift;	// смещение от оптической оси. 
	double  m_height;	// высота
	double  m_width;	// ширина
	void   Clear();
public:
	CBlob(TVACamera& Camera, TVASize& Size);
	void Init(awpRect& r);
	bool IsActive();
	int  GetAge();
	awpRect* GetBounds();
	void SetBounds(awpRect& rect);
	void SetInvisible();
	void Update();
	// возвращает структуру TVAPackageBlob
	TVAPackageBlob GetPackageBlob();
};

CBlob::CBlob(TVACamera& Camera, TVASize& Size)
{
	this->m_Camera = Camera;
	this->m_Size = Size;
	this->m_status = -1;
	Clear();
}
void   CBlob::Clear()
{
	m_isActive = false;
	m_isPackage = false;
	m_Invisible = 0;
	m_Age    = 0;
	m_dist   = 0;		
	m_shift  = 0;	
	m_height = 0;
	m_width  = 0;  
	memset(&this->m_Bounds, 0, sizeof(awpRect));
	memset(&this->m_id, 0, sizeof(UUID));

}
void CBlob::Init(awpRect& r)
{
	Clear();
	UuidCreate(&this->m_id);	
	m_Bounds = r;

	TVARect rect;
	rect.LeftTop.X = r.left;
	rect.LeftTop.Y = r.top;
	rect.RightBottom.X = r.right;
	rect.RightBottom.Y = r.bottom;
	this->m_width   = WH_to_w(&this->m_Camera, &m_Size, &rect);
	this->m_height  = WH_to_h(&this->m_Camera, &m_Size, &rect);
	this->m_dist    = Y_to_d(&this->m_Camera, &m_Size, rect.RightBottom.Y);
	TVAPoint p; 
	p.Y = rect.RightBottom.Y;
	p.X = (rect.LeftTop.X + rect.RightBottom.X) / 2;
	this->m_shift   = XY_to_x(&this->m_Camera, &m_Size, &p);
	this->m_isActive = true;
	this->m_status = 1;
}
bool CBlob::IsActive()
{
	return this->m_isActive;
}
int  CBlob::GetAge()
{
	return this->m_Age;
}
awpRect* CBlob::GetBounds()
{
	return &this->m_Bounds;
}

void CBlob::SetBounds(awpRect& rect)
{
	this->m_Bounds = rect;
}

void CBlob::SetInvisible()
{
	this->m_Invisible++;
	m_Age++;
	this->m_status = 3;
	if (this->m_Invisible > 0)
	{
		Clear();
	}
}
void CBlob::Update()
{
	m_Invisible = 0;
	this->m_status = 2;
}
// возвращает структуру TVAPackageBlob
TVAPackageBlob CBlob::GetPackageBlob()
{
	TVAPackageBlob result;

	result.id	  = this->m_id;
	result.Height = this->m_Bounds.bottom - this->m_Bounds.top;
	result.time   = this->m_Age;
	result.Width  = this->m_Bounds.right - this->m_Bounds.left;
	result.XPos   = (this->m_Bounds.right + this->m_Bounds.left)/2; 
	result.YPos   = (this->m_Bounds.bottom + this->m_Bounds.top)/2;
	result.status = this->m_status;

	if (this->m_status == 3)
		Clear();

	return result;
}

class CBlobList : public TLFObjectList
{
public:
	CBlob* GetBlob(int index);
};

CBlob* CBlobList::GetBlob(int index)
{
	if (index >= 0 && index < this->GetCount())
		return (CBlob*)Get(index);
	else
		return NULL;
}


/// =====================================================================
#define _NUM_BLOBS_ 1000
class CPackage
{
protected:
	CBlobList			m_lf_blobs;
	TVAPackageInit      m_init;
	TVAInitParams		m_params;
	TVASize				m_size;
	CStrokeDetecor		m_stroke;
//	CFGBGDetector_bacc*  m_fgbg;
	CFGBGSwirl*			m_fgbg;
	TLFSabotage			m_Sabotage;
	awpImage*			 m_mask;
	int					 m_sw;
	int					 m_sh;
	int					 m_sbpp;
	int					 m_mode;
	void	Clear();
	void    Restart(int w, int h, int bpp);
	bool	IsPackage(awpRect r);
	CBlob*	HasFreeBlob();

	void  CheckCurrentBlobs(awpImage* img, awpImage* intgr);
public:
	CPackage();
	~CPackage();
	void init(TVAInitParams* params, TVAPackageInit* p);
	bool process(int w, int h, int bpp, unsigned char* data);

	void GetActiveBlobs(int& num, TVAPackageBlob* blobs);

	IFGBGDetector*	GetFGBG();
	CStrokeDetecor* GetStroke();
	TVAPackageInit* GetPackageInitParams();
	TVAInitParams*  GetInitParams();

	int GetMode();
	void SetMode(int value);
};

CPackage::CPackage()
{
	m_mask = NULL;
	m_fgbg = NULL;

	m_sw = 0;
	m_sh = 0;
	m_sbpp = 0;
}

CPackage::~CPackage()
{
	this->Clear();
}
int CPackage::GetMode()
{
	return this->m_mode;
}
void CPackage::SetMode(int value)
{
	this->m_mode = value;
	if (this->m_fgbg != NULL)
		this->m_fgbg->SetMode(value);
}


void CPackage::Clear()
{
//	for (int i = 0; i < _NUM_BLOBS_; i++)
//	{
		//if (m_blobs[i] != NULL)
		//{
			//delete m_blobs[i];
			//m_blobs[i] = NULL;
		//}
	//}
	this->m_lf_blobs.Clear();
	_AWP_SAFE_RELEASE_(m_mask);
	delete m_fgbg;
	this->m_fgbg = NULL;
}

void    CPackage::Restart(int w, int h, int bpp)
{
	Clear();
	int ch = 3;
	if (bpp == 1)
		ch = 1;
	if (m_fgbg == NULL)
	{
		//if (ch == 1)
//			m_fgbg = new CFGBGDetector_bacc();
//		else
			//m_fgbg = new CFGBGDetector_rgbacc();
		m_fgbg = new CFGBGSwirl();
		m_fgbg->SetThreshold(this->GetInitParams()->EventSens);
		m_fgbg->SetDelta_t(this->GetInitParams()->EventTimeSens);
	}

	if (this->m_lf_blobs.GetCount() == 0)
	{
		TVASize s;
		s.width = w / this->m_fgbg->GetCoef();
		s.height = h / this->m_fgbg->GetCoef();
		m_size = s;

		awpCreateImage(&this->m_mask, w, h, 1, AWP_BYTE);
		unsigned char* b = (unsigned char*)m_mask->pPixels;
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				if (IsPointInZone(x, y, &this->m_params))
				{
					for (int i = 0; i < bpp; i++)
					{
						b[x + y*w] = 255;
					}
				}

			}
		}
	}

	this->m_sh = h;
	this->m_sw = w;
	this->m_sbpp = bpp;
}

void CPackage::init(TVAInitParams* params, TVAPackageInit* p)
{
	memcpy(&m_init, p, sizeof(TVAPackageInit));
	memcpy(&m_params, params, sizeof(TVAInitParams));
	// инициализация модуля саботажа. 
	TVAInitParams sparams;
	sparams.NumZones = 0;
	sparams.Zones = NULL;
	sparams.EventSens = 0.5;
	sparams.EventTimeSens = 500;
	this->m_mode = p->Mode;
}
CBlob* CPackage::HasFreeBlob()
{
	return new CBlob(this->m_params.Camera, this->m_size);
}
// проверяет прямоугольник r на соответствие модели 
bool CPackage::IsPackage(awpRect r)
{

	double w,h;
	double width;
	double height;
	double c = this->m_fgbg->GetCoef();

	width  = this->m_size.width*c;
	height = this->m_size.height*c;

	TVARect rect;
	rect.LeftTop.X = r.left*c ;
	rect.LeftTop.Y = r.top*c;
	rect.RightBottom.X = r.right*c;
	rect.RightBottom.Y = r.bottom*c;
	w = rect.RightBottom.X - rect.LeftTop.X;
	h = rect.RightBottom.Y - rect.LeftTop.Y;

	w /= width;
	h /= height;
	w *= 100;
	h *= 100;

	if (w < this->m_init.maxWidth && w > this->m_init.minWidth && 
		h < this->m_init.maxHeight && h > this->m_init.minHeight)
		return true;

	return false;
}

void  CPackage::CheckCurrentBlobs(awpImage* img, awpImage* intgr)
{
	for (int k = m_lf_blobs.GetCount()-1; k >= 0; k--)
	{
		CBlob* blob = m_lf_blobs.GetBlob(k);
		
		if (blob && blob->IsActive())
		{
			awpRect* br = blob->GetBounds();
			double* pix = (double*)intgr->pPixels;
			int w = br->right - br->left;
			int h = br->bottom - br->top;
			int x = br->left;
			int y = br->top;
			if (x < 0 || y < 0 || x + w >= intgr->sSizeX || y + h >= intgr->sSizeY)
			{
				m_lf_blobs.Delete(k);
			}
			else
			{
				double s = CalcSum(pix, br->left, br->top, w, h, intgr->sSizeX) / (w*h + 1);
				if (s == 0)
					m_lf_blobs.Delete(k);
				else
				{
					// зкараска 
					blob->Update();
					awpRect* rr = blob->GetBounds();
					AWPBYTE* b = (AWPBYTE*)img->pPixels;
					if (rr != NULL)
					{
						for (int y = rr->top; y < rr->bottom; y++)
						{
							for (int x = rr->left; x < rr->right; x++)
							{
								b[x + img->sSizeX*y] = 0;
							}
						}

					}
				}
			}
		}
	}
}
int BlobSort(void* item1, void* item2)
{

	CBlob* b1 = (CBlob*)item1;
	CBlob* b2 = (CBlob*)item2;

	awpRect* rect1 = b1->GetBounds();
	awpRect* rect2 = b2->GetBounds();

	int s1 = (rect1->right - rect1->left)*(rect1->bottom - rect1->top);
	int s2 = (rect2->right - rect2->left)*(rect2->bottom - rect2->top);

	if (s1 > s2)
		return 1;
	else if (s1 < s2)
		return -1;
	else
		return 0;
}
//$(BuildDebug)\x64\$(TargetName)$(TargetExt)
bool CPackage::process(int w, int h, int bpp, unsigned char* data)
{
	
	bool res = true;
	
	if (m_sw != w || m_sh != h || m_sbpp != bpp)
	{
		this->Restart(w,h,bpp);
#ifdef _DEBUG
		printf("initial restart.\n");
#endif
		return true;
	}

	int ch = bpp == 1 ? 1 : 3;
	awpImage* img = NULL;
	awpCreateMultiImage(&img, w, h, ch, bpp, data);
	// подготовка изображения 
	unsigned char* b = (unsigned char*) m_mask->pPixels;
	unsigned char* b1 = (unsigned char*) img->pPixels;
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			if (b[x + m_mask->sSizeX*y] == 0)
			{
				{
					b1[x + y*w] = 0;
				}
			}

		}
	}


	bool sr = false;
	int  state = 0;

	//m_Sabotage.PorocessImage(img, &sr);
	state = m_Sabotage.GetState();
	if (state != 0)
	{
#ifdef _DEBUG
		printf("inappropriate image.\n");
#endif
		return false;
	}

	if (sr && state == 0)
	{
		this->Restart(w, h, bpp);
//#ifdef _DEBUG
		printf("Restart by sabotage.\n");
//#endif
		return true;
	}


	this->m_fgbg->SetMode(this->m_mode);
	this->m_fgbg->Detect(img);

	awpImage* fg0 = this->m_fgbg->GetForeground();
	// интегральное изображение. оно нам еще понадобится.
	if (fg0)
	{
		awpImage* intgr = NULL;
		awpImage* fg = NULL;
		awpIntegral(fg0, &intgr, AWP_LINEAR);
		awpCopyImage(fg0, &fg);
		// проверка наличия объектов на прежних местах. 
		this->CheckCurrentBlobs(fg, intgr);
		// на оставшемся изображении находим новые объекты. 
		if (this->m_stroke.Detect(fg) > 0)
		{

			for (int i = 0; i < m_stroke.GetRectsCount(); i++)
			{
				awpRect* rr = m_stroke.GetRect(i);
				// появился новый объект, если его линейные размеры 
				// удовлетворяют модели package, то инициализируем его. 
				if (this->IsPackage(*rr))
				{
					CBlob* b = this->HasFreeBlob();
					if (b != NULL)
					{
						b->Init(*rr);
						m_lf_blobs.Add(b);
					}
				}
			}
		}

		// объединение прямоугольников. 
		this->m_lf_blobs.Sort(BlobSort);
		int c = 0;
		CBlobList objects;
		// устанавливаем метки для удаления 		
		for (int i = 0; i < m_lf_blobs.GetCount(); i++)
		{
			CBlob* b1 = m_lf_blobs.GetBlob(i);
			if (b1->IsActive())
			{
				awpRect* r1 = b1->GetBounds();
				for (int j = i+1; j < m_lf_blobs.GetCount(); j++)
				{
					CBlob* b2 = m_lf_blobs.GetBlob(j);
					awpRect* r2 = b2->GetBounds();
					if (_awpRectsOverlap(*r1, *r2) > 0)
					{
						awpRect sum;
						sum.left = AWP_MIN(r1->left, r2->left);
						sum.top = AWP_MIN(r1->top, r2->top);
						sum.right = AWP_MAX(r1->right, r2->right);
						sum.bottom = AWP_MAX(r1->bottom, r2->bottom);
						b1->SetBounds(sum);
						b2->SetInvisible();
					}
				}
			}
		}
		// удаление
		for (int i = m_lf_blobs.GetCount() - 1; i >= 1; i--)
		{
			CBlob* b1 = m_lf_blobs.GetBlob(i);
			if (!b1->IsActive())
				m_lf_blobs.Delete(i);
		}

		_AWP_SAFE_RELEASE_(intgr);
		_AWP_SAFE_RELEASE_(fg);
	}
	_AWP_SAFE_RELEASE_(img);
	return 0;
}

IFGBGDetector* CPackage::GetFGBG()
{
	return this->m_fgbg;
}

CStrokeDetecor* CPackage::GetStroke()
{
	return &this->m_stroke;
}

TVAPackageInit* CPackage::GetPackageInitParams()
{
	return &this->m_init;
}

TVAInitParams*  CPackage::GetInitParams()
{
	return &this->m_params;
}

void CPackage::GetActiveBlobs(int& num, TVAPackageBlob* blobs)
{
	int c = 0;
	if (m_lf_blobs.GetCount() > 0)
	{
		int count = m_lf_blobs.GetCount();
		if (num < count)
			count = num-1;
		for (int i = 0; i < count; i++)
		{
			CBlob* blob = m_lf_blobs.GetBlob(i);
			if (blob && blob->IsActive())
			{
				blobs[c] = blob->GetPackageBlob();
				blobs[c].Height *= this->m_fgbg->GetCoef();
				blobs[c].Width  *= this->m_fgbg->GetCoef();
				blobs[c].XPos   *= this->m_fgbg->GetCoef();
				blobs[c].YPos   *= this->m_fgbg->GetCoef();
				c++;
			}
		}
	}

	num = c;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct 
{
	int size;
	CPackage* p;
	TLFPackageEngine* e;
	TVAInitParams params;
	TVAPackageInit  params1;
}ThePackage;

// Экспортируемые функции. 
PACKAGE_API HANDLE		packageCreate(TVAInitParams* params, TVAPackageInit* params1)
{
	ThePackage* p = new ThePackage();
	p->size = sizeof(ThePackage);
	memcpy(&p->params, params, sizeof(TVAInitParams));
	memcpy(&p->params1, params1, sizeof(TVAPackageInit));
	p->p = new CPackage();
	if (p->p == NULL)
	{
		return NULL;
	}
	//p->p->init(params, params1);
	p->e = new TLFPackageEngine();
	if (p->e == NULL)
	{
		delete p;
		return NULL;
	}

	p->e->SetNeedCluster(true);
	p->e->SetNeedTrack(true);
	p->e->SetResize(true);
	p->e->SetBaseImageWidth(256);
	p->e->SetAverageBufferSize(200);
	p->e->SetBgStability(5);

	// 
	int delay = p->params.EventTimeSens * 10 / 1000;

	p->e->SetDelay(delay);
	
	p->e->SetMinHeight(p->params1.minHeight);
	p->e->SetMinWidth(p->params1.minWidth);
	p->e->SetMaxHeight(p->params1.maxHeight);
	p->e->SetMaxWidth(p->params1.maxWidth);

	ILFObjectDetector* d = NULL;
	d = p->e->GetDetector(0);
	if (d != NULL)
	{
		d->SetBaseHeight(4);
		d->SetBaseWidht(4);
		d->SetThreshold(p->params.EventSens);
	}

	p->e->SetMaxSize(params1->maxHeight*params1->maxWidth);
	p->e->SetMinSize(params1->minHeight*params1->minWidth);

	return (HANDLE)p;
}
PACKAGE_API HRESULT		packageProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAPackageResult* result)
{
	ThePackage* p = (ThePackage*)hModule;
	if (p->size != sizeof(ThePackage))
		return E_FAIL;
	HRESULT res = S_OK;
	awpImage* tmp = NULL;
	awpCreateGrayImage(&tmp, width, height, bpp, data);
#ifdef _DEBUG 
	awpSaveImage("out.awp", tmp);
#endif 
	p->e->SetSourceImage(tmp, true);
	int n = p->e->GetItemsCount();
	result->num = n < p->params1.numObects ? n : p->params1.numObects;
	//printf("num = %d\n", n);
	for (int i = 0; i < result->num; i++)
	{
		TLFDetectedItem*  di = p->e->GetItem(i);
		if (di != NULL)
		{
			awpRect rr = di->GetBounds()->GetRect();
			UUID id;
			di->GetId(id);
			memcpy(&result->objects[i].id, &id, sizeof(UUID));
			result->objects[i].XPos = rr.left;
			result->objects[i].YPos = rr.top;
			result->objects[i].Width = rr.right - rr.left;
			result->objects[i].Height = rr.bottom - rr.top;
			result->objects[i].status = di->GetState();
		}
	}

	awpReleaseImage(&tmp);

	return S_OK;

/*	ThePackage* p = (ThePackage*)hModule;
	if (p->size != sizeof(ThePackage))
		return E_FAIL;
	HRESULT res = S_OK;
	
	p->p->process(width, height, bpp, data);

	int num = 0;
	TVAPackageInit* params = p->p->GetPackageInitParams();
	TVAPackageBlob* blobs = (TVAPackageBlob*)malloc(params->numObects*sizeof(TVAPackageBlob));
	num = params->numObects;
	p->p->GetActiveBlobs(num, blobs);
	result->num = num;
	
	if (num > 0)
	{
		for (int i = 0; i < num; i++)
		{
			memcpy(&result->objects[i], &blobs[i], sizeof(TVAPackageBlob));
		}
	}
	
	free(blobs);

	return res;*/
}
PACKAGE_API HRESULT		packageForeground(HANDLE hModule, int width, int height, unsigned char* data)
{
	ThePackage* p = (ThePackage*)hModule;
	if (p->size != sizeof(ThePackage))
		return E_FAIL;
	
	awpImage* img = p->e->GetForeground();//->GetFGBG()->GetForeground();
	if (img != NULL)
	{
		awpImage* tmp = NULL;
		awpCopyImage(img, &tmp);

		if (tmp->sSizeX != width || tmp->sSizeY != height)
			awpResize(tmp, width, height);
		memcpy(data, tmp->pPixels, width*height*sizeof(unsigned char));
		_AWP_SAFE_RELEASE_(tmp);
	}

	return S_OK;
}
PACKAGE_API HRESULT		packageRelease (HANDLE* hModule )
{
	ThePackage* p = (ThePackage*)hModule;
	if (p->size != sizeof(ThePackage))
		return E_FAIL;

	delete p->p;
	delete p->e;
	delete p;
	p = NULL;

	return S_OK;
}