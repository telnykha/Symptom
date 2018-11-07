// counter.cpp: определяет экспортированные функции для приложения DLL.
//
#include "va_common.h"
#include "vautils.h"
#include "_LF.h"
#include "Counter.h"
#include "lftcounter.h"

#include "zones.cpp"
TVAZone g_zones[2] = 
{
	{
		true,
		{ {10,10}, {50,90}},
		0, 
		NULL
	},
	{
		true,
		{ { 5, 5 }, { 95, 95 } },
		0, 
		NULL
	}
};
TVAInitParams g_params = {
	false,//bool	  SaveLog;		// для ревизора должен быть всегда установлен в false	
	"\0",// char*	  Path;			// для ревизора должне быть установлен в ""
	true, 
	{
		0,//double Height;		//Высота установки в миллиметрах
		0,//double Angle;		//Угол наклона камеры в градусах
		0,//double Focus;		//Фокусное расстояние камеры в миллиметрах
		0,//double WChipSize;	//Ширина сенсора в миллиметрах. 
		0,//double HChipSize;	//Высота сенсора в миллиметрах. 
		0,//double dMaxLenght;
	},
	//TVACamera Camera;		// параметры камеры видеонаблюдения. 
	2,//int		  NumZones;		// число областей наблюдения на кадре
	g_zones,// TVAZone*  Zones;		// области наблюдения. 
	0.5,//double    EventSens;    // чувствительноность. 0...1; 0 - минимальная, 1 - максимальная, по умолчанию 0.5
	0,//double	  EventTimeSens;// время наступления события в миллисекундах, по умолчанию 1000  
	0,//int       numObects;	// максимальное число объектов наблюдения. 
};


AWPRESULT awpMatrixTranspose(awpImage* inMatrix, awpImage* outMatrix)
{
	if (inMatrix == NULL || outMatrix == NULL)
		return AWP_BADARG;
	if (inMatrix->bChannels != 1 || outMatrix->bChannels != 1)
		return AWP_BADARG;
	if (inMatrix->dwType != AWP_DOUBLE || outMatrix->dwType != AWP_DOUBLE)
		return AWP_BADARG;
	if (inMatrix->sSizeX != outMatrix->sSizeY || inMatrix->sSizeY != outMatrix->sSizeX)
		return AWP_BADARG;

	return AWP_OK;
}

AWPRESULT awpMatrixDeterminant(awpImage* inMatrix, AWPDOUBLE* det)
{
	return AWP_OK; 
}

AWPRESULT awpMatrixMultiplication(awpImage* src1, awpImage* src2, awpImage* out)
{
	return AWP_OK;
}

typedef struct tagCStat2D
{
	double avg1;
	double disp1;
	double sigma1;
	double min1;
	double max1;
	double avg2;
	double disp2;
	double sigma2;
	double min2;
	double max2;
	double corr;
}CStat2D;
const CStat2D Classes2D[4] = { {0.128,0.00055,0.023,0.105,0.235,0.0075,6.4e-5,0.0080,0.0003,0.036,0.587}, 
							   {0.370,0.00839,0.091,0.250,0.625,0.2, 0.005,0.072,0.08,0.34,0.69},
							   {0.491,0.01570,0.125,0.295,0.725,0.326,0.01592,0.1262,0.154,0.609,0.71}, 
							   {0.671,0.01274,0.112,0.530,0.815,0.632,0.0462,0.2150,0.4185,0.897,0.72} };

const double normx = 200;
const double normy = 6000;
double Gauss2D(double x, double y, CStat2D stat)
{
	x /= normx;
	y /= normy;
	double result = 0;
	double alfa = 1.0 / (2 * AWP_PI*sqrt(1 - stat.corr*stat.corr));
	result = exp(-((x - stat.avg1)*(x - stat.avg1)/stat.disp1 - 2*stat.corr*(x-stat.avg1)*(y-stat.avg2)/(stat.sigma1*stat.sigma2)+(y-stat.avg2)*(y-stat.avg2)/stat.disp2) / (2 * (1 - stat.corr*stat.corr)));
	result *= alfa;
	return result;
}

int GetCount2D(double x, double y)
{
	double ver[4];
	double max = 0;
	int index = 0;
	for (int i = 0; i < 4; i++)
	{ 
		ver[i] = Gauss2D(x, y, Classes2D[i]);
		if (ver[i] > max)
		{
			max = ver[i];
			index = i;
		}
	}
	return index;
}

class CCounter : public ILFDetectEngine
{
protected:
	TLFFGBGDetector		detector;
	TLFImage 			m_foreground;
	TVAInitParams		m_params;
	TLFImage			m_mask;
	TLFAFeature*		m_pAFeature;
	double				m_aValue;
	double				m_hValue;
	int					m_in_count;
	int					m_out_count;
	double				m_start_value;
	std::vector<double> m_hdata;
	std::vector<awpPoint> m_trajectory;
	double				m_aintegral;

	awpPoint		    m_startPoint;
	awpPoint			m_endPoint;
	int				    m_finish_counter;
	bool				m_has_trajectory;

	DWORD				m_dt;
	DWORD				m_event_start_time;
	TLFObjectList		m_event_data;

	void				AddEventData();
	int					EventGetCount();
	TLFImage			m_event_image;

	virtual void InitDetectors();
	virtual bool FindObjects();
	virtual void OverlapsFilter(TLFSemanticImageDescriptor* descriptor){};
	void		 BuildForeground();
	void		 GetStartStopStates(double& v1, double& v2);
	awpPoint     GetCentroid(double& width);
public:
	CCounter(TVAInitParams& params);
	virtual ~CCounter();
	virtual void Clear();
	awpImage* GetForeground();
	awpImage* GetEventImage()
	{
		return this->m_event_image.GetImage();
	}

	void SetAverageBufferSize(int value);
	int GetAverageBufferSize();

	void SetBGStability(int value);
	int GetBGStability();

	double AValue()
	{
		return m_aValue;
	}
	double HValue()
	{
		return m_hValue;
	}

	int InCount()
	{
		return this->m_in_count;
	}
	int OutCount()
	{
		return this->m_out_count;
	}

	awpPoint GetStartPoint()
	{
		return this->m_startPoint;
	}
	awpPoint GetEndPoint()
	{
		return this->m_endPoint;
	}
	bool GetHasTrajectory()
	{
		return this->m_has_trajectory;
	}

	/*Loading from-to xml files*/
	virtual bool LoadXML(TiXmlElement* parent)
	{
		return false;
	}
	virtual TiXmlElement* SaveXML()
	{
		return NULL;
	}
};

CCounter::CCounter(TVAInitParams& params)
{
	TLFFGBGDetector* d = new TLFFGBGDetector();
	this->m_detectors.Add(d);
	m_strPredictorName = "TLFNullPredictor";
	
	memcpy(&m_params, &params, sizeof(TVAInitParams));

	m_params.NumZones = 2;
	m_params.Zones = (TVAZone*)malloc(m_params.NumZones*sizeof(TVAZone));
	for (int i = 0; i < 2; i++)
	{
		m_params.Zones[i].IsRect = params.Zones[i].IsRect;
		if (m_params.Zones[i].IsRect)
		{
			m_params.Zones[i].NumPoints = 0;
			m_params.Zones[i].Points = NULL;
			m_params.Zones[i].Rect = params.Zones[i].Rect;
		}
		else
		{
			m_params.Zones[i].NumPoints = params.Zones[i].NumPoints;
			m_params.Zones[i].Points = (TVAPoint*)malloc(m_params.Zones[i].NumPoints*sizeof(TVAPoint));
			memcpy(m_params.Zones[i].Points, params.Zones[i].Points, m_params.Zones[i].NumPoints*sizeof(TVAPoint));
		}
	}

	m_pAFeature = NULL;
	m_in_count = 0;
	m_out_count = 0;
	m_start_value = 0;
	m_has_trajectory = false;
	m_dt = 0;
	m_event_start_time = 0;
}

CCounter::~CCounter()
{
	if (m_pAFeature != NULL)
		delete m_pAFeature;

	if (m_params.NumZones > 0)
	{
		for (int i = 0; i < m_params.NumZones; i++)
		{
			if (!m_params.Zones[i].IsRect)
			{
				free(m_params.Zones[i].Points);
				m_params.Zones[i].Points = NULL;
				m_params.Zones[i].NumPoints = 0;
			}
		}
		free(m_params.Zones);
		m_params.Zones = NULL;
		m_params.NumZones = 0;
	}
}

void CCounter::Clear()
{
	this->m_result.Clear();
	this->m_tmp_result.Clear();
	for (int i = 0; i < this->m_detectors.GetCount(); i++)
	{
		ILFObjectDetector* d = dynamic_cast<ILFObjectDetector*>(m_detectors.Get(i));
		if (d)
		{
			d->Clear();
		}
	}

	m_in_count = 0;
	m_out_count = 0;
	m_start_value = 0;
	m_has_trajectory = false;
}
void CCounter::BuildForeground()
{
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	if (!d)
		return;

	awpImage* fg = NULL;
	TLFImage* img = d->GetImage();

	awpCreateImage(&fg, img->GetImage()->sSizeX, img->GetImage()->sSizeY, 1, AWP_BYTE);
	m_foreground.SetImage(fg);
	awpReleaseImage(&fg);

	if (d->GetNumItems() > 0)
	{
		for (int i = 0; i < d->GetNumItems(); i++)
		{
			TLFDetectedItem* item = d->GetItem(i);
			if (item != NULL && item->HasObject())
			{
				awpRect    item_rect = item->GetBounds()->GetRect();
				awpImage*  fg = m_foreground.GetImage();
				awpFillRect(fg, &item_rect, 0, 255);
			}
		}
	}

	this->m_aValue = 100.* m_pAFeature->fCalcValue(&m_foreground) / 255.;
	this->m_hValue = 0;

	if (m_aValue > 0 && this->m_State == 0) // начало события. 
	{
		m_State = 1;
		m_aintegral = m_aValue;
		this->m_event_start_time = LFGetTickCount();
		m_finish_counter = 0;
		m_hdata.clear();
		this->m_start_value = m_hValue;
		m_trajectory.clear();
		m_event_data.Clear();
	}
	else if (m_aValue == 0 && m_State == 1) // завершение события. 
	{
		
		m_finish_counter++;
		//if (m_finish_counter > 20)
		if (m_trajectory.size() > 0)
		{
			m_finish_counter = 0;
			int count = 0;

			count = this->EventGetCount();

			//if (m_hdata.size() > 30 && m_hdata.size() <= 90)
			//	count = 1;
			//else if (m_hdata.size() > 90 && m_hdata.size() <= 124)
			//	count = 2;
			//else if (m_hdata.size() > 124)
			//	count = 3;
			
			if (m_trajectory[0].Y > m_trajectory[m_trajectory.size() - 1].Y)
				this->m_in_count += count;
			else
				this->m_out_count += count;
			
			if (count > 0)
			{
				this->m_startPoint = m_trajectory[0];
				this->m_endPoint = m_trajectory[m_trajectory.size() - 1];
				this->m_has_trajectory = true;
			}

			printf("start %d end %d count %d size %d\n", m_startPoint.Y, m_endPoint.Y, count, m_hdata.size());
		}
		m_State = 0;
	}

	if (m_State == 1) // внутри события. 
	{
		m_aintegral += m_aValue;
		m_hdata.push_back(m_hValue);
		if (m_aValue > 0.2)
		{
			awpPoint p = GetCentroid(this->m_hValue);
			m_trajectory.push_back(p);

		}
		this->AddEventData();
	}
}

awpPoint     CCounter::GetCentroid(double& width)
{
	awpPoint result;
	result.X = 0;
	result.Y = 0;

	awpImage *img = m_foreground.GetImage();
	if (img == NULL)
		return result;
	awpRect rect = m_pAFeature->GetRect();
	AWPBYTE* b = (AWPBYTE*)img->pPixels;
	AWPDOUBLE sum, x0, y0;
	sum = 0;
	x0 = 0;
	y0 = 0;
	double min_x = rect.right;
	double max_x = rect.left;
	for (int y = rect.top; y < rect.bottom; y++)
	{
		for (int x = rect.left; x < rect.right; x++)
		{
			sum += b[y*img->sSizeX + x];
			x0 += x*b[y*img->sSizeX + x];
			y0 += y*b[y*img->sSizeX + x];
			if (b[y*img->sSizeX + x] > 0)
			{
				if (min_x > x)
					min_x = x;
				if (max_x < x)
					max_x = x;
			}
		}
	}
	if (sum == 0)
		return result;
	result.X = (AWPSHORT)floor(x0 / (double)sum + 0.5);
	result.Y = (AWPSHORT)floor(y0 / (double)sum + 0.5);
	width = max_x - min_x;
	return result;
}
void CCounter::GetStartStopStates(double& v1, double& v2)
{
	v1 = 0;
	v2 = 0;
	if (m_hdata.size() == 0)
		return;
	double lenght = (double)(m_hdata.size()) / 2.0;
	for (int i = 0; i < m_hdata.size() / 2; i++)
		v1 += m_hdata[i];
	for (int i = m_hdata.size() / 2; i < m_hdata.size(); i++)
		v2 += m_hdata[i];
	v1 /= lenght;
	v2 /= lenght;
}

bool CCounter::FindObjects()
{
	if (this->m_SourceImage.GetImage() == NULL)
		return false;
	if (this->m_SourceImage.GetImage()->dwType != AWP_BYTE)
		return false;
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	if (!d)
		return false;

	d->Detect();
	this->BuildForeground();
	return true;
}

//double CCounter::GetThreshold()
//{
//	return this->m_threshold;
//}
//void CCounter::SetThreshold(double value)
//{
//	this->m_threshold = value;
//}
void CCounter::InitDetectors()
{
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	if (d != NULL)
	{
		awpImage* img = m_SourceImage.GetImage();
		
		if (this->m_mask.GetImage() == NULL)
		{
			// make mask 
			awpImage* mask = NULL;
			awpCreateImage(&mask, img->sSizeX, img->sSizeY, 1, AWP_BYTE);
			HANDLE image = (HANDLE)mask;
			MakeMaskImage(image, &m_params);
			this->m_mask.SetImage(mask);

			int w = mask->sSizeX;
			int h = mask->sSizeY;
			int x = 0; 
			int y = 0;


			x = (int)floor(w*m_params.Zones[0].Rect.LeftTop.X / 100. + 0.5);
			y = (int)floor(h*m_params.Zones[0].Rect.LeftTop.Y / 100. + 0.5);
			int _w = (int)floor(w*(m_params.Zones[0].Rect.RightBottom.X - m_params.Zones[0].Rect.LeftTop.X) / 100. + 0.5);
			int _h = (int)floor(h*(m_params.Zones[0].Rect.RightBottom.Y - m_params.Zones[0].Rect.LeftTop.Y) / 100. + 0.5) - 1;

			if (_w >= mask->sSizeX) _w = mask->sSizeX - 1;
			if (_h >= mask->sSizeY) _h = mask->sSizeY - 1;

			m_pAFeature = new TLFAFeature(x, y, _w, _h);

			_AWP_SAFE_RELEASE_(mask)
		}
		// apply mask
		AWPBYTE* a = (AWPBYTE*)img->pPixels;
		AWPBYTE* b = (AWPBYTE*)(m_mask.GetImage()->pPixels);
		for (int i = 0; i < img->sSizeX*img->sSizeY; i++)
			a[i] = b[i]*a[i];

		d->Init(img);
	}
}
awpImage* CCounter::GetForeground()
{
	return this->m_foreground.GetImage();
}

void CCounter::SetAverageBufferSize(int value)
{
	if (this->GetDetector() != NULL)
	{
		TLFFGBGDetector* d = (TLFFGBGDetector*)this->GetDetector();
		d->SetAverageBufferSize(value);
	}
}
int CCounter::GetAverageBufferSize()
{
	if (this->GetDetector() != NULL)
	{
		TLFFGBGDetector* d = (TLFFGBGDetector*)this->GetDetector();
		return d->GetAverageBufferSize();
	}
	else
		return 0;
}

void CCounter::SetBGStability(int value)
{
	if (this->GetDetector() != NULL)
	{
		TLFFGBGDetector* d = (TLFFGBGDetector*)this->GetDetector();
		d->SetBgStability(value);
	}
}
int CCounter::GetBGStability()
{
	if (this->GetDetector() != NULL)
	{
		TLFFGBGDetector* d = (TLFFGBGDetector*)this->GetDetector();
		return d->GetBgStability();
	}
	else
		return 0;
}

void CCounter::AddEventData()
{
  	awpImage* fg = this->GetForeground();
	if (fg == NULL)
		return;
	AWPBYTE* b = (AWPBYTE*)fg->pPixels;

	int ImageWidth = fg->sSizeX;
	int ImageHeight = fg->sSizeY;
	int sx = ImageWidth*this->m_params.Zones[1].Rect.LeftTop.X / 100;
	int ex = ImageWidth*this->m_params.Zones[1].Rect.RightBottom.X / 100;
	int sy = ImageHeight*this->m_params.Zones[1].Rect.LeftTop.Y / 100;
	int ey = ImageHeight*this->m_params.Zones[1].Rect.RightBottom.Y / 100;


	int size = ex - sx + 1;
	DWORD dt = LFGetTickCount() - m_event_start_time;
	m_event_start_time += dt;
	TLFBuffer* buffer = new TLFBuffer(size, 0);
	buffer->Push((double)dt);
	for (int x = sx; x < ex; x++)
	{
		double sum = 0;
		for (int y = sy; y < ey; y++)
		{
			sum += b[y*ImageWidth + x];
		}
		buffer->Push(sum);
	}
	this->m_event_data.Add(buffer);
}
int	 CCounter::EventGetCount()
{
	// 
	if (m_event_data.GetCount() < 10)
	{
		m_event_image.FreeImages();
		return 0;
	}

	awpImage* fg = this->GetForeground();
	if (fg == NULL)
		return 0;
	AWPBYTE* b = (AWPBYTE*)fg->pPixels;

	int ImageWidth = fg->sSizeX;
	int ImageHeight = fg->sSizeY;

	int sx = ImageWidth*this->m_params.Zones[1].Rect.LeftTop.X / 100;
	int ex = ImageWidth*this->m_params.Zones[1].Rect.RightBottom.X / 100;
	int sy = ImageHeight*this->m_params.Zones[1].Rect.LeftTop.Y / 100;
	int ey = ImageHeight*this->m_params.Zones[1].Rect.RightBottom.Y / 100;
	double t = 64 * (ey - sy);
	double w = ex - sx;

	TLFBuffer* buffer = (TLFBuffer*)m_event_data.Get(0);
	ImageWidth = buffer->GetSize() - 1;
	ImageHeight = 0;
	for (int i = 0; i < m_event_data.GetCount(); i++)
	{
		TLFBuffer* buffer = (TLFBuffer*)m_event_data.Get(i);
		double dy = buffer->GetValue(0) / 50. < 1 ? 1 : buffer->GetValue(0) / 50.;
		ImageHeight += (int)floor(dy + 0.5);
	}
	awpImage* event_img = NULL;
	awpCreateImage(&event_img, ImageWidth, ImageHeight, 1, AWP_DOUBLE);
	AWPDOUBLE* data = (AWPDOUBLE*)event_img->pPixels;
	int y = 0;
	double s = 0;
	for (int i = 0; i < m_event_data.GetCount(); i++)
	{
		TLFBuffer* buffer = (TLFBuffer*)m_event_data.Get(i);
		double dy = buffer->GetValue(0) / 50. < 1 ? 1 : buffer->GetValue(0) / 50.;
		int count = (int)floor(dy + 0.5);
		int c = 0;
		while (c < count)
		{
			for (int j = 1; j < buffer->GetSize(); j++)
			{
				data[y*ImageWidth + j-1] = buffer->GetValue(j);
				if (data[y*ImageWidth + j - 1] > t)
					s++;
			}
			c++;
			y++;
		}
	}


	
	this->m_event_image.SetImage(event_img);
	awpReleaseImage(&event_img);
	double k = 25;
	return floor(s/(k*w) + 0.5);
}



typedef struct
{
	int size;
//	TVAInitParams params;
//	awpImage*  mask;
	CCounter*  counter;
}TheCounter;

COUNTER_API HANDLE   counterCreate(TVAInitParams* params)
{
	if (params == NULL)
		return NULL;
	bool create_zone = false;
	if (params->NumZones == 0)
	{
		create_zone = true;
	}
	else
		if (params->NumZones != 2 && params->Zones[0].IsRect == false)
		return NULL;

	TheCounter* hcounter = new TheCounter();
	hcounter->size = sizeof(TheCounter);
	hcounter->counter = new CCounter(*params);
	hcounter->counter->SetNeedTrack(false);
	hcounter->counter->SetNeedCluster(false);
	hcounter->counter->SetResize(true);
	hcounter->counter->SetBaseImageWidth(320);
	hcounter->counter->SetAverageBufferSize(5);
	hcounter->counter->SetBGStability(1);
	ILFObjectDetector* s = hcounter->counter->GetDetector(0);
	s->SetBaseWidht(8);
	s->SetBaseHeight(8);
	//s->SetThreshold(params->EventSens);
	s->SetThreshold(0.4);

	return (HANDLE)hcounter;
}
COUNTER_API HRESULT  counterProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, double& value1, double& value2, int& state)
{
	HANDLE module = hModule;
	TheCounter* hcounter = (TheCounter*)module;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;

	awpImage* tmp = NULL;
	awpCreateGrayImage(&tmp, width, height, bpp, data);

	hcounter->counter->SetSourceImage(tmp, true);
	value1 = hcounter->counter->AValue();
	value2 = hcounter->counter->HValue();
	state = hcounter->counter->GetState();
	awpReleaseImage(&tmp);
	return S_OK;
}
COUNTER_API HRESULT  counterGetCount(HANDLE hModule, int* in_count, int* out_count)
{
	HANDLE module = hModule;
	TheCounter* hcounter = (TheCounter*)module;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;
	*in_count = hcounter->counter->InCount();
	*out_count = hcounter->counter->OutCount();
	return S_OK;
}
COUNTER_API HRESULT  counterGetLastTrajectory(HANDLE hModule, TVAPoint* startPoint, TVAPoint* endPoint, bool* has_trajectory)
{
	HANDLE module = hModule;
	TheCounter* hcounter = (TheCounter*)module;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;
	*has_trajectory = false;
	startPoint->X = 0;
	startPoint->Y = 0;
	endPoint->X = 0;
	endPoint->Y = 0;
	if (hcounter->counter->GetHasTrajectory())
	{
		*has_trajectory = true;
		double rc = hcounter->counter->GetResizeCoef();
		awpPoint p = hcounter->counter->GetStartPoint();
		startPoint->X = p.X*rc;
		startPoint->Y = p.Y*rc;
		p = hcounter->counter->GetEndPoint();
		endPoint->X = p.X*rc;
		endPoint->Y = p.Y*rc;
	}
//	*in_count = hcounter->counter->InCount();
//	*out_count = hcounter->counter->OutCount();
	return S_OK;
}
COUNTER_API HRESULT  counterRestart(HANDLE hModule)
{
	HANDLE module = hModule;
	TheCounter* hcounter = (TheCounter*)module;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;
	hcounter->counter->Clear();
	return S_OK;
}
COUNTER_API HRESULT  counterRelease(HANDLE* hModule)
{
	HANDLE module = *hModule;
	TheCounter* hcounter = (TheCounter*)module;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;

	//_AWP_SAFE_RELEASE_(hcounter->mask)
	
	delete hcounter->counter;
	hcounter->counter = NULL;

	delete hcounter;

	*hModule = NULL;
	return S_OK;
}

COUNTER_API HRESULT	 counterForeground(HANDLE hModule, int width, int height, unsigned char* data)
{
	TheCounter*  hcounter  = (TheCounter*)hModule;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;

	awpImage* img = hcounter->counter->GetForeground();
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

COUNTER_API HRESULT  counterGetEventImageSize(HANDLE hModule, int& width, int& height)
{
	TheCounter*  hcounter = (TheCounter*)hModule;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;
	width = 0;
	height = 0;
	awpImage* img = hcounter->counter->GetEventImage();
	if (img != NULL)
	{
		width = img->sSizeX;
		height = img->sSizeY;
	}
	return S_OK;
}
COUNTER_API HRESULT  counterGetEventImage(HANDLE hModule, int width, int height, double* data)
{
	TheCounter*  hcounter = (TheCounter*)hModule;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;

	awpImage* img = hcounter->counter->GetEventImage();
	if (img != NULL)
	{
		if (width != img->sSizeX)
			return E_FAIL;

		if (height != img->sSizeY)
			return E_FAIL;
		
		if (data == NULL)
			return E_FAIL;

		memcpy(data, img->pPixels, width*height*sizeof(double));
	}
	return S_OK;

}

COUNTER_API TVAInitParams* counterGetDefaultParams()
{
	return &g_params;
}

//---------------------------------------
const int tcmagic = 1980588;
typedef struct
{
	int				size;
	int				magic;
	TLFTCounter*	counter;
}TheTCounter;

COUNTER_API HANDLE   tcounterCreate(TVAPoint start, TVAPoint finish, TVARect sizes, double eventSens)
{
	if (eventSens < 0 || eventSens > 1)
		return NULL;
	if (start.X < 0 || start.X >100 || start.Y < 0 || start.Y > 100)
		return NULL;
	if (finish.X < 0 || finish.X >100 || finish.Y < 0 || finish.Y > 100)
		return NULL;
	TVAPoint s = sizes.LeftTop;
	if (s.X < 0 || s.X >100 || s.Y < 0 || s.Y > 100)
		return NULL;
	s = sizes.RightBottom;
	if (s.X < 0 || s.X >100 || s.Y < 0 || s.Y > 100)
		return NULL;

	TheTCounter* c = new TheTCounter();
	c->size  = sizeof(TheTCounter);
	c->magic = tcmagic;
	c->counter = new TLFTCounter(start, finish, sizes, eventSens);

	return (HANDLE)c;
}

COUNTER_API HRESULT  tcounterProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, double& value)
{
	HANDLE module = hModule;
	TheTCounter* c = (TheTCounter*)module;
	if (c->size != sizeof(TheTCounter))
		return E_FAIL;

	if (c->magic != tcmagic)
		return E_FAIL;
	if (c->counter == NULL)
		return E_FAIL;

	awpImage* tmp = NULL;
	if (awpCreateGrayImage(&tmp, width, height, bpp, data) != S_OK)
		return E_FAIL;
	int num_in  = 0;
	int num_out = 0;
	value = 0;
	HRESULT res = S_OK;
	double s = 0;
	if (c->counter->ProcessImage(tmp, num_in, num_out, s))
	{
		value = num_in + num_out;
	}
	else
		res = E_FAIL;
	awpReleaseImage(&tmp);
	return res;
}

COUNTER_API HRESULT  tcounterRelease(HANDLE* hModule)
{
	HANDLE module = *hModule;
	TheTCounter* c = (TheTCounter*)module;
	if (c->size != sizeof(TheTCounter))
		return E_FAIL;
	if (c->magic != tcmagic)
		return E_FAIL;
	if (c->counter == NULL)
		return E_FAIL;

	delete c->counter;
	c->counter = NULL;

	delete c;

	*hModule = NULL;
	return S_OK;
}
