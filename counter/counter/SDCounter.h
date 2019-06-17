#ifndef _sd_counter_h
#define _sd_counter_h

#include "va_common.h"
#include "vautils.h"
#include "_LF.h"

class CCounter : public ILFDetectEngine
{
protected:
	TLFFGBGDetector		detector;
	TLFImage 			m_foreground;
	TVAInitParams		m_params;
	TLFImage			m_mask;
	TLFAFeature*		m_pAFeature;
	double				m_aValue;
	double				m_hValue;
	int					m_in_count;
	int					m_out_count;
	double				m_start_value;
	std::vector<double> m_hdata;
	std::vector<awpPoint> m_trajectory;
	double				m_aintegral;

	awpPoint		    m_startPoint;
	awpPoint			m_endPoint;
	int				    m_finish_counter;
	bool				m_has_trajectory;

	DWORD				m_dt;
	DWORD				m_event_start_time;
	TLFObjectList		m_event_data;

	void				AddEventData();
	int					EventGetCount();
	TLFImage			m_event_image;

	virtual void InitDetectors();
	virtual bool FindObjects();
	virtual void OverlapsFilter(TLFSemanticImageDescriptor* descriptor){};
	void		 BuildForeground();
	void		 GetStartStopStates(double& v1, double& v2);
	awpPoint     GetCentroid(double& width);
public:
	CCounter(TVAInitParams& params);
	virtual ~CCounter();
	virtual void Clear();
	awpImage* GetForeground();
	awpImage* GetEventImage()
	{
		return this->m_event_image.GetImage();
	}

	void SetAverageBufferSize(int value);
	int GetAverageBufferSize();

	void SetBGStability(int value);
	int GetBGStability();

	double AValue()
	{
		return m_aValue;
	}
	double HValue()
	{
		return m_hValue;
	}

	int InCount()
	{
		return this->m_in_count;
	}
	int OutCount()
	{
		return this->m_out_count;
	}

	awpPoint GetStartPoint()
	{
		return this->m_startPoint;
	}
	awpPoint GetEndPoint()
	{
		return this->m_endPoint;
	}
	bool GetHasTrajectory()
	{
		return this->m_has_trajectory;
	}

	/*Loading from-to xml files*/
	virtual bool LoadXML(TiXmlElement* parent)
	{
		return false;
	}
	virtual TiXmlElement* SaveXML()
	{
		return NULL;
	}
};

#endif //_sd_counter_h