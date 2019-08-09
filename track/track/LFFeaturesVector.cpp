//---------------------------------------------------------------------------

#pragma hdrstop

#include "LFFeaturesVector.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


TLFFeaturesVector::TLFFeaturesVector() : TLFObject()
{
    m_data = NULL;
}
TLFFeaturesVector::TLFFeaturesVector(TLFFeaturesVector& vector)
{
}

TLFFeaturesVector::~TLFFeaturesVector()
{
    if (m_data != NULL)
    {
        free(m_data);
        m_data = NULL;
    }
}

double* TLFFeaturesVector::CalcFeatures(TLFImage* image)
{
    if (image == NULL)
        return NULL;
    for (int i = 0; i < this->m_features.GetCount(); i++)
    {
        ILFFeature* f = (ILFFeature*)this->m_features.Get(i);
        m_data[i] = f->fCalcValue(image);
    }
    return this->m_data;
}
int     TLFFeaturesVector::NumFeatures()
{
    return this->m_features.GetCount();
}
double* TLFFeaturesVector::GetData()
{
    return this->m_data;
}

bool TLFFeaturesVector::Load(const char* lpFileName)
{
    if (lpFileName == NULL)
        return false;

    TiXmlDocument doc;
    if (!doc.LoadFile(lpFileName))
        return false;
    TiXmlElement* e = doc.FirstChildElement();
    if (e == NULL)
        return false;
    if (strcmp(e->Value(), this->GetName()) != 0)
        return false;

    for(TiXmlNode* child = e->FirstChild(); child; child = child->NextSibling() )
    {
         ILFFeature* f = LFCreateFeature(child->ToElement());
         if (!f->LoadXML(child->ToElement()))
         {
            delete f;
            return false;
         }
         this->AddFeature(f);
    }


    return true;
}
bool TLFFeaturesVector::Save(const char* lpFileName)
{
    TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );

    TiXmlElement* e = new TiXmlElement(this->GetName());

    for (int i = 0; i < this->m_features.GetCount(); i++)
    {
      ILFFeature* f = (ILFFeature*)this->m_features.Get(i);
      if (!f->SaveXML(e))
        return false;
    }

    doc.LinkEndChild(e);
    return doc.SaveFile(lpFileName);
}

// fetures operation
ILFFeature* TLFFeaturesVector::GetFeature(int index)
{
    ILFFeature* f = (ILFFeature*)this->m_features.Get(index);
    return f;
}
bool        TLFFeaturesVector::DeleteFeature(int index)
{
    if (index < 0 || index > m_features.GetCount())
        return false;
    this->m_features.Delete(index);
    UpdateDataBuffer();
    return true;
}
bool        TLFFeaturesVector::AddFeature(ILFFeature* feature)
{
    if (feature == NULL)
        return false;
    ILFFeature* f = LFCreateFeature(feature);
    if (f == NULL)
        return false;
    this->m_features.Add(f);
    UpdateDataBuffer();
    return true;
}
// data buffer
void TLFFeaturesVector::UpdateDataBuffer()
{
    if (m_data != NULL)
    {
        free(m_data);
        m_data = NULL;
    }

    if (m_features.GetCount() > 0)
        m_data = (double*)malloc(m_features.GetCount()*sizeof(double));
}

