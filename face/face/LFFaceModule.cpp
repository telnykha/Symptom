#include "_LF.h"
#include "LFFaceModule.h"

//-------------------------------TLFFaceEngine----------------------------------
TLFFaceEngine::TLFFaceEngine(bool tilt, bool profile, bool profile_tilt)
{
	m_needTilt = tilt;
	m_needSemiFront = profile;
	m_needSemiFrontTilt = profile_tilt;
}

TLFFaceEngine::~TLFFaceEngine()
{

}
bool TLFFaceEngine::LoadXML(TiXmlElement* parent)
{
	try
	{
		if (parent == NULL)
			return false;
		const char* str = parent->Attribute("type");
		if (strcmp(str, this->GetName()) != 0)
			return false;
		m_strPredictorName = parent->Attribute("predictor");
		m_detectors.Clear();
		// загружаем детектор в ракурсе front 
		TiXmlElement* elem = parent->FirstChild("ILFObjectDetector")->ToElement();
		if (elem == NULL)
			return NULL;
		const char* type = elem->Attribute("type");

		if (strcmp(type, "TSCObjectDetector") == 0)
		{
			int n = this->m_needTilt ? 3 : 1;
			for (int i = 0; i < n; i++)
			{
				TSCObjectDetector* detector = new TSCObjectDetector();
				TiXmlElement* e = elem->FirstChildElement();
				if (!detector->LoadXML(e))
				{
					delete detector;
					throw 0;
				}
				m_detectors.Add(detector);
			}
		}
		// при необходимости загружаем второй детектор
		if (this->m_needSemiFront)
		{
			if (elem->NextSibling() != NULL)
			{
				elem = elem->NextSibling()->ToElement();
				const char* type = elem->Attribute("type");
				if (strcmp(type, "TSCObjectDetector") == 0)
				{
					int n = this->m_needSemiFrontTilt ? 6 : 2;
					for (int i = 0; i < n; i++)
					{
						TSCObjectDetector* detector = new TSCObjectDetector();
						TiXmlElement* e = elem->FirstChildElement();
						if (!detector->LoadXML(e))
						{
							delete detector;
							throw 0;
						}
						m_detectors.Add(detector);
					}
				}
			}
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

static  void  RotateKeep0(awpImage* pImage, awpImage** ppImage, int angle)
{
	if (pImage == NULL || pImage->dwType != AWP_BYTE)
		return;

	double fi = (double)angle;
	double rad_fi = 0;

	rad_fi = fi*3.14 / 180.;

	double sn_fi = sin(rad_fi);
	double cn_fi = cos(rad_fi);

	awpColor* c_pix_src = NULL;
	awpColor* c_pix_dst = NULL;

	AWPBYTE*     b_pix_src = NULL;
	AWPBYTE*     b_pix_dst = NULL;

	awpCreateImage(ppImage, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, pImage->dwType);

	awpImage* pDst = *ppImage;
	if (pImage->bChannels == 3)
	{
		c_pix_src = (awpColor*)pImage->pPixels;
		c_pix_dst = (awpColor*)pDst->pPixels;
	}
	else if (pImage->bChannels == 1)
	{
		b_pix_src = (AWPBYTE*)pImage->pPixels;
		b_pix_dst = (AWPBYTE*)pDst->pPixels;
	}
	else
		return;

	int x1, y1;
	int dx = pDst->sSizeX / 2;
	int dy = pDst->sSizeY / 2;
	for (int y = 0; y < pDst->sSizeY; y++)
	{
		for (int x = 0; x < pDst->sSizeX; x++)
		{
			x1 = int(dx + (x - dx)*cn_fi + (y - dy)*sn_fi);
			y1 = int(dy - (x - dx)*sn_fi + (y - dy)*cn_fi);

			if (x1 >= pDst->sSizeX || x1 < 0)
				continue;
			if (y1 >= pDst->sSizeY || y1 < 0)
				continue;


			if (c_pix_src != NULL)
			{
				c_pix_dst[x + pDst->sSizeX*y] = c_pix_src[x1 + pDst->sSizeX*y1];
			}
			else
			{
				b_pix_dst[x + pDst->sSizeX*y] = b_pix_src[x1 + pDst->sSizeX*y1];
			}
		}
	}
}
static awpRect RotateRect(awpRect rect, int angle, awpPoint c)
{
	awpRect result;
	memset(&result, 0, sizeof(awpRect));
	double fi = angle;

	// rotate rect 
	awpPoint p;
	int w, h;
	p.X = rect.left;
	p.Y = rect.top;
	w = rect.right - rect.left;
	h = rect.bottom - rect.top;
	p.X = int(c.X + (p.X - c.X)*cos(fi) + (p.Y - c.Y)*sin(fi));
	p.Y = int(c.Y - (p.X - c.X)*sin(fi) + (p.Y - c.Y)*cos(fi));
	rect.left = p.X;
	rect.right = p.X + w;
	rect.top = p.Y;
	rect.bottom = p.Y + h;

	result.left = rect.left;
	result.top = rect.top;
	result.right = rect.right;
	result.bottom = rect.bottom;


	return result;
}
void TLFFaceEngine::Clear()
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
void TLFFaceEngine::InitDetectors()
{
	int c = 0;
	awpImage* img = m_SourceImage.GetImage();

	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(c++);
	d->Init(img);
	d->SetAngle(0);
	d->SetRacurs(0);
	if (this->m_needTilt)
	{
		awpImage* tmp = NULL;

		//поворот +15
		RotateKeep0(img, &tmp, 15);
		d = (ILFObjectDetector*)m_detectors.Get(c++);
		d->Init(tmp);
		d->SetAngle(15);
		d->SetRacurs(0);
		_AWP_SAFE_RELEASE_(tmp)
			//поворот -15
			d = (ILFObjectDetector*)m_detectors.Get(c++);
		RotateKeep0(img, &tmp, -15);
		d->Init(tmp);
		d->SetAngle(-15);
		d->SetRacurs(0);
		_AWP_SAFE_RELEASE_(tmp)
	}
	if (this->m_needSemiFront)
	{
		awpImage* tmp = NULL;

		//профиль вправо
		d = (ILFObjectDetector*)m_detectors.Get(c++);
		d->Init(img);
		d->SetAngle(0);
		d->SetRacurs(1);
		if (m_needSemiFrontTilt)
		{
			//поворот +15
			RotateKeep0(img, &tmp, 15);
			d = (ILFObjectDetector*)m_detectors.Get(c++);
			d->Init(tmp);
			d->SetAngle(15);
			d->SetRacurs(1);
			_AWP_SAFE_RELEASE_(tmp)
				//поворот -15
				d = (ILFObjectDetector*)m_detectors.Get(c++);
			RotateKeep0(img, &tmp, -15);
			d->Init(tmp);
			d->SetAngle(-15);
			d->SetRacurs(1);
			_AWP_SAFE_RELEASE_(tmp)
		}
		//профиль влево
		d = (ILFObjectDetector*)m_detectors.Get(c++);
		awpCopyImage(img, &tmp);
		awpFlip(&tmp, AWP_FLIP_HRZT);
		d->Init(tmp);
		d->SetAngle(0);
		d->SetRacurs(7);
		if (m_needSemiFrontTilt)
		{
			awpImage* tmp0 = NULL;
			//поворот +15
			RotateKeep0(tmp, &tmp0, 15);
			d = (ILFObjectDetector*)m_detectors.Get(c++);
			d->Init(tmp);
			d->SetAngle(15);
			d->SetRacurs(7);
			_AWP_SAFE_RELEASE_(tmp0)
				//поворот -15
				d = (ILFObjectDetector*)m_detectors.Get(c++);
			RotateKeep0(tmp, &tmp0, -15);
			d->Init(tmp);
			d->SetAngle(-15);
			d->SetRacurs(7);

			_AWP_SAFE_RELEASE_(tmp0)
		}
		_AWP_SAFE_RELEASE_(tmp)
	}
}

bool TLFFaceEngine::FindObjects()
{
	if (this->m_SourceImage.GetImage() == NULL)
		return false;
	if (this->m_SourceImage.GetImage()->dwType != AWP_BYTE)
		return false;

	m_tmpList.Clear();
	if (this->m_needTrack)
		m_result.Predict(this);
	else
		m_result.Clear();
	/*
	if (this->m_needTrack)
	{
	m_fgbg.Detect();

	for (int i = 0; i < m_fgbg.GetNumItems(); i++)
	{

	TLFDetectedItem* di = m_fgbg.GetItem(i);
	for (int j = 0; j < m_detectors.GetCount(); j++)
	{
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(j);
	ILFScanner* s = d->GetScanner();
	TLFBounds* b0 = s->GetFragment(i);
	b0->HasObject = !di->HasObject();
	}
	}
	}
	*/

	int cc = 0;
#ifdef _OMP_
	omp_set_num_threads(m_detectors.GetCount());
#pragma omp parallel for  reduction(+: cc)
#endif
	for (int k = 0; k < m_detectors.GetCount(); k++)
	{
		++cc;
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(k);
		d->Detect();
	}

	for (int k = 0; k < m_detectors.GetCount(); k++)
	{
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(k);
		if (d->GetNumItems() > 0)
		{
			for (int i = 0; i < d->GetNumItems(); i++)
			{
				TLFDetectedItem* item = d->GetItem(i);
				TLFDetectedItem* di = new TLFDetectedItem(item);
				di->Resize(m_resizeCoef);
				if (d->GetRacurs() == 7)
				{
					//Flip di
					int w = m_result.Width();
					di->Flip(w);
				}
				if (d->GetAngle() != 0)
				{
					int angle = d->GetAngle();
					awpPoint c;
					c.X = m_result.Width() / 2;
					c.Y = m_result.Height() / 2;
					di->Rotate(c, angle);
				}
				m_tmpList.PushBack(di);
			}
		}
	}
	if (this->m_needTrack)
	{
		m_tmp_result.Clear();
		this->OverlapsFilter(&m_tmp_result);
		this->m_result.Update(this, &m_tmp_result);
	}
	else
	{
		if (!this->m_needCluster)
		{
			m_result.Clear();
			TLFDetectedItem* di = dynamic_cast<TLFDetectedItem*>(m_tmpList.First());
			if (di != NULL)
			{
				while (di != NULL)
				{
					TLFDetectedItem* dd = new  TLFDetectedItem(di);
					m_result.AddDetectedItem(dd);

					di = (TLFDetectedItem*)m_tmpList.Next(di);
				}
			}
			m_tmpList.Clear(true);
		}
		else
			OverlapsFilter(&this->m_result);
	}
	return true;
}

TiXmlElement*  TLFFaceEngine::SaveXML()
{
	return NULL;
}

bool TLFFaceEngine::DetectInRect(awpRect* rect)
{
	if (rect == NULL)
		return false;
	double e[4];
	int    v[4];
	bool result = false;
	for (int k = 0; k < m_detectors.GetCount(); k++)
	{
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(k);
		d->ClassifyRect(*rect, e, v);
		if (v[0] > 0)
			result = true;
	}
	return result;
}
void TLFFaceEngine::OverlapsFilter(TLFSemanticImageDescriptor* descriptor)
{
	TLFList list;
	while (m_tmpList.First() != NULL)
	{
		TLFDetectedItem* di = (TLFDetectedItem*)m_tmpList.First();
		m_tmpList.Pop(di);
		list.PushBack(di);
		TLFDetectedItem* item = di;

		TLFRect* rect1 = item->GetBounds();
		di = (TLFDetectedItem*)m_tmpList.First();
		if (di != NULL)
		{
			do
			{
				TLFRect* rect2 = di->GetBounds();
				awpRect r = rect1->GetRect();
				if (rect2->RectOverlap(r) > 0.1)
				{
					list.PushBack(di);
					di = (TLFDetectedItem*)m_tmpList.Pop(di);
				}
				else
					di = (TLFDetectedItem*)m_tmpList.Next(di);
			} while (di != NULL);
		}

		TLFDetectedItem* d = (TLFDetectedItem*)list.First();
		if (d != NULL)
		{
			int count = 1;
			int r0 = 0;
			int r1 = 0;
			int r7 = 0;
			if (d->GetRacurs() == 0)
				r0++;
			else if (d->GetRacurs() == 1)
				r1++;
			else if (d->GetRacurs() == 7)
				r7++;
			awpRect rect = d->GetBounds()->GetRect();
			TLFDetectedItem* dd = d;
			do
			{
				dd = (TLFDetectedItem*)list.Next(dd);
				if (dd != NULL)
				{
					awpRect rect1 = dd->GetBounds()->GetRect();
					rect.left += rect1.left;
					rect.top += rect1.top;
					rect.right += rect1.right;
					rect.bottom += rect1.bottom;

					if (dd->GetRacurs() == 0)
						r0++;
					else if (dd->GetRacurs() == 1)
						r1++;
					else if (dd->GetRacurs() == 7)
						r7++;

					count++;
				}
			} while (dd != NULL);

			rect.left /= count;
			rect.top /= count;
			rect.right /= count;
			rect.bottom /= count;
			item->SetBounds(rect);

			if (r0 >= r1 && r0 >= r7)
				item->SetRacurs(0);
			else if (r1 > r0 && r1 > r7)
				item->SetRacurs(1);
			else if (r7 > r0 && r7 > r1)
				item->SetRacurs(7);

		}
		//m_result.AddDetectedItem(item);
		descriptor->AddDetectedItem(item);
		delete item;

		list.Clear(true);
	}
	m_tmpList.Clear();
}
