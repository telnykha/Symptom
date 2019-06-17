#ifndef _event_log_h_
#define _event_log_h_
#include "..\..\..\TinyXML\src\tinyxml.h"
#include <string>
class CEventLog
{
protected:
	TiXmlDocument m_doc;
	std::string   m_fileName;
	TiXmlElement* m_element;
public:
	CEventLog(const char* lpFileName)
	{
		m_fileName = lpFileName;
		TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
		m_doc.LinkEndChild(decl);
		m_element = new TiXmlElement("Events");
	}
	~CEventLog()
	{
		Close();
	}
	void Close()
	{
		m_doc.LinkEndChild(m_element);
		m_doc.SaveFile(m_fileName.c_str());
	}
	void AddEvent(int frame, int duration, double aintegr, double hintegr)
	{
		TiXmlElement* elem = new TiXmlElement("event");
		elem->SetAttribute("frame", frame);
		elem->SetAttribute("d", duration);
		elem->SetDoubleAttribute("a", aintegr);
		elem->SetDoubleAttribute("h", hintegr);

//		m_element->LinkEndChild(elem);
	}
};

#endif //_event_log_h_