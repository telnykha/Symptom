//---------------------------------------------------------------------------
#ifndef LFTrainsModuleH
#define LFTrainsModuleH
//---------------------------------------------------------------------------
#include "vautils.h"
#include "_LF.h"
#include "..\..\common\awpFlowTracker\awpflowtracker.h"
/*
	выполняет:
	1. поиск номера железнодорожного вагона. Поиск ведется в первом прямоугольнике, 
	   который задан в переменной zones конструктора. Если в zones прямоугольников нет, 
	   то поиск ведется по всему изображению. 
	2. распознавание номера. Записывает результат в переменную number 
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
	// загрузка нейросетей из файла 
    bool InitOCR(const char* lpFileName);
	// возвращает 1 если номер найден и распознан. 
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
слежение за мишенью.
Изображение мишени хранится в переменной m_model
состояние [m_model_visible] мешени [m_model] можно получить с
использованием функции IsTargetVisible. По умолчанию мишень не видна.
Инициализация модели из файла происходит с использованием функции LoadModel
Инициализация модели из изображения выполняет функция InitModel
Модель можно сохранить на диск используя функцию SaveModel
Наличие модели можно получить используя функцию HasModel

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
	// модель 
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
определение наполенности цистерны. 
наполненность определяется путем анализа измений яркости 
внутри зоны наблюдения. Зона наблюдения задается в 
переменной [m_zones]. зона должна быть одна и иметь тип ZTRect
Анализируется вертикальная интегральная кривая, полученная 
путем суммирования строк втури зоны наблюдения v=F(y) 

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
