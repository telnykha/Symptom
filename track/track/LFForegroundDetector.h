//---------------------------------------------------------------------------
#ifndef LFForegroundDetectorH
#define LFForegroundDetectorH
//---------------------------------------------------------------------------

#include "_LF.h"
#include "LFFeaturesVector.h"
#include "LFAwpImageBuffer.h"

class ILFFeaturesClassify : public TLFObject
{
protected:
    TLFFeaturesVector 	m_vector;
    double 				m_threshold;
    TLFRect             m_rect;
public:
    ILFFeaturesClassify();
    ILFFeaturesClassify(TLFFeaturesVector& vector, TLFRect& rect, double threshold);
    virtual bool Classify(TLFImage& image) = 0;

    double GetThreshold();
    void   SetThreshold(double value);

    TLFRect* GetRect();
    void     SetRect(TLFRect& rect);

	virtual const char* GetName()
	{
		return "ILFFeaturesClassify";
	}
};
/*
*/
class TLFNNFeaturesClassify : public ILFFeaturesClassify
{
protected:
    TLFAwpImageBuffer* m_buffer;
	int				   m_delay;
	int				   m_count;
public:
    TLFNNFeaturesClassify(int depth, TLFRect& rect, double threshold, int delay);
    virtual ~TLFNNFeaturesClassify();
    virtual bool Classify(TLFImage& image);
};
/*
    Foreground detector
*/
class TLFForegroundDetector : public ILFObjectDetector
{
protected:
    TLFObjectList m_sensors;
	int m_delay;
public:
    TLFForegroundDetector(int delay, int size = 12);
    virtual ~TLFForegroundDetector();

    virtual bool Init(awpImage* pImage, bool scan = false );
    virtual int  ClassifyRect(awpRect Fragmnet, double* err, int* vect);
	virtual int  Detect();
    virtual  bool	AddStrong(ILFStrong* strong);
    virtual  double  GetThreshold();
    virtual  void SetThreshold(double Value);

	virtual TiXmlElement* SaveXML();
	virtual bool          LoadXML(TiXmlElement* parent);

    bool Save(const char* lpFileName);
    bool Load(const char* lpFileName);


	virtual const char* GetName()
	{
		return "TLFForegroundDetector";
	}
};
#endif
