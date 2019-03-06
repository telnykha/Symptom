#include "_LF.h"
#include "LFFireModule.h"	



TLFFireEngine::TLFFireEngine()
{
	TLFFireDetector* d = new TLFFireDetector();
	this->m_detectors.Add(d);
	m_strPredictorName = "TLFNullPredictor";
	m_buffer = new TLFBuffer(50, 0);
	m_push_count = 0;
	m_State = 0;
	m_threshold = 0.5;
}

TLFFireEngine::~TLFFireEngine()
{
	delete m_buffer;
}
void TLFFireEngine::Clear()
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
}
void TLFFireEngine::OverlapsFilter(TLFSemanticImageDescriptor* descriptor)
{

}
void TLFFireEngine::InitDetectors()
{
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	if (d != NULL)
	{
		awpImage* img = m_SourceImage.GetImage();
		d->Init(img);
	}
}
bool TLFFireEngine::LoadXML(TiXmlElement* parent)
{
	return false;
}

double TLFFireEngine::GetThreshold()
{
	return this->m_threshold;
}

void TLFFireEngine::SetThreshold(double value)
{
	if (value >= 0 && value <= 1)
		this->m_threshold = value;
}

bool TLFFireEngine::FindObjects()
{
	if (this->m_SourceImage.GetImage() == NULL)
		return false;
	if (this->m_SourceImage.GetImage()->dwType != AWP_BYTE)
		return false;
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);

	if (!d)
		return false;

	m_tmpList.Clear();
	m_result.Clear();

	d->Detect();
	m_tmpList.Clear();
	if (d->GetNumItems() > 0)
	{
		for (int i = 0; i < d->GetNumItems(); i++)
		{
			TLFDetectedItem* item = d->GetItem(i);
			if (item != NULL && item->HasObject())
			{
				TLFDetectedItem* di = new TLFDetectedItem(item);
				di->Resize(m_resizeCoef);
				m_tmpList.Add(di);
			}
		}
	}

	{
		m_result.Clear();
		for (int i = 0; i < m_tmpList.GetCount(); i++)
		{
			TLFDetectedItem* di = (TLFDetectedItem*)m_tmpList.Get(i);
			m_result.AddDetectedItem(di);
		}
		m_tmpList.Clear();
	}

	double t = 15 + m_threshold * 10;

	if (this->GetItemsCount() > t)
		this->m_State = 1;
	else
		this->m_State = 0;
	return true;
}


TiXmlElement*  TLFFireEngine::SaveXML()
{
	return NULL;
}
bool TLFFireEngine::DetectInRect(awpRect* rect)
{
	return false;
}
