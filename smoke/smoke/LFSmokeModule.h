#ifndef LFSmokeModuleH
#define LFSmokeModuleH
// smoke detector
class TLFSmokeEngine : public ILFDetectEngine
{
protected:
	TLFObjectList m_tmpList;
	virtual void OverlapsFilter(TLFSemanticImageDescriptor* descriptor);
	virtual void InitDetectors();
public:
	TLFSmokeEngine();
	virtual void Clear();
	virtual bool LoadXML(TiXmlElement* parent);
	virtual bool FindObjects();
	virtual TiXmlElement*  SaveXML();
	virtual bool DetectInRect(awpRect* rect);
	virtual const char* GetName()
	{
		return "TLFSmokeEngine";
	}
};
#endif 