#include "SDCounter.h"
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
	for (unsigned int i = 0; i < m_hdata.size() / 2; i++)
		v1 += m_hdata[i];
	for (unsigned int i = m_hdata.size() / 2; i < m_hdata.size(); i++)
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
			a[i] = b[i] * a[i];

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
	int sx = (int)floor(0.5 + ImageWidth*this->m_params.Zones[1].Rect.LeftTop.X / 100.);
	int ex = (int)floor(0.5 + ImageWidth*this->m_params.Zones[1].Rect.RightBottom.X / 100.);
	int sy = (int)floor(0.5 + ImageHeight*this->m_params.Zones[1].Rect.LeftTop.Y / 100.);
	int ey = (int)floor(0.5 + ImageHeight*this->m_params.Zones[1].Rect.RightBottom.Y / 100.);


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

	int sx = (int)floor(0.5 + ImageWidth*this->m_params.Zones[1].Rect.LeftTop.X / 100.);
	int ex = (int)floor(0.5 + ImageWidth*this->m_params.Zones[1].Rect.RightBottom.X / 100.);
	int sy = (int)floor(0.5 + ImageHeight*this->m_params.Zones[1].Rect.LeftTop.Y / 100.);
	int ey = (int)floor(0.5 + ImageHeight*this->m_params.Zones[1].Rect.RightBottom.Y / 100.);
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
				data[y*ImageWidth + j - 1] = buffer->GetValue(j);
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
	return (int)floor(s / (k*w) + 0.5);
}

