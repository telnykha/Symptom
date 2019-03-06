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

	if (awpGetStrokes(img, &num, &str, 128, NULL) != AWP_OK)
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
class IFGBGDetrector
{
protected:
	double	  m_threshold;
	int		  m_FrameCounter;
	int		  m_coef;

	int		  m_delta_t;
	int		  m_local_t;
	int		  m_trainig_t;

	awpImage* m_pCurrentImage;
	awpImage* m_pForeground;

	virtual void LBPtransform(awpImage* src, awpImage* dst) = 0;
	virtual void AllocImages(awpImage* src)  = 0;
	virtual void Clear() = 0;

public:
	IFGBGDetrector();

	virtual bool Detect(awpImage* img) = 0;
	// Получение данных 
	awpImage* GetLastIamge();
	awpImage* GetForeground();
	double	  GetThreshold();
	int		  GetFrameCounter();
	int		  GetCoef();
	int		  GetDelta_t();
	// Установка параметров
	void SetThreshold(double v);
	void SetDelta_t(int v);
};
IFGBGDetrector::IFGBGDetrector()
{
	m_threshold = 0.5;
	m_FrameCounter = 0;
	m_coef = 4;

	m_delta_t = 5000;
	m_local_t = 0;
	m_trainig_t = 50;

	m_pCurrentImage = NULL;
	m_pForeground   = NULL;
}
awpImage* IFGBGDetrector::GetLastIamge()
{
	return this->m_pCurrentImage;
}
awpImage* IFGBGDetrector::GetForeground()
{
	return this->m_pForeground;
}
double	  IFGBGDetrector::GetThreshold()
{
	return this->m_threshold;
}
int		  IFGBGDetrector::GetFrameCounter()
{
	return this->m_FrameCounter;
}
int		  IFGBGDetrector::GetCoef()
{
	return this->m_coef;
}
int		  IFGBGDetrector::GetDelta_t()
{
	return this->m_delta_t;
}
void IFGBGDetrector::SetThreshold(double v)
{
	if (v >= 0 && v <= 1)
		this->m_threshold = v;
}
void IFGBGDetrector::SetDelta_t(int v)
{
	if (v >= 0 && v <= 600000)
	{
		this->m_delta_t = v;
		//this->m_trainig_t = this->m_delta_t;
	}
}


// Детектор оставленных объектов, основанный на распределении LBP признака в пикселе
class CFGBGDetector : public IFGBGDetrector
{
protected:
	awpImage* m_F1;
	awpImage* m_pModel;
protected:
	virtual void LBPtransform(awpImage* src, awpImage* dst);
	virtual void AllocImages(awpImage* src);
	virtual void Clear();
public:
	CFGBGDetector();
	~CFGBGDetector();

	// action
	virtual bool Detect(awpImage* img);
};
CFGBGDetector::CFGBGDetector() : IFGBGDetrector()
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
	this->LBPtransform(this->m_pCurrentImage, this->m_F1);
	

	_AWP_SAFE_RELEASE_(src);
	return true;
}
void CFGBGDetector::LBPtransform(awpImage* src, awpImage* dst)
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
	awpCreateImage(&this->m_pCurrentImage, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_BYTE);
	awpCreateImage(&this->m_F1, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_BYTE);
	awpCreateImage(&this->m_pForeground, src->sSizeX / m_coef, src->sSizeY / m_coef, 1, AWP_BYTE);
	awpCreateImage(&this->m_pModel, src->sSizeX / m_coef, src->sSizeY / m_coef, 255, AWP_DOUBLE);
}

// детектор оставленных объектов, основанный на аккамуляторе яркости. 
class CFGBGDetector_bacc : public CFGBGDetector
{
protected:
	virtual void LBPtransform(awpImage* src, awpImage* dst);
	virtual void AllocImages(awpImage* src);
public:
	CFGBGDetector_bacc();
	virtual bool Detect(awpImage* img);
};

CFGBGDetector_bacc::CFGBGDetector_bacc() :CFGBGDetector()
{
	m_delta_t = 250;
	m_trainig_t = 50;
	m_local_t = 0;
} 
void CFGBGDetector_bacc::LBPtransform(awpImage* src, awpImage* dst)
{
	double thr = 0.003 - 0.0025*this->m_threshold;

	
	awpImage* norm = NULL;
	awpNormalize(src, &norm, AWP_NORM_L2);

	AWPDOUBLE* s = (AWPDOUBLE*)norm->pPixels;
	AWPDOUBLE* d = (AWPDOUBLE*)dst->pPixels;
	// накопление
	for (int i = 0; i < dst->sSizeX*dst->sSizeY; i++)
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
	this->LBPtransform(this->m_pCurrentImage, this->m_F1);

	_AWP_SAFE_RELEASE_(src1);
	_AWP_SAFE_RELEASE_(src);
	return true;
}

// детектор оставленных объектов, основанный на аккамуляторе яркости цветовых компонент rgb.
class CFGBGDetector_rgbacc : public CFGBGDetector_bacc
{
protected:
	awpImage* m_F1_1;
	awpImage* m_pModel1;
	awpImage* m_F1_2;
	awpImage* m_pModel2;
	virtual void LBPtransform(awpImage* src, awpImage* dst);
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
void CFGBGDetector_rgbacc::LBPtransform(awpImage* src, awpImage* dst)
{
   
	double thr = 0.0035 - 0.0025*this->m_threshold;

	awpImage* ri = NULL;
	awpImage* gi = NULL;
	awpImage* bi = NULL;

	awpGetChannel(src, &ri, 0);
	awpGetChannel(src, &gi, 1);
	awpGetChannel(src, &bi, 2);

	awpImage* norm_r = NULL;
	awpImage* norm_g = NULL;
	awpImage* norm_b = NULL;

	awpImage* norm_mr = NULL;
	awpImage* norm_mg = NULL;
	awpImage* norm_mb = NULL;


	awpNormalize(ri, &norm_r, AWP_NORM_L2);
	awpNormalize(gi, &norm_g, AWP_NORM_L2);
	awpNormalize(bi, &norm_b, AWP_NORM_L2);

	AWPDOUBLE* sr = (AWPDOUBLE*)norm_r->pPixels;
	AWPDOUBLE* sg = (AWPDOUBLE*)norm_g->pPixels;
	AWPDOUBLE* sb = (AWPDOUBLE*)norm_b->pPixels;

	AWPDOUBLE* dr = (AWPDOUBLE*)m_F1->pPixels;
	AWPDOUBLE* dg = (AWPDOUBLE*)m_F1_1->pPixels;
	AWPDOUBLE* db = (AWPDOUBLE*)m_F1_2->pPixels;

	AWPDOUBLE* mr = (AWPDOUBLE*)m_pModel->pPixels;
	AWPDOUBLE* mg = (AWPDOUBLE*)m_pModel->pPixels;
	AWPDOUBLE* mb = (AWPDOUBLE*)m_pModel->pPixels;

	// накопление
	for (int i = 0; i < dst->sSizeX*dst->sSizeY; i++)
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

			awpNormalize(m_F1, &acc_norm_r, AWP_NORM_L2);
			awpNormalize(m_F1_1, &acc_norm_g, AWP_NORM_L2);
			awpNormalize(m_F1_2, &acc_norm_b, AWP_NORM_L2);

			awpNormalize(m_pModel, &norm_mr, AWP_NORM_L2);
			awpNormalize(m_pModel1, &norm_mg, AWP_NORM_L2);
			awpNormalize(m_pModel2, &norm_mb, AWP_NORM_L2);


			sr = (AWPDOUBLE*)acc_norm_r->pPixels;
			dr = (AWPDOUBLE*)norm_mr->pPixels;
			sg = (AWPDOUBLE*)acc_norm_g->pPixels;
			dg = (AWPDOUBLE*)norm_mg->pPixels;
			sb = (AWPDOUBLE*)acc_norm_b->pPixels;
			db = (AWPDOUBLE*)norm_mb->pPixels;

			AWPBYTE* b = (AWPBYTE*)m_pForeground->pPixels;
			for (int i = 0; i < m_F1->sSizeX*m_F1->sSizeY; i++)
			{
				if (fabs(sr[i] - dr[i]) > thr || fabs(sg[i] - dg[i]) > thr || fabs(sb[i] - db[i]) > thr)
					b[i] = 255;
				else
					b[i] = 0;
			}

//			awpImage* ti = NULL;
			//awpCopyImage(m_pForeground, &ti);
			//awpGaussianBlur(ti, this->m_pForeground, 1);
			//awpReleaseImage(&ti);

			awpZeroImage(m_F1);
			awpZeroImage(m_F1_1);
			awpZeroImage(m_F1_2);
			_AWP_SAFE_RELEASE_(acc_norm_r);
			_AWP_SAFE_RELEASE_(acc_norm_g);
			_AWP_SAFE_RELEASE_(acc_norm_b);

			_AWP_SAFE_RELEASE_(norm_mr);
			_AWP_SAFE_RELEASE_(norm_mg);
			_AWP_SAFE_RELEASE_(norm_mb);

		}

	}
	else if (m_FrameCounter == m_trainig_t)
	{
		// формирование модели

		m_local_t = GetTickCount();

//		_AWP_SAFE_RELEASE_(m_pModel);
//		_AWP_SAFE_RELEASE_(m_pModel1);
//		_AWP_SAFE_RELEASE_(m_pModel2);

//		awpNormalize(m_F1,   &m_pModel, AWP_NORM_L2);
//		awpNormalize(m_F1_1, &m_pModel1, AWP_NORM_L2);
//		awpNormalize(m_F1_2, &m_pModel2, AWP_NORM_L2);
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
	this->LBPtransform(this->m_pCurrentImage, this->m_F1);

	_AWP_SAFE_RELEASE_(src);
	return true;
}

// 
class CBlob 
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
void CBlob::SetInvisible()
{
	this->m_Invisible++;
	m_Age++;
	this->m_status = 3;
	if (this->m_Invisible > 1)
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

/// =====================================================================
#define _NUM_BLOBS_ 1000
class CPackage
{
protected:
	CBlob*				m_blobs[_NUM_BLOBS_];
	TVAPackageInit      m_init;
	TVAInitParams		m_params;
	TVASize				m_size;
	CStrokeDetecor		m_stroke;
	CFGBGDetector_bacc*  m_fgbg;
	awpImage*			 m_mask;
	int					 m_sw;
	int					 m_sh;
	int					 m_sbpp;

	HANDLE				 m_sabotage;
	TLFSabotage			 m_lfsabotage;
	void	Clear();
	void    Restart(int w, int h, int bpp);
	bool	IsPackage(awpRect r);
	CBlob*	HasFreeBlob();
public:
	CPackage();
	~CPackage();
	void init(TVAInitParams* params, TVAPackageInit* p);
	bool process(int w, int h, int bpp, unsigned char* data);

	void GetActiveBlobs(int& num, TVAPackageBlob* blobs);

	CFGBGDetector*	GetFGBG();
	CStrokeDetecor* GetStroke();
	TVAPackageInit* GetPackageInitParams();
	TVAInitParams*  GetInitParams();
};

CPackage::CPackage()
{
	m_mask = NULL;
	for (int i = 0; i < _NUM_BLOBS_; i++)
		m_blobs[i] = NULL;
	m_fgbg = NULL;
	m_sw = 0;
	m_sh = 0;
	m_sbpp = 0;
	m_sabotage = NULL;
}

CPackage::~CPackage()
{
	this->Clear();
	if (m_sabotage != NULL)
		sabotageRelease(&this->m_sabotage);
}
void CPackage::Clear()
{
	for (int i = 0; i < _NUM_BLOBS_; i++)
	{
		if (m_blobs[i] != NULL)
		{
			delete m_blobs[i];
			m_blobs[i] = NULL;
		}
	}
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
		if (ch == 1)
			m_fgbg = new CFGBGDetector_bacc();
		else
			m_fgbg = new CFGBGDetector_rgbacc();
		m_fgbg->SetThreshold(this->GetInitParams()->EventSens);
		m_fgbg->SetDelta_t(this->GetInitParams()->EventTimeSens);
	}

	if (this->m_blobs[0] == NULL)
	{
		TVASize s;
		s.width = w / this->m_fgbg->GetCoef();
		s.height = h / this->m_fgbg->GetCoef();
		m_size = s;
		for (int i = 0; i < _NUM_BLOBS_; i++)
		{
			m_blobs[i] = new CBlob(this->m_params.Camera, s);
		}

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

	m_sabotage = sabotageCreate(&sparams);
}
CBlob* CPackage::HasFreeBlob()
{
	for (int i = 0; i < _NUM_BLOBS_; i++)
		if (!m_blobs[i]->IsActive())
			return m_blobs[i];
	return NULL;
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

	bool sr = false;
	int  state = 0;
	m_lfsabotage.PorocessImage(img, &sr);
	state = m_lfsabotage.GetState();

	if (state != 0)
	{
#ifdef _DEBUG
		printf("inappropriate image.\n");
#endif
//		printf("inappropriate image.\n");
		return false;
	}

	if (sr && state == 0)
	{
		this->Restart(w, h, bpp);
#ifdef _DEBUG
		printf("Restart by sabotage.\n");
#endif
//		printf("Restart by sabotage.\n");
		return true;
	}


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

#ifdef _DEBUG
	awpSaveImage("out.awp", img);
#endif
	this->m_fgbg->Detect(img);

	awpImage* fg = this->m_fgbg->GetForeground();
	// интегральное изображение. оно нам еще понадобится.
	if (fg)
	{
		awpImage* intgr = NULL;
		awpIntegral(fg, &intgr, AWP_LINEAR);

		if (this->m_stroke.Detect(fg) > 0)
		{

			for (int i = 0; i < m_stroke.GetRectsCount(); i++)
			{
				awpRect* rr = m_stroke.GetRect(i);
				if (rr != NULL)
				{
					bool tracked = false;
					for (int k = 0; k < m_init.numObects; k++)
					{
						if (m_blobs[k]->IsActive())
						{
							awpRect* br = m_blobs[k]->GetBounds();
							// если найденный прямоугольник и прямоугольник
							// активного объекта пересекаются
							if (_awpRectsOverlap(*rr,*br) > 0)
							{
								// проверим прямоугольник активного объекта
								// если проверка прошла, то  устанавливаем 
								// флаг tracked 
								double* pix = (double*)intgr->pPixels;
								int w = br->right - br->left;
								int h = br->bottom - br->top;
								int x = br->left;
								int y = br->top;
								if (x < 0 || y < 0 || x + w >= intgr->sSizeX || y + h >= intgr->sSizeY)
								{
									m_blobs[k]->SetInvisible();
									tracked = true;
									break;
								}
								else
								{
									double s =  CalcSum(pix, br->left, br->top, w, h, intgr->sSizeX) / (w*h + 1);
									if (s > 0)
									{

										m_blobs[k]->Update();
									}
									else
									{
										m_blobs[k]->SetInvisible();
									}
									tracked = true;
									break;
								}
							}
						}
					}

					if (!tracked)
					{
						// появился новый объект, если его линейные размеры 
						// удовлетворяют модели package, то инициализируем его. 
						if (this->IsPackage(*rr))
						{
							CBlob* b = this->HasFreeBlob();
							if (b != NULL)
							{
								b->Init(*rr);
							}
						}	
					}
						
				}
			}

		}

		for (int k = 0; k < m_init.numObects; k++)
		{
			if (m_blobs[k]->IsActive())
			{
				awpRect* br = m_blobs[k]->GetBounds();
				double* pix = (double*)intgr->pPixels;
				int w = br->right - br->left;
				int h = br->bottom - br->top;
				int x = br->left;
				int y = br->top;
				if (x < 0 || y < 0 || x + w >= intgr->sSizeX || y + h >= intgr->sSizeY)
				{
					m_blobs[k]->SetInvisible();
				}
				else
				{
					double s =  CalcSum(pix, br->left, br->top, w, h, intgr->sSizeX) / (w*h + 1);
					if (s == 0)
						m_blobs[k]->SetInvisible();
				}
			}
		}
		awpReleaseImage(&intgr);
	}
	_AWP_SAFE_RELEASE_(img);
	return 0;
}

CFGBGDetector* CPackage::GetFGBG()
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
	for (int i = 0; i < this->m_init.numObects; i++)
	{
		if (this->m_blobs[i]->IsActive())
			c++;
	}
	num = c;

	if (num > 0)
	{
		c = 0;
		for (int i = 0; i < this->m_init.numObects; i++)
		{
			if (this->m_blobs[i]->IsActive())
			{
				blobs[c] = this->m_blobs[i]->GetPackageBlob();
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

typedef struct 
{
	int size;
	CPackage* p;
}ThePackage;

// Экспортируемые функции. 
PACKAGE_API HANDLE		packageCreate(TVAInitParams* params, TVAPackageInit* params1)
{
	ThePackage* p = new ThePackage();
	p->size = sizeof(ThePackage);
	p->p = new CPackage();
	//p->p = new TLFPackage(6, 6, params->EventSens, params->EventTimeSens);
	if (p->p == NULL)
	{
		delete p;
		return NULL;
	}
	p->p->init(params, params1);

	return (HANDLE)p;
}
PACKAGE_API HRESULT		packageProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAPackageResult* result)
{
	ThePackage* p = (ThePackage*)hModule;
	if (p->size != sizeof(ThePackage))
		return E_FAIL;
	HRESULT res = S_OK;
	/*
	awpImage* tmp = NULL;
	int ch = bpp == 1 ? 1 : 3;
	if (awpCreateMultiImage(&tmp, width, height, ch, bpp, data) != AWP_OK)
		return E_FAIL;
	if (!p->p->PorocessImage(tmp))
		res = E_FAIL;
	awpReleaseImage(&tmp);

	if (p->p->GetItemsCount() > 0)
	{
		result->num = 0;
		for (int i = 0; i < p->p->GetItemsCount(); i++)
		{
			TLFDetectedItem* di = p->p->GetItem(i);
			if (di != NULL)
			{
				awpRect rect = di->GetBounds()->GetRect();
				result->num++;
				UuidCreate(&result->objects[i].id);
				result->objects[i].status = 1;
				result->objects[i].XPos = rect.left;
				result->objects[i].YPos = rect.top;
				result->objects[i].Height = rect.bottom - rect.top;
				result->objects[i].Width = rect.right - rect.left;
			}
		}
	}
	*/
	
	p->p->process(width, height, bpp, data);

	int num = 0;
	TVAPackageInit* params = p->p->GetPackageInitParams();
	TVAPackageBlob* blobs = (TVAPackageBlob*)malloc(params->numObects*sizeof(TVAPackageBlob));
	
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

	return res;
}
PACKAGE_API HRESULT		packageForeground(HANDLE hModule, int width, int height, unsigned char* data)
{
	ThePackage* p = (ThePackage*)hModule;
	if (p->size != sizeof(ThePackage))
		return E_FAIL;
	
	awpImage* img = p->p->GetFGBG()->GetForeground();
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
	delete p;
	p = NULL;

	return S_OK;
}