#ifndef _lf_flow_detector_h_ 
#define _lf_flow_detector_h_ 

#include "_LF.h"

class TLFFlowDetector : public ILFObjectDetector
{
protected:
	TLFImage m_flow;
	TLFImage m_prev;
	double m_threshold;
public:
	TLFFlowDetector();

	virtual bool Init(awpImage* pImage, bool scan = false);
	// classification
	virtual int  ClassifyRect(awpRect Fragmnet, double* err, int* vect);
	virtual int  Detect();
	// properties
	virtual int	  GetStagesCount();
	virtual bool  AddStrong(ILFStrong* strong);
	virtual  double GetThreshold();
	virtual  void SetThreshold(double Value);
	virtual int Rating(awpRect& Fragment, double* rating);


	virtual TiXmlElement* SaveXML();
	virtual bool          LoadXML(TiXmlElement* parent);

	virtual const char* GetName()
	{
		return "TLFFlowDetector";
	}
};
#endif 