//---------------------------------------------------------------------------

#ifndef LFAwpImageBufferH
#define LFAwpImageBufferH
//---------------------------------------------------------------------------
#include "_LF.h"

/*
    encapsulate  awpImage as double vector  of vectors of doubles
*/
class TLFAwpImageBuffer : public TLFObject
{
protected:
    awpImage* m_pData;
    int       m_counter;
    double*   m_pixels;

public:
   TLFAwpImageBuffer(unsigned short widht, unsigned short height);
   virtual ~TLFAwpImageBuffer();

   void AddVector(double* data);
   awpImage* GetBuffer();
   double*   GetVector(int index);

   double    GetMinDistanceL2(double* data);
   double    GetMinDistanceL2Norm(double* data, double* norm);
};
#endif
