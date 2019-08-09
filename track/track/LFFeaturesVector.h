//---------------------------------------------------------------------------

#ifndef LFFeaturesVectorH
#define LFFeaturesVectorH
//---------------------------------------------------------------------------
#include "_LF.h"
/**
    TLFFeaturesVector - вектор признаков ILFFeature.
    Возвращает отклик в виде массива чисел с плавающе точкой.
    Размер массива определяется числом признаков.
*/
class TLFFeaturesVector : public TLFObject
{
protected:
    double* m_data;
    TLFObjectList m_features;
    void UpdateDataBuffer();
public:
    TLFFeaturesVector();
    TLFFeaturesVector(TLFFeaturesVector& vector);
    virtual ~TLFFeaturesVector();

    double* CalcFeatures(TLFImage* image);
    int     NumFeatures();
    double* GetData();

    // io operations
    bool Load(const char* lpFileName);
    bool Save(const char* lpFileName);

    // fetures operation
    ILFFeature* GetFeature(int index);
    bool        DeleteFeature(int index);
    bool        AddFeature(ILFFeature* feature);

   virtual const char* GetName()
   {
	return "TLFFeaturesVector";
   }
};
#endif
