// sabotage.cpp : Defines the exported functions for the DLL application.
//

#include "math.h"
#include "sabotage.h"
#include <stdio.h>

#include "_LF.h"

class TSabotage
{
protected:
	double m_minBr;
	double m_maxBr;
	double m_DeltaBr;
	double m_minSg;
	double m_maxSg;
	double m_DeltaSg;
	double m_minEn;
	double m_maxEn;
	double m_DeltaEn;

	double m_hst[256];
	double m_br;
	double m_sg;
	double m_en;

	double m_t;
	double m_s;

	bool    m_first_frame;
	DWORD   m_event_time;
	DWORD   m_event_start;

	int width;
	int height;
	int bpp;
	int state;

	int InternalProcess(int w, int h, int bpp, unsigned char* data, double* br, double* sg, double* en);
public:
	TSabotage(double s, double t);
	~TSabotage();

	void process(int w, int h, int bpp, unsigned char* data, bool* result);
	int GetState();
};

TSabotage::TSabotage(double s, double t)
{
	this->m_s = s;
	this->m_t = t;
	this->m_event_time = 0;

	this->width = 0;
	this->height = 0;
	this->bpp   = 0;

	this->m_first_frame = true;
	double delta;

	this->m_maxBr = 0.6 + (1-m_s)*0.3;
	this->m_minBr = 0.4 - (1-m_s)*0.3;
	this->m_DeltaBr = 0.07;
	
	this->m_minSg = 0.4 - (1-m_s)*0.3;
	this->m_maxSg = 0.6 + (1-m_s)*0.3;
	this->m_DeltaSg = 0.1;

	this->m_minEn = 0.5 - (1-m_s)*0.3;
	this->m_maxEn = 0.7 + (1-m_s)*0.3;
	this->m_DeltaEn = 0.1;

	this->m_br = (this->m_maxBr - this->m_minBr) /2;
	this->m_sg = (this->m_maxSg - this->m_minSg) /2;
	this->m_en = (this->m_maxEn - this->m_minEn) /2;

	this->state = 0;

	memset(this->m_hst, 0, sizeof(this->m_hst));
}
TSabotage::~TSabotage()
{

}
void TSabotage::process(int w, int h, int bpp, unsigned char* data, bool* result)
{
	*result = false;
	double br,sg,en;
	br = 0; sg = 0; en = 0;

	if (this->m_first_frame)
	{
		this->width = w;
		this->height = h;
		this->bpp   = bpp;
		this->m_first_frame = false;
		this->InternalProcess(w,h,bpp,data,&this->m_br,&this->m_sg,&this->m_en);
		return;
	}

	if (w != this->width || h != this->height || this->bpp != bpp)
	{
		this->m_first_frame = true;
		return;
	}

	// обработка
	int st = this->InternalProcess(w,h,bpp,data, &br,&sg,&en);
	//printf("%i\n", st);
	if (st != this->state)
	{
		if (this->m_event_time == 0)
		{
			this->m_event_start = LFGetTickCount();
			m_event_time = 1;
		}
		else
		{
			m_event_time = LFGetTickCount() - m_event_start;
		}

		if (m_event_time > this->m_t)
		{
			this->state = st;
			*result = true;
		}
		else
			*result = false;
	}
	else
	{
		this->m_event_time = 0;
		*result = false;
	}

	/*
	// анализ
	if ((br > this->m_maxBr || br < this->m_minBr) ||
		(sg < this->m_minSg || sg > this->m_maxSg) ||
		(en > this->m_maxEn || en < this->m_minEn))
	{

		if (this->m_event_time == 0)
		{
			this->m_event_start = GetTickCount();
			m_event_time = 1;
		}
		else
			m_event_time = GetTickCount() - m_event_start;

		if (m_event_time > this->m_t)
		{
#ifdef _DEBUG
			printf("----------------SABOTAGE!!!------------------------------\n");
#endif
			*result = true;
		}

		*result = true;
	}
	else
		this->m_event_time = 0;

#ifdef _DEBUG
	printf("%f\t%f\t%f%d\n", br, m_minSg, en, m_event_time);
#endif
	// завершение. 
	this->m_br = br;
	this->m_sg = sg;
	this->m_en = en;
*/
}

static double _Log2(double value)
{
	return log(value) / log(2.f);
}
/*
	анализ входного изображения для получения его интегральных параметров. 
	1. Яркости
	2. Дисперсии
	3. Энтропии. 

	ЭТО ВАЖНО!!!
	Мы рассматриваем исходный массив данных таким образом, что строки выровнены на 1 (ОДИН) байт. 
	
*/
int TSabotage::InternalProcess(int w, int h, int bpp, unsigned char* data, double* br, double* sg, double* en)
{
	memset(this->m_hst, 0, sizeof(this->m_hst));
	// получаем гистограмму прореженного изображения. 
	for (int y = 0; y < h; y += 4)
	{
		for (int x = 0; x < w; x += 4)
		{
			double Value = 0;
			for (int j = 0; j < bpp; j++)
			{
				Value += data[bpp*(y*w + x + j)];
			}
			Value /= bpp;
			int index = (int)Value;
			this->m_hst[index]++;
		}
	}
	// вычисляем среднюю яркость и дисперсию. 
	*br = 0;
	*sg = 0;
	double s = w*h / 16;
	for (int i = 0; i < 256; i++)
	{
		this->m_hst[i] /= s;
		*br += i*this->m_hst[i];
		*sg += i*i*this->m_hst[i];
	}

	*sg -= (*br)*(*br);
	*sg = sqrt(*sg);
	*br /= 255;
	*sg /= 100;
	// вычисление энтропии

	double d = 0;
	double hh = 0;
	for (int i = 0; i < 256; i++)
		d += m_hst[i];

	double v = 0;
	for (int i = 0; i < 256; i++)
	{
		v = m_hst[i] / d;
		if (m_hst[i] > 0)
			hh -= v*_Log2(v);
	}
	*en =  hh;
	*en /= 10;
	int result = 0;

	if (*br < this->m_minBr)
		result = 1;
	if (*br > this->m_maxBr)
		result = 2;
	if (*sg < this->m_minSg)
		result = 3;
	if (*en < this->m_minEn)
		result = 4;

	if (result == 0 && (fabs(m_br - *br) > this->m_DeltaBr ||
		fabs(m_sg - *sg) > this->m_DeltaSg ||
		fabs(m_en - *en) > this->m_DeltaEn))
		result = 5;


	if (result < 5)
	{
		this->m_br = *br;
		this->m_sg = *sg;
		this->m_en = *en;
	}
	printf("%f\t%f\t%f%d\n", *br, *sg, *en, m_event_time);
	return result;
}

int TSabotage::GetState()
{
	return this->state;
}

typedef struct 
{
	int size;
	TLFSabotage* s;

}TheSabotage;

SABOTAGE_API HANDLE		sabotageCreate(TVAInitParams* params)
{
	TheSabotage* ts = new TheSabotage();
	if (ts == NULL)
		return NULL;
	ts->size = sizeof(TheSabotage);
	ts->s = new TLFSabotage(params->EventSens, params->EventTimeSens);
	if (ts->s == NULL)
	{
		delete ts;
		return NULL;
	}
	
	return (HANDLE)ts;
}
SABOTAGE_API HRESULT	sabotageProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, bool* result)
{
	TheSabotage* ts = (TheSabotage*)hModule;
	if (ts->size != sizeof(TheSabotage))
		return E_FAIL;
	//todo: convert to awp image 
	HRESULT res = S_OK;
	awpImage* tmp = NULL;
	if (awpCreateGrayImage(&tmp, width, height, bpp, data) != AWP_OK)
		return E_FAIL;
	if (!ts->s->PorocessImage(tmp, result))
		res = E_FAIL;
	awpReleaseImage(&tmp);
	return res;
}

SABOTAGE_API HRESULT	sabotageState(HANDLE hModule, int* state)
{
	TheSabotage* ts = (TheSabotage*)hModule;
	if (ts->size != sizeof(TheSabotage))
		return E_FAIL;
	*state = ts->s->GetState();
	return S_OK;
}
SABOTAGE_API HRESULT    sabotageRelease (HANDLE* hModule )

{
	HANDLE module = *hModule;
	TheSabotage* ts = (TheSabotage*)module;
	if (ts->size != sizeof(TheSabotage))
		return E_FAIL;

	delete ts->s;
	delete ts;
	ts = NULL;

	*hModule = NULL;
	return S_OK;
}

