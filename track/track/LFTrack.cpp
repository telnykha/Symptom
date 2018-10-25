#include "_LF.h"
#include "LFTrack.h"

#include "zones.cpp"

TLFTrackEngine::TLFTrackEngine()
{
	m_minWidth = 5;
	m_maxWidth = 30;
	m_minHeight = 5;
	m_maxHeight = 30;
    TLFMotionDetector* d = new TLFMotionDetector();
	this->m_detectors.Add(d);
	m_cluster_maker = new TLFClusterTrack(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);
	m_strPredictorName = "NULL";
}

TLFTrackEngine::TLFTrackEngine(TVAInitParams& params)
{
	TLFMotionDetector* d = new TLFMotionDetector();

	m_minWidth = params.minWidth;
	m_maxWidth = params.maxWidth;
	m_minHeight = params.minHeight;
	m_maxHeight = params.maxWidth;

	// Zones
	TLFZones* z = ConvertVAParamsToZones(params);
	if (z != NULL)
	{
		SetZones(z);
		delete z;
	}
	this->m_detectors.Add(d);
	m_cluster_maker = new TLFClusterTrack(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);
	m_strPredictorName = "NULL";
}
TLFTrackEngine::~TLFTrackEngine()
{
	delete m_cluster_maker;
}

void TLFTrackEngine::InitDetectors()
{
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	if (d != NULL)
	{
		awpImage* img = m_SourceImage.GetImage();
		d->Init(img);
	}
}

void TLFTrackEngine::Clear()
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
bool TLFTrackEngine::LoadXML(TiXmlElement* parent)
{
	return false;
}
TiXmlElement*  TLFTrackEngine::SaveXML()
{
	return NULL;
}
bool TLFTrackEngine::FindObjects()
{
	if (this->m_SourceImage.GetImage() == NULL)
		return false;
	if (this->m_SourceImage.GetImage()->dwType != AWP_BYTE)
		return false;

	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	d->Detect();
	m_cluster_maker->MakeClusters();
	BuildForeground();
	this->OverlapsFilter(&m_tmp_result);
	return true;
}

bool TLFTrackEngine::DetectInRect(awpRect* rect)
{
	return false;
}


void    TLFTrackEngine::BuildForeground()
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

	if (d->GetNumItems() > 0)
	{
		for (int i = 0; i < d->GetNumItems(); i++)
		{
			TLFDetectedItem* item = d->GetItem(i);
			if (item != NULL && item->HasObject())
			{
				awpRect    item_rect = item->GetBounds()->GetRect();
				awpImage*  fg = m_foreground.GetImage();

				awpFillRect(fg, &item_rect, 0, item->GetColor() + 1);
				count++;
			}
		}
	}
}

awpImage* TLFTrackEngine::GetForeground()
{
	return m_foreground.GetImage();
}

TLFImage* TLFTrackEngine::GetForegroundImage()
{
	return &m_foreground;
}

void TLFTrackEngine::OverlapsFilter(TLFSemanticImageDescriptor* descriptor)
{
	this->m_result.Clear();
	SLFBinaryBlob* blobs = this->m_cluster_maker->GetBlobs();
	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		if (blobs[i].state > 0)
		{
			awpRect rect;
			rect.left = (AWPSHORT)blobs[i].sx;
			rect.right = rect.left + (AWPSHORT)blobs[i].w;
			rect.top = (AWPSHORT)blobs[i].sy;
			rect.bottom = rect.top + (AWPSHORT)blobs[i].h;

			TLFDetectedItem* item = new TLFDetectedItem(&rect, 0, "Motion", 0, 0, 8, 8, "TLFMotionDetector", blobs[i].id, NULL);
			item->Resize(m_resizeCoef);
			item->SetState(blobs[i].state);
			m_result.Add(item);
		}
	}
}

float TLFTrackEngine::GetMinWidth()
{
	return m_minWidth;
}
void TLFTrackEngine::SetMinWidth(float value)
{
	if (value > 0 && value < 100)
	{
		m_minWidth = value;
		delete m_cluster_maker;
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
		m_cluster_maker = new TLFClusterTrack(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);

	}
}
float TLFTrackEngine::GetMinHeight()
{
	return m_minHeight;
}

void TLFTrackEngine::SetMinHeight(float value)
{
	if (value > 0 && value < 100)
	{
		m_minHeight = value;
		delete m_cluster_maker;
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
		m_cluster_maker = new TLFClusterTrack(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);
	}
}

float TLFTrackEngine::GetMaxWidth()
{
	return this->m_maxWidth;
}

void TLFTrackEngine::SetMaxWidth(float value)
{
	if (value > 0 && value < 100)
	{
		m_maxWidth = value;
		delete m_cluster_maker;
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
		m_cluster_maker = new TLFClusterTrack(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);
	}
}

float TLFTrackEngine::GetMaxHeight()
{
	return this->m_maxHeight;
}

void  TLFTrackEngine::SetMaxHeight(float value)
{
	if (value > 0 && value < 100)
	{
		m_maxHeight = value;
		delete m_cluster_maker;
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
		m_cluster_maker = new TLFClusterTrack(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);
	}
}

