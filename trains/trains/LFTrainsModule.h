//---------------------------------------------------------------------------
#ifndef LFTrainsModuleH
#define LFTrainsModuleH
//---------------------------------------------------------------------------
#include "vautils.h"
#include "_LF.h"
#include "..\..\common\awpFlowTracker\awpflowtracker.h"
/*
	���������:
	1. ����� ������ ���������������� ������. ����� ������� � ������ ��������������, 
	   ������� ����� � ���������� zones ������������. ���� � zones ��������������� ���, 
	   �� ����� ������� �� ����� �����������. 
	2. ������������� ������. ���������� ��������� � ���������� number 
*/
class TLFTrains
{
private:
   // tracking 
	HAWPFLOW        m_handle;
	awpRect*	    m_pBox;
	TLFImage		m_prevImage;
	void TrackBox(awpImage* currentImage);
protected:
	int m_scale;//scale  
	
   TLFDetectEngine m_detector;
   TLFObjectList   m_digits;
   TLFZones        m_zones;
   TLFSemanticImageDescriptor m_res0;
   TLFImage			m_OCRImage;
   awpRect		  GetZoneRect();
   bool  CheckBoxScale(awpRect& r);
   bool  SetupBox();
public:
	TLFTrains(TLFZones& zones);
    virtual ~TLFTrains();
	// �������� ���������� �� ����� 
    bool InitOCR(const char* lpFileName);
	// ���������� 1 ���� ����� ������ � ���������. 
    int ProcessImage(awpImage* img, awpRect* rect, char* number);
    void SetZones(TLFZones& zones);

	double GetThreshold();
    void   SetThreshold(double value);



	/*internals. need to debug*/
    TLFDetectEngine* GetDetector();
	TLFObjectList*   GetDigitsDetectors();
	TLFSemanticImageDescriptor* GetRawSescriptor();
	TLFImage*  GetOCRImage();
};

/*
�������� �� �������.
����������� ������ �������� � ���������� m_model
��������� [m_model_visible] ������ [m_model] ����� �������� �
�������������� ������� IsTargetVisible. �� ��������� ������ �� �����.
������������� ������ �� ����� ���������� � �������������� ������� LoadModel
������������� ������ �� ����������� ��������� ������� InitModel
������ ����� ��������� �� ���� ��������� ������� SaveModel
������� ������ ����� �������� ��������� ������� HasModel

*/
class TLFTrainsTarget
{
private:
	TVAInitParams* m_params;
protected:
	TLFImage		m_model;
	bool			m_model_visible;
	TLFZones        m_zones;
public:
	TLFTrainsTarget(TLFZones& zones, TVAInitParams* params);
	~TLFTrainsTarget();
	// ������ 
	bool InitModel(awpImage* image);
	bool LoadModel(const char* lpFileName);
	bool SaveModel(const char* lpFileName);
	bool HasModel();
	void ClearModel();
	double CompareModel(awpImage* image);
	bool IsTargetVisible();

	TVAInitParams* getModuleParams();
};

/*
����������� ������������ ��������. 
������������� ������������ ����� ������� ������� ������� 
������ ���� ����������. ���� ���������� �������� � 
���������� [m_zones]. ���� ������ ���� ���� � ����� ��� ZTRect
������������� ������������ ������������ ������, ���������� 
����� ������������ ����� ����� ���� ���������� v=F(y) 

*/

class TLFTrainsFullness
{
private: 
	TVAInitParams* m_params;
protected: 
	TLFZones        m_zones;
public: 
	TLFTrainsFullness(TLFZones& zones, TVAInitParams* params);
	~TLFTrainsFullness();

	double ProcessImage(awpImage* image);
};

#endif
