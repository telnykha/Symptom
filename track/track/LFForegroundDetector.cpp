//---------------------------------------------------------------------------

#pragma hdrstop

#include "LFForegroundDetector.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


ILFFeaturesClassify::ILFFeaturesClassify(TLFFeaturesVector& vector, TLFRect& rect, double threshold)
{
    this->m_threshold = threshold;
    this->m_rect = rect;
    this->m_vector = vector;
}

ILFFeaturesClassify::ILFFeaturesClassify()
{
    this->m_threshold = 0;
    this->m_rect.SetRect(0,0,8,8);
}


double ILFFeaturesClassify::GetThreshold()
{
    return this->m_threshold;
}
void   ILFFeaturesClassify::SetThreshold(double value)
{
    this->m_threshold = value;
}

TLFRect* ILFFeaturesClassify::GetRect()
{
    return &this->m_rect;
}
void     ILFFeaturesClassify::SetRect(TLFRect& rect)
{
    m_rect = rect;
    // setup features vector
    for (int i = 0; i < this->m_vector.NumFeatures(); i++)
    {
        ILFFeature* f   = (ILFFeature*)this->m_vector.GetFeature(i);
        f->Setup(rect);
    }
}
////////////////////////////////////////////////////////////////////////////

TLFNNFeaturesClassify::TLFNNFeaturesClassify(int depth, TLFRect& rect, double threshold, int delay):ILFFeaturesClassify()
{
    int x,y,w,h;
    x = rect.Left();
    y = rect.Top();
    w = rect.Width();
    h = rect.Height();

	ILFFeature*   f = LFCreateFeature("TLFAFeature", x,y,w,h);
    m_vector.AddFeature(f);
    f = LFCreateFeature("TLFSFeature", x,y,w,h);
    m_vector.AddFeature(f);
    f = LFCreateFeature("TLFHAFeature", x,y,w,h/2);
    m_vector.AddFeature(f);
    f = LFCreateFeature("TLFVAFeature", x,y,w/2,h);
    m_vector.AddFeature(f);
    f = LFCreateFeature("TLFDAFeature", x,y,w/2,h/2);
    m_vector.AddFeature(f);

    m_threshold = threshold;

    m_buffer = new TLFAwpImageBuffer(5, depth);

	m_count = 0;
	m_delay = delay; // delay in the ms 
}
TLFNNFeaturesClassify::~TLFNNFeaturesClassify()
{
    delete m_buffer;
}
bool TLFNNFeaturesClassify::Classify(TLFImage& image)
{
    double* d = m_vector.CalcFeatures(&image);
    double norm[5] = {255, 100
		, 255, 255, 255};
    double  D = m_buffer->GetMinDistanceL2Norm(d, norm);
    if (D < this->m_threshold)
    {
		this->m_count = 0;
		m_buffer->AddVector(d);
        return false;
    }
	if (this->m_count == 0)
	{
		this->m_count = LFGetTickCount();
	}
	unsigned long time = LFGetTickCount() - m_count;
	if (time > this->m_delay)
		m_buffer->AddVector(d);
    return true;
}


////////////////////////////////////////////////////////////////////////////
TLFForegroundDetector::TLFForegroundDetector(int delay) : ILFObjectDetector()
{

    m_strDetName = this->GetName();
    this->m_baseHeight = 12;
    this->m_baseWidth  = 12;
	this->m_delay = delay;
    if (this->m_scanner != NULL)
    {
        delete this->m_scanner;
        this->m_scanner = new TLFTileScanner();
        this->m_scanner->GetParameter(0)->SetValue(50);
        this->m_scanner->SetBaseHeight(this->m_baseHeight);
        this->m_scanner->SetBaseWidth(this->m_baseWidth);
    }
}
TLFForegroundDetector::~TLFForegroundDetector()
{

}

bool TLFForegroundDetector::Init(awpImage* pImage, bool scan)
{
    if (pImage == NULL || pImage->dwType != AWP_BYTE || m_scanner == NULL)
        return false;
	bool changed = m_Image.GetImage() == NULL || m_Image.GetImage()->sSizeX != pImage->sSizeX ||
		m_Image.GetImage()->sSizeY != pImage->sSizeY || m_scanner->GetFragmentsCount() == 0;

   	m_Image.SetImage(pImage);

    if (changed)
    {
        this->m_sensors.Clear();
        m_scanner->Scan(&m_Image);
        for(int i= 0; i < m_scanner->GetFragmentsCount(); i++)
        {
            TLFRect rect(m_scanner->GetFragmentRect(i));

            // We use a descendant of the class ILFFeaturesClassify
            // TLFNNFeaturesClassify

            TLFNNFeaturesClassify* c = new TLFNNFeaturesClassify(50, rect, 0.1, m_delay);
            this->m_sensors.Add(c);

            UUID id;
            LF_NULL_UUID_CREATE(id);

            awpRect r = rect.GetRect();
			TLFDetectedItem* de = new TLFDetectedItem(&r, 0, "Foreground", 0, 0,
				this->m_baseWidth, this->m_baseHeight, this->m_strDetName, id);
			de->SetHasObject(false);
			m_objects.Add(de);
        }
    }
    return m_Image.GetImage() != NULL;
}
int  TLFForegroundDetector::ClassifyRect(awpRect Fragmnet, double* err, int* vect)
{
    return 0;
}
int  TLFForegroundDetector::Detect()
{
    int count = 0;
	for (int i = 0; i < m_objects.GetCount(); i++)
	{
		TLFDetectedItem* di = (TLFDetectedItem*)m_objects.Get(i);
        ILFFeaturesClassify* c = (ILFFeaturesClassify*)m_sensors.Get(i);
		if (di != NULL)
		{
            di->SetHasObject(false);
            if (c->Classify(this->m_Image))
            {
				di->SetHasObject(true);
                count++;
            }
		}
	}
	return count;
}
bool	TLFForegroundDetector::AddStrong(ILFStrong* strong)
{
    return false;
}
double  TLFForegroundDetector::GetThreshold()
{
    ILFFeaturesClassify* c = (ILFFeaturesClassify*)this->m_sensors.Get(0);
    if (c != NULL)
        return c->GetThreshold();
    return 0;
}
//  value should be between 0 - 1
//  1 means  0.05 0 means 0.25
void TLFForegroundDetector::SetThreshold(double Value)
{
	double t = 0.05 + (1 - Value) / 5;
	for (int i = 0; i < this->m_sensors.GetCount(); i++)
    {
         ILFFeaturesClassify* c = (ILFFeaturesClassify*)this->m_sensors.Get(i);
         if (c != NULL)
            c->SetThreshold(t);
    }
}
TiXmlElement* TLFForegroundDetector::SaveXML()
{
    TiXmlElement* e = new TiXmlElement(this->GetName());
    return e;
}
bool          TLFForegroundDetector::LoadXML(TiXmlElement* parent)
{
    return true;
}

bool TLFForegroundDetector::Save(const char* lpFileName)
{
    TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );

    TiXmlElement*  e = new TiXmlElement(this->GetName());
    TLFFeaturesVector* v = (TLFFeaturesVector*)this->m_sensors.Get(0);
    if (v == NULL)
        return false;

    //todo:

    return doc.SaveFile(lpFileName);
}

bool TLFForegroundDetector::Load(const char* lpFileName)
{
    return false;
}
