#ifndef _lf_crowd_module_
#define _lf_crowd_module_
#include "_LF.h"
#include "vautils.h"
class CCrowdEngine : public ILFDetectEngine
{
protected:
	TLFObjectList m_tmpList;
	virtual void OverlapsFilter(TLFSemanticImageDescriptor* descriptor);
	virtual void InitDetectors();
	double m_threshold;
	double m_sum_square;
	unsigned long m_num_frames;
	TVAInitParams		m_params;
	TLFZone*			m_objectSquare;
	TLFImage 			m_foreground;
	void				BuildForeground();
	double				MeanSquare();
public:
	CCrowdEngine(TVAInitParams& params);
	virtual ~CCrowdEngine();
	virtual void Clear();
	virtual bool LoadXML(TiXmlElement* parent);
	virtual bool FindObjects();
	virtual TiXmlElement*  SaveXML();
	virtual bool DetectInRect(awpRect* rect);
	double GetThreshold();
	void SetThreshold(double value);
	awpImage* GetForeground();
	double GetS0();
	double				SourceSquare();
	TVAInitParams* GetParams();
	void AddNewSquare(double s);
};
#endif  //_lf_crowd_module_