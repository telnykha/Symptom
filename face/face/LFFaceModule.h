#ifndef LFFaceModuleH
#define LFFaceModuleH


// face detector
class TLFFaceEngine : public ILFDetectEngine
{
protected:
	bool				m_needTilt;
	bool				m_needSemiFront;
	bool				m_needSemiFrontTilt;
	virtual void OverlapsFilter(TLFSemanticImageDescriptor* descriptor);
	virtual void InitDetectors();
	TLFList  m_tmpList;
public:
	TLFFaceEngine(bool tilt = false, bool profile = true, bool profile_tilt = false);
	virtual ~TLFFaceEngine();
	virtual void Clear();

	virtual bool DetectInRect(awpRect* rect);

	virtual bool LoadXML(TiXmlElement* parent);
	virtual bool FindObjects();
	virtual TiXmlElement*  SaveXML();

	virtual const char* GetName()
	{
		return "TLFFaceEngine";
	}
};

#endif 