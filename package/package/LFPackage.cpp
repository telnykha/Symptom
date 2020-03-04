#include "_LF.h"
#include "LFPackage.h"
#include "zones.cpp"

TLFPackageEngine::TLFPackageEngine(TVAInitParams& params) : ILFDetectEngine()
{
	m_minWidth = 1;
	m_maxWidth = 30;
	m_minHeight = 1;
	m_maxHeight = 30;

	TLFFGBGDetector* d = new TLFFGBGDetector();
    d->SetDelay(15);

	this->m_detectors.Add(d);
	m_cluster_maker = new TLFClusterTrack(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);
	m_strPredictorName = "TLFIntegralImagePredictor";
#ifdef _DEBUG
	UUID id;
	LF_UUID_CREATE(id);
	std::string str = LFGUIDToString(&id);
	str += ".package";
	m_strFileName = str;
	m_logFile = fopen(m_strFileName.c_str(), "w+t");
	fclose(m_logFile);
	m_logFile = NULL;
#endif 

	// Zones
	TLFZones* z = ConvertVAParamsToZones(params);
	if (z != NULL)
	{
		SetZones(z);
		delete z;
	}
}

TLFPackageEngine::~TLFPackageEngine()
{
#ifdef _DEBUG
	if (m_logFile != NULL)
		fclose(m_logFile);
#endif
}


void TLFPackageEngine::InitDetectors()
{
#ifdef _DEBUG 
	m_logFile = fopen(m_strFileName.c_str(), "a+t");
	fprintf(m_logFile, "Entrer TLFPackageEngine::InitDetectors()\n");
	fclose(m_logFile);
	m_logFile = NULL;
#endif 
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
   if (d != NULL)
   {
	   awpImage* img = m_SourceImage.GetImage();
	   d->Init(img);
   }
#ifdef _DEBUG 
   m_logFile = fopen(m_strFileName.c_str(), "a+t");
   fprintf(m_logFile, "Leave TLFPackageEngine::InitDetectors()\n");
   fclose(m_logFile);
   m_logFile = NULL;
#endif 
}

void TLFPackageEngine::Clear()
{
#ifdef _DEBUG 
	m_logFile = fopen(m_strFileName.c_str(), "a+t");
	fprintf(m_logFile, "Entrer TLFPackageEngine::Clear()\n");
	fclose(m_logFile);
	m_logFile = NULL;
#endif 

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
#ifdef _DEBUG 
	m_logFile = fopen(m_strFileName.c_str(), "a+t");
	fprintf(m_logFile, "Leave TLFPackageEngine::Clear()\n");
	fclose(m_logFile);
	m_logFile = NULL;
#endif 
}
bool TLFPackageEngine::LoadXML(TiXmlElement* parent)
{
	return false;
}
TiXmlElement*  TLFPackageEngine::SaveXML()
{
	return NULL;
}
bool TLFPackageEngine::FindObjects()
{
#ifdef _DEBUG 
	m_logFile = fopen(m_strFileName.c_str(), "a+t");
	fprintf(m_logFile, "Enter TLFPackageEngine::FindObjects()\n");
	fclose(m_logFile);
	m_logFile = NULL;
#endif 
	if (this->m_SourceImage.GetImage() == NULL)
		return false;
	if (this->m_SourceImage.GetImage()->dwType != AWP_BYTE)
		return false;

	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	d->Detect();
   	m_cluster_maker->MakeClusters();
	BuildForeground();
	this->OverlapsFilter(&m_tmp_result);

#ifdef _DEBUG 
	m_logFile = fopen(m_strFileName.c_str(), "a+t");
	fprintf(m_logFile, "Leave TLFPackageEngine::FindObjects()\n");
	fclose(m_logFile);
	m_logFile = NULL;
#endif 
	return true;
}


int TLFPackageEngine::GetDelay()
{
	TLFFGBGDetector* d = (TLFFGBGDetector*)GetDetector();
	if (d != NULL)
	{
		return d->GetDelay();
	}
	else
		return 0;
}

void TLFPackageEngine::SetDelay(int value)
{
	TLFFGBGDetector* d = (TLFFGBGDetector*)GetDetector();
	if (d != NULL)
	{
		d->SetDelay(value);
	}
}

void    TLFPackageEngine::BuildForeground()
{
#ifdef _DEBUG 
	m_logFile = fopen(m_strFileName.c_str(), "a+t");
	fprintf(m_logFile, "Enter TLFPackageEngine::BuildForeground()\n");
	fclose(m_logFile);
	m_logFile = NULL;
#endif
	m_tmpList.Clear();
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

				//awpFillRect(fg, &item_rect, 0,item->GetColor() + 1);
				awpFillRect(fg, &item_rect, 0,255);

				TLFDetectedItem* di = new TLFDetectedItem(item);
				di->Resize(m_resizeCoef);
				m_tmpList.Add(di);
				count++;
			}
		}
	}
#ifdef _DEBUG 
	m_logFile = fopen(m_strFileName.c_str(), "a+t");
	fprintf(m_logFile, "Leave TLFPackageEngine::BuildForeground()\n");
	fclose(m_logFile);
	m_logFile = NULL;
#endif
}

awpImage* TLFPackageEngine::GetForeground()
{
	return m_foreground.GetImage();
}

TLFImage* TLFPackageEngine::GetForegroundImage()
{
	return &m_foreground;
}

void TLFPackageEngine::OverlapsFilter(TLFSemanticImageDescriptor* descriptor)
{
#ifdef _DEBUG 
	m_logFile = fopen(m_strFileName.c_str(), "a+t");
	fprintf(m_logFile, "Enter TLFPackageEngine::OverlapsFilter()\n");
	fclose(m_logFile);
	m_logFile = NULL;
#endif
	//	SLFBinaryBlob* blob = this->m_cluster_maker->GetBlobs();
	this->m_result.Clear();
	SLFBinaryBlob* blobs = this->m_cluster_maker->GetBlobs();
	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		if (blobs[i].state > 0 && blobs[i].state < 4)
		{
			awpRect rect;
			rect.left = (AWPSHORT)blobs[i].sx;
			rect.right = rect.left + (AWPSHORT)blobs[i].w;
			rect.top = (AWPSHORT)blobs[i].sy;
			rect.bottom = rect.top + (AWPSHORT)blobs[i].h;

			TLFDetectedItem* item = new TLFDetectedItem(&rect, 0, "Package part", 0, 0,8,8,"TLFFGBGDetector",blobs[i].id,NULL);
			item->Resize(m_resizeCoef);
			item->SetState(blobs[i].state);
			m_result.Add(item);
		}
	}
#ifdef _DEBUG 
	m_logFile = fopen(m_strFileName.c_str(), "a+t");
	fprintf(m_logFile, "Leave TLFPackageEngine::OverlapsFilter()\n");
	fclose(m_logFile);
	m_logFile = NULL;
#endif
}

float TLFPackageEngine::GetMinWidth()
{
	return m_minWidth;
}
void TLFPackageEngine::SetMinWidth(float value)
{
	if (value > 0 && value < 100)
	{
		m_minWidth = value;
		delete m_cluster_maker;
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
		m_cluster_maker = new TLFClusterTrack(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);

	}
}
float TLFPackageEngine::GetMinHeight()
{
	return m_minHeight;
}

void TLFPackageEngine::SetMinHeight(float value)
{
	if (value > 0 && value < 100)
	{
		m_minHeight = value;
		delete m_cluster_maker;
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
		m_cluster_maker = new TLFClusterTrack(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);
	}
}

float TLFPackageEngine::GetMaxWidth()
{
	return this->m_maxWidth;
}

void TLFPackageEngine::SetMaxWidth(float value)
{
	if (value > 0 && value < 100)
	{
		m_maxWidth = value;
		delete m_cluster_maker;
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
		m_cluster_maker = new TLFClusterTrack(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);
	}
}

float TLFPackageEngine::GetMaxHeight()
{
	return this->m_maxHeight;
}

void  TLFPackageEngine::SetMaxHeight(float value)
{
	if (value > 0 && value < 100)
	{
		m_maxHeight = value;
		delete m_cluster_maker;
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
		m_cluster_maker = new TLFClusterTrack(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);
	}
}

void TLFPackageEngine::SetAverageBufferSize(int value)
{
	if (this->GetDetector() != NULL)
	{
		TLFFGBGDetector* d = (TLFFGBGDetector*)this->GetDetector();
		d->SetAverageBufferSize(value);
	}
}

int TLFPackageEngine::GetAverageBufferSize()
{
	if (this->GetDetector() != NULL)
	{
		TLFFGBGDetector* d = (TLFFGBGDetector*)this->GetDetector();
		return d->GetAverageBufferSize();
	}
	else
		return 0;

}

void TLFPackageEngine::SetBGStability(int value)
{
	if (this->GetDetector() != NULL)
	{
		TLFFGBGDetector* d = (TLFFGBGDetector*)this->GetDetector();
		d->SetBgStability(value);
	}
}
int TLFPackageEngine::GetBGStability()
{
	if (this->GetDetector() != NULL)
	{
		TLFFGBGDetector* d = (TLFFGBGDetector*)this->GetDetector();
		return d->GetBgStability();
	}
	else
		return 0;
}

