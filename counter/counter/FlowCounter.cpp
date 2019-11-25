#include "FlowCounter.h"

#include "_counter.h"
// ----------- event -----
class TLFEvent : public TLFObject
{
protected:
	int m_startFrame;
	int m_finishFrame;
	double m_power;
public:
	TLFEvent(int frame)
	{
		m_startFrame = frame;
		m_finishFrame = frame;
		m_power = 0;
	}

	void SetFinishFrame(int frame)
	{
		m_finishFrame = frame;
	}
	int GetStartFrame()
	{
		return m_startFrame;
	}
	int GetFinishFrame()
	{
		return m_finishFrame;
	}
	int GetLenght()
	{
		return m_finishFrame - m_startFrame;
	}
	double GetPower()
	{
		return m_power;
	}
	void AddPower(double power)
	{
		m_power += power;
	}
};
//------ Flow counter
TLFFlowCounter::TLFFlowCounter(TVAPoint& start, TVAPoint& finish, TVARect& sizes, double eventSens)
{

//	m_border.SetSp(CvPoint(start.X, start.Y));
//	m_border.SetEp(CvPoint(finish.X, finish.Y));
	m_in_event = NULL;
	m_out_event = NULL;
	m_counter = 0;
	m_threshold = 1.8 - 0.5*eventSens;
	m_sp = start;
	m_ep = finish;
	m_border_done = false;
}
TLFFlowCounter::~TLFFlowCounter()
{

	if (m_in_event != NULL)
		delete m_in_event;
	if (m_out_event != NULL)
		delete m_out_event;
}

bool TLFFlowCounter::ProcessImage(awpImage* img, int& num_in, int& num_out, double& sq)
{
	if (img == NULL)
		return false;
	if (m_border_done == false)
	{
		CvPoint sp;
		CvPoint ep;
		sp.x = m_sp.X*img->sSizeX / 100;
		sp.y = m_sp.Y*img->sSizeY / 100;
		ep.x = m_ep.X*img->sSizeX / 100;
		ep.y = m_ep.Y*img->sSizeY / 100;
		m_border.SetSp(sp);
		m_border.SetEp(ep);
		m_border_done = true;
	}

	IplImage* _img = cvCreateImageHeader(cvSize(img->sSizeX, img->sSizeY), IPL_DEPTH_8U, 1);
	_img->imageData = (char*)img->pPixels;
	m_flow.Process(_img);
	if (m_flow.GetModule() != NULL)
	{
		m_sensor.Init(m_flow.GetModule(), &m_border);
		if (m_sensor.DataLength() > 0)
		{
			m_sensor.Update(m_flow.GetModule(), m_flow.GetAngle());
			m_processor.AddData(m_sensor.Data());
		}
	}
	m_counter++;
	cvReleaseImageHeader(&_img);
	return true;
}

void TLFFlowCounter::AnslysisDataLine()
{
	if (m_sensor.DataLength() == 0)
		return;
	double* d = m_sensor.Data();
	double dmax = 0;
	double in_max = 0;
	double in_min = 0;
	double in_sum = 0;
	double out_sum = 0;
	// анализ данных на сенсоре
	for (int i = 0; i < 255; i++)
	{
		double v = fabs(d[i]);
		if (dmax < v)
			dmax = v;
		if (in_max < d[i])
			in_max = d[i];
		if (in_min > d[i])
			in_min = d[i];
		if (d[i] > 0)
			in_sum += d[i];
		else
			out_sum += fabs(d[i]);
	}

	double push_in = in_max > m_threshold ? 32 : 0;
	m_state_in.Push(push_in);
	if (push_in == 32 && m_state_in.GetData()[254] == 0)
	{
		m_in_event = new TLFEvent(m_counter);
	}
	else if (push_in == 0 && m_state_in.GetData()[254] == 32)
	{
		m_in_event->SetFinishFrame(m_counter);
		if (m_in_event->GetLenght() > 10)
		{
			m_in_events.Add(m_in_event);
			printf("in event: start = %d finish = %d length = %d power = %lf\n", m_in_event->GetStartFrame(), m_in_event->GetFinishFrame(),
				m_in_event->GetLenght(), m_in_event->GetPower() / m_in_event->GetLenght());
		}
		else
			delete m_in_event;
	}
	else if (push_in == 32)
		m_in_event->AddPower(in_sum);

	double push_out = in_min < -m_threshold ? 32 : 0;
	m_state_out.Push(push_out);

	if (push_out == 32 && m_state_out.GetData()[254] == 0)
	{
		m_out_event = new TLFEvent(m_counter);
	}
	else if (push_out == 0 && m_state_out.GetData()[254] == 32)
	{
		m_out_event->SetFinishFrame(m_counter);
		if (m_out_event->GetLenght() > 10)
		{
			m_out_events.Add(m_out_event);
			printf("out event: start = %d finish = %d length = %d power = %lf\n", m_out_event->GetStartFrame(), m_out_event->GetFinishFrame(),
				m_out_event->GetLenght(), m_out_event->GetPower() / m_out_event->GetLenght());
		}
		else
			delete m_out_event;
	}
	else if (push_out == 32)
		m_out_event->AddPower(out_sum);
}

void TLFFlowCounter::GetEventsCount(int* in_count, int* out_count)
{
	*in_count = m_in_events.GetCount();
	*out_count = m_out_events.GetCount();
}


// flow counter c API
HANDLE   fcounterCreate(TVAPoint start, TVAPoint finish, TVARect sizes, double eventSens)
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

	TheCounter* c = new TheCounter();
	c->size = sizeof(TheCounter);
	c->options = 2;//FLOW_COUNTER
	c->c_counter = NULL;
	c->t_counter = NULL;
	c->f_counter = new TLFFlowCounter(start, finish, sizes, eventSens);

	return (HANDLE)c;
}
HRESULT  fcounterProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, double& value1, double& value2)
{
	HANDLE module = hModule;
	TheCounter* c = (TheCounter*)module;
	if (c->size != sizeof(TheCounter))
		return E_FAIL;
	if (c->f_counter == NULL)
		return E_FAIL;
	if (c->options != 2)
		return E_FAIL;

	awpImage* tmp = NULL;
	if (awpCreateGrayImage(&tmp, width, height, bpp, data) != S_OK)
		return E_FAIL;
	int num_in = 0;
	int num_out = 0;
	HRESULT res = S_OK;
	double s = 0;
	c->f_counter->ProcessImage(tmp, num_in, num_out, s);
	value1 = num_in;
	value2 = num_out;
	awpReleaseImage(&tmp);

	return S_OK;
}
HRESULT  fcounterGetCount(HANDLE hModule, int* in_count, int* out_count)
{
	HANDLE module = hModule;
	TheCounter* c = (TheCounter*)module;
	if (c->size != sizeof(TheCounter))
		return E_FAIL;
	if (c->f_counter == NULL)
		return E_FAIL;
	if (c->options != 2)
		return E_FAIL;

	c->f_counter->GetEventsCount(in_count, out_count);

	return S_OK;
}
HRESULT  fcounterRelease(HANDLE* hModule)
{

	HANDLE module = *hModule;
	TheCounter* c = (TheCounter*)module;
	if (c->size != sizeof(TheCounter))
		return E_FAIL;

	if (c->f_counter != NULL)
	{
		delete c->f_counter;
		c->f_counter = NULL;

		delete c;

		*hModule = NULL;
		return S_OK;
	}
	return E_FAIL;
}
