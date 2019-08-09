//---------------------------------------------------------------------------

#pragma hdrstop

#include "LFAwpImageBuffer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


TLFAwpImageBuffer::TLFAwpImageBuffer(unsigned short width, unsigned short height)
{
    awpCreateImage(&this->m_pData, width, height, 1, AWP_DOUBLE);
    m_counter = 0;
    m_pixels =  (double*)m_pData->pPixels;
}
TLFAwpImageBuffer::~TLFAwpImageBuffer()
{
    _AWP_SAFE_RELEASE_(this->m_pData)
}

void TLFAwpImageBuffer::AddVector(double* data)
{
    if (m_counter < m_pData->sSizeY)
    {
        double* p = (m_pixels + m_counter*m_pData->sSizeX);
        memcpy(p, data, m_pData->sSizeX*sizeof(double));
        m_counter++;
    }
    else
	{
        memcpy(m_pixels, m_pixels + m_pData->sSizeX, m_pData->sSizeX*(m_pData->sSizeY-1)*sizeof(double));
        double* p = m_pixels + m_pData->sSizeX*(m_pData->sSizeY-1);
        memcpy(p, data, m_pData->sSizeX*sizeof(double));
    }
}

awpImage* TLFAwpImageBuffer::GetBuffer()
{
    return m_pData;
}

double*   TLFAwpImageBuffer::GetVector(int index)
{
    if (index  < 0 || index > m_pData->sSizeY)
        return NULL;

}

double    TLFAwpImageBuffer::GetMinDistanceL2(double* data)
{
    if (this->m_counter < this->m_pData->sSizeY)
        return -0.1;
    double min_d = 1e10;
    for (unsigned short i = 0; i < m_pData->sSizeY; i++)
    {
        double* p = m_pixels + i*m_pData->sSizeX;
        double  D = 0;
        for (unsigned short j = 0; j < m_pData->sSizeX; j++)
        {
            D += (data[j] - p[j])*(data[j] - p[j]);
        }
        D = sqrt(D);
        if (D < min_d)
            min_d = D;
    }
    return min_d;
}

double    TLFAwpImageBuffer::GetMinDistanceL2Norm(double* data, double* norm)
{
    if (this->m_counter < this->m_pData->sSizeY)
        return -0.1;
    double min_d = 1e10;
    for (unsigned short i = 0; i < m_pData->sSizeY; i++)
    {
        double* p = m_pixels + i*m_pData->sSizeX;
        double  D = 0;
        for (unsigned short j = 0; j < m_pData->sSizeX; j++)
        {
            D += (data[j]/norm[j] - p[j]/norm[j])*(data[j]/norm[j] - p[j]/norm[j]);
        }
        D = sqrt(D);
        if (D < min_d)
            min_d = D;
    }
    return min_d;
}

