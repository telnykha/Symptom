#ifndef LFTrackH
#define LFTrackH
#include "_LF.h"
#include "va_common.h"

class TLFTrackEngine : public ILFDetectEngine
{
protected:

	double m_minWidth;
	double m_maxWidth;
	double m_minHeight;
	double m_maxHeight;

	TLFObjectList 		m_tmpList;
	TLFImage 			m_foreground;

	virtual void OverlapsFilter(TLFSemanticImageDescriptor* descriptor);
	virtual void InitDetectors();
	virtual void BuildForeground();

	TLFClusterTrack* m_cluster_maker;
public:
	TLFTrackEngine();
	TLFTrackEngine(TVAInitParams& params);
	virtual ~TLFTrackEngine();
	virtual void Clear();
	virtual bool LoadXML(TiXmlElement* parent);
	virtual bool FindObjects();
	virtual TiXmlElement*  SaveXML();
	virtual bool DetectInRect(awpRect* rect);


	awpImage* GetForeground();
	TLFImage* GetForegroundImage();

	float GetMinWidth();
	void SetMinWidth(float value);

	float GetMinHeight();
	void SetMinHeight(float value);

	float GetMaxWidth();
	void SetMaxWidth(float value);

	float GetMaxHeight();
	void  SetMaxHeight(float value);
	virtual const char* GetName()
	{
		return "TLFTrackEngine";
	}
};

#endif //LFTrackH