#include "LFFlowDetector.h"

TLFFlowDetector::TLFFlowDetector()
{
	this->m_threshold = 1;
	this->m_baseHeight = 2;
	this->m_baseWidth = 2;
	if (m_scanner != NULL)
		delete m_scanner;
	m_scanner = new TLFTileScanner();
	m_scanner->GetParameter(0)->SetValue(50);
}

bool TLFFlowDetector::Init(awpImage* pImage, bool scan)
{
	awpImage* tmp_img = NULL;
	if (pImage == NULL || pImage->dwType != AWP_BYTE || m_scanner == NULL)
		return false;
	bool changed = m_Image.GetImage() == NULL ||
		m_Image.GetImage()->sSizeX != pImage->sSizeX ||
		m_Image.GetImage()->sSizeY != pImage->sSizeY ||
		m_scanner->GetFragmentsCount() == 0;

	if (changed)
	{
		m_objects.Clear();
		m_flow.FreeImages();
		m_Image.FreeImages();

		// create flow image
		if (awpCreateImage(&tmp_img, pImage->sSizeX / 2, pImage->sSizeY / 2, 2, AWP_FLOAT) != AWP_OK)
			return false;
		m_flow.SetImage(tmp_img);
		_AWP_SAFE_RELEASE_(tmp_img)

			// create previous image
			if (awpCopyImage(pImage, &tmp_img))
			{
				m_flow.FreeImages();
				return false;
			}
		//
		awpResize(tmp_img, pImage->sSizeX / 2, pImage->sSizeY / 2);
		awpConvert(tmp_img, AWP_CONVERT_3TO1_BYTE);
		m_prev.SetImage(tmp_img);
		_AWP_SAFE_RELEASE_(tmp_img)

			m_Image.SetImage(pImage);
		m_scanner->Scan(&m_Image);

		for (int i = 0; i < m_scanner->GetFragmentsCount(); i++)
		{
			awpRect rect = m_scanner->GetFragmentRect(i);
			UUID id;

			LF_NULL_UUID_CREATE(id);
			TLFDetectedItem* de = new TLFDetectedItem(&rect, 0, "", 0, 0,
				this->m_baseWidth, this->m_baseHeight, this->m_strDetName.c_str(), id);
			de->SetHasObject(false);
			m_objects.Add(de);
		}

		return true;
	}

	awpCopyImage(pImage, &tmp_img);
	awpResize(tmp_img, pImage->sSizeX / 2, pImage->sSizeY / 2);
	awpConvert(tmp_img, AWP_CONVERT_3TO1_BYTE);

	double sum = 0;
	AWPBYTE* b0 = _AWP_BPIX_(m_prev.GetImage(), AWPBYTE)
		AWPBYTE* b1 = _AWP_BPIX_(tmp_img, AWPBYTE)
		for (int i = 0; i < tmp_img->sSizeX*tmp_img->sSizeY; i++)
		{
			sum += fabs(float(b1[i] - b0[i]));
		}
	if (sum > 0)
	{
		//awpcvFBFlow(m_prev.GetImage(), tmp_img, m_flow.GetImage());

		m_prev.SetImage(tmp_img);
		m_Image.SetImage(pImage);
	}

	_AWP_SAFE_RELEASE_(tmp_img)

		return true;
}
// classification
int  TLFFlowDetector::ClassifyRect(awpRect Fragmnet, double* err, int* vect)
{
	return 0;
}

int  TLFFlowDetector::Detect()
{
	if (m_flow.GetImage() == NULL)
		return 0;
	awpImage* mmimage = m_flow.GetImage();
	AWPFLOAT* f = (AWPFLOAT*)(m_flow.GetImage())->pPixels;
	int _x;
	int _y;
	float v;
	awpPoint c;
	TLFRect* rect;
	TLFDetectedItem* di;
	for (int i = 0; i < m_objects.GetCount(); i++)
	{
		di = (TLFDetectedItem*)m_objects.Get(i);
		di->SetHasObject(false);

		if (di != NULL)
		{
			rect = di->GetBounds();
			if (rect != NULL)
			{
				c = rect->Center();
				_x = c.X / 2;
				_y = c.Y / 2;
				v = fabs(f[_y * 2 * mmimage->sSizeX + 2 * _x]) > fabs(f[_y * 2 * mmimage->sSizeX + 2 * _x + 1]) ? fabs(f[_y * 2 * mmimage->sSizeX + 2 * _x]) : fabs(f[_y * 2 * mmimage->sSizeX + 2 * _x + 1]);

				di->SetHasObject(v > this->m_threshold);
			}
		}
	}
	return 0;
}
// properties
int	  TLFFlowDetector::GetStagesCount()
{
	return 0;
}
bool  TLFFlowDetector::AddStrong(ILFStrong* strong)
{
	return false;
}
double TLFFlowDetector::GetThreshold()
{
	return 	m_threshold;
}
void TLFFlowDetector::SetThreshold(double Value)
{
	m_threshold = Value;
}

int TLFFlowDetector::Rating(awpRect& Fragment, double* rating)
{
	return 0;
}

TiXmlElement* TLFFlowDetector::SaveXML()
{
	return NULL;
}
bool          TLFFlowDetector::LoadXML(TiXmlElement* parent)
{
	return false;
}
