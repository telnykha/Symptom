#include "smoke.h"
#include "_LF.h"
#include "LFSmokeModule.h"
////////////////////////////////////////////////////////////////////////////////
TLFSmokeEngine::TLFSmokeEngine()
{
	TLFSmokeDetector* d = new TLFSmokeDetector();
	this->m_detectors.Add(d);
	m_strPredictorName = "TLFNullPredictor";

}
void TLFSmokeEngine::Clear()
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
void TLFSmokeEngine::OverlapsFilter(TLFSemanticImageDescriptor* descriptor)
{

}
void TLFSmokeEngine::InitDetectors()
{
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	if (d != NULL)
	{
		awpImage* img = m_SourceImage.GetImage();
		d->Init(img);
	}
}
bool TLFSmokeEngine::LoadXML(TiXmlElement* parent)
{
	return false;
}
bool TLFSmokeEngine::FindObjects()
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

	//	if (!this->m_needCluster)
	{
		m_result.Clear();
		for (int i = 0; i < m_tmpList.GetCount(); i++)
		{
			TLFDetectedItem* di = (TLFDetectedItem*)m_tmpList.Get(i);
			m_result.AddDetectedItem(di);
		}
		m_tmpList.Clear();
	}
	//else
	//	OverlapsFilter(&this->m_result);
	return true;
}
TiXmlElement*  TLFSmokeEngine::SaveXML()
{
	return NULL;
}
bool TLFSmokeEngine::DetectInRect(awpRect* rect)
{
	return false;
}
