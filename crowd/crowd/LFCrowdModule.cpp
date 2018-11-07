#include "LFCrowdModule.h"
#include "zones.cpp"
CCrowdEngine::CCrowdEngine(TVAInitParams& params)
{
	TLFFGBGDetector* d = new TLFFGBGDetector();
	this->m_detectors.Add(d);
	m_strPredictorName = "TLFNullPredictor";
	m_State = 0;
	m_threshold = params.EventSens;
	TLFZones* z = ConvertVAParamsToZones(params);
	m_params.EventSens = params.EventSens;
	m_params.EventTimeSens = params.EventTimeSens;
	m_params.NumZones = 0;
	m_params.Zones = NULL;
	m_objectSquare = new TLFZone(*z->GetZone(0));
	z->Delete(0);
	this->SetZones(z);
	delete z;

	m_sum_square = 0;
	m_num_frames = 0;

}
CCrowdEngine::~CCrowdEngine()
{
	delete m_objectSquare;
}
void CCrowdEngine::Clear()
{
	this->m_tmpList.Clear();
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
	m_sum_square = 0;
	m_num_frames = 0;
}

awpImage* CCrowdEngine::GetForeground()
{
	return this->m_foreground.GetImage();
}
// возвращает эталонную площадь объекта для подсчета, 
// полученную из параметров инициализации модуля. 
double	CCrowdEngine::SourceSquare()
{
	awpImage* img = this->m_SourceImage.GetImage();
	if (img == NULL || m_objectSquare == NULL)
		return -1;
	int w = img->sSizeX;
	int h = img->sSizeY;

	TLF2DRect* rect = m_objectSquare->GetRect();
	if (rect == NULL)
		return -1;

	double wr = w*rect->Width() / 100.;
	double hr = h*rect->Height() / 100.;

	return wr*hr;
}

// возвращает площадь для сравнения с площадью
// найденного объекта. 
double CCrowdEngine::GetS0()
{
	double ss = SourceSquare();

	if (m_params.EventTimeSens < 0)
		return ss;
	if (m_params.EventTimeSens == 0 && m_num_frames > 100)
		return 	MeanSquare();
	else if (m_params.EventTimeSens == 0 && m_num_frames < 100)
		return ss;

	if (m_params.EventTimeSens > m_num_frames)
		return ss;
	else
		return MeanSquare();
}


bool CCrowdEngine::LoadXML(TiXmlElement* parent)
{
	return false;
}

void    CCrowdEngine::BuildForeground()
{
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	if (!d)
		return;

	awpImage* fg = NULL;
	TLFImage* img = d->GetImage();

	awpCreateImage(&fg, img->GetImage()->sSizeX, img->GetImage()->sSizeY, 1, AWP_BYTE);
	m_foreground.SetImage(fg);
	awpReleaseImage(&fg);
	int count = 0;
	d->Detect();
	m_tmpList.Clear();
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

				TLFDetectedItem* di = new TLFDetectedItem(item);
				di->Resize(m_resizeCoef);
				m_tmpList.Add(di);
				count++;
			}
		}
	}
}

bool CCrowdEngine::FindObjects()
{
	if (this->m_SourceImage.GetImage() == NULL)
		return false;
	if (this->m_SourceImage.GetImage()->dwType != AWP_BYTE)
		return false;

	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	if (!d)
		return false;

	awpImage* fg = NULL;
	TLFImage* img = d->GetImage();

	awpCreateImage(&fg, img->GetImage()->sSizeX, img->GetImage()->sSizeY, 1, AWP_BYTE);
	m_foreground.SetImage(fg);
	awpReleaseImage(&fg);
	int count = 0;

	d->Detect();
	m_result.Clear();
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

				TLFDetectedItem* di = new TLFDetectedItem(item);
				di->Resize(m_resizeCoef);
				m_result.Add(di);
				count++;
			}
		}
	}
	return true;
}
TiXmlElement*  CCrowdEngine::SaveXML()
{
	return NULL;
}
bool CCrowdEngine::DetectInRect(awpRect* rect)
{
	return false;
}
double CCrowdEngine::GetThreshold()
{
	return this->m_threshold;
}
void CCrowdEngine::SetThreshold(double value)
{
	this->m_threshold = value;
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	if (d != NULL)
		d->SetThreshold(this->m_threshold);
}
void CCrowdEngine::OverlapsFilter(TLFSemanticImageDescriptor* descriptor)
{

}
void CCrowdEngine::InitDetectors()
{
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	if (d != NULL)
	{
		awpImage* img = m_SourceImage.GetImage();
		d->Init(img);
	}
}

TVAInitParams* CCrowdEngine::GetParams()
{
	return &this->m_params;
}

void CCrowdEngine::AddNewSquare(double s)
{
	m_sum_square += s;
	m_num_frames++;
}

double	CCrowdEngine::MeanSquare()
{
	if (m_num_frames == 0)
		return -2;
	return m_sum_square / (double)m_num_frames;
}

