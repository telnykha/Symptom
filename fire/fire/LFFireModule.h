#ifndef LFFireModuleH
#define LFFireModuleH

// fire detector
class TLFFireEngine : public ILFDetectEngine
{
protected:
	TLFObjectList m_tmpList;
	TLFBuffer*    m_buffer;
	int m_push_count;
	virtual void OverlapsFilter(TLFSemanticImageDescriptor* descriptor);
	virtual void InitDetectors();
	double m_threshold;
public:
	TLFFireEngine();
	virtual ~TLFFireEngine();
	virtual void Clear();
	virtual bool LoadXML(TiXmlElement* parent);
	virtual bool FindObjects();
	virtual TiXmlElement*  SaveXML();
	virtual bool DetectInRect(awpRect* rect);
	double GetThreshold();
	void SetThreshold(double value);
	virtual const char* GetName()
	{
		return "TLFFireEngine";
	}
};

#endif //LFFireModuleH