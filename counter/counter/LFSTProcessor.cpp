#include "LFSTProcessor.h"

typedef struct palEntry
{
	double r;
	double g;
	double b;
};

const palEntry HeatmapPal[128]
{
	{0.0, 0.0, 74.0},
	{ 0.0, 0.0, 78.0 },
	{ 0.0, 0.0, 78.0 },
	{ 0.0, 0.0, 78.0 },
	{ 0.0, 0.0, 82.0 },
	{ 0.0, 0.0, 86.0 },
	{ 0.0, 0.0, 90.0 },
	{ 0.0, 0.0, 94.0 },
	{ 0.0, 0.0, 98.0 },
	{ 0.0, 0.0, 102.0 },
	{ 0.0, 0.0, 106.0 },
	{ 0.0, 0.0, 110.0 },
	{ 0.0, 0.0, 114.0 },
	{ 0.0, 0.0, 118.0 },
	{ 0.0, 0.0, 122.0 },
	{ 0.0, 0.0, 126.0 },
	{ 0.0, 2.0, 128.0 },
	{ 0.0, 6.0, 128.0 },
	{ 0.0, 10.0, 128.0 },
	{ 0.0, 14.0, 128.0 },
	{ 0.0, 18.0, 128.0 },
	{ 0.0, 22.0, 128.0 },
	{ 0.0, 26.0, 128.0 },
	{ 0.0, 30.0, 128.0 },
	{ 0.0, 34.0, 128.0 },
	{ 0.0, 38.0, 128.0 },
	{ 0.0, 42.0, 128.0 },
	{ 0.0, 46.0, 128.0 },
	{ 0.0, 50.0, 128.0 },
	{ 0.0, 54.0, 128.0 },
	{ 0.0, 58.0, 128.0 },
	{ 0.0, 62.0, 128.0 },
	{ 0.0, 66.0, 128.0 },
	{ 0.0, 70.0, 128.0 },
	{ 0.0, 74.0, 128.0 },
	{ 0.0, 78.0, 128.0 },
	{ 0.0, 82.0, 128.0 },
	{ 0.0, 86.0, 128.0 },
	{ 0.0, 90.0, 128.0 },
	{ 0.0, 94.0, 128.0 },
	{ 0.0, 98.0, 128.0 },
	{ 0.0, 102.0, 128.0 },
	{ 0.0, 106.0, 128.0 },
	{ 0.0, 110.0, 128.0 },
	{ 0.0, 114.0, 128.0 },
	{ 0.0, 118.0, 128.0 },
	{ 0.0, 122.0, 128.0 },
	{ 0.0, 126.0, 128.0 },
	{ 2.0, 128.0, 126.0 },
	{ 6.0, 128.0, 122.0 },
	{ 10.0, 128.0, 118.0 },
	{ 14.0, 128.0, 114.0 },
	{ 18.0, 128.0, 110.0 },
	{ 22.0, 128.0, 106.0 },
	{ 26.0, 128.0, 102.0 },
	{ 30.0, 128.0, 98.0 },
	{ 34.0, 128.0, 94.0 },
	{ 38.0, 128.0, 90.0 },
	{ 42.0, 128.0, 86.0 },
	{ 46.0, 128.0, 82.0 },
	{ 50.0, 128.0, 78.0 },
	{ 54.0, 128.0, 74.0 },
	{ 58.0, 128.0, 70.0 },
	{ 62.0, 128.0, 66.0 },
	{ 66.0, 128.0, 62.0 },
	{ 70.0, 128.0, 58.0 },
	{ 74.0, 128.0, 54.0 },
	{ 78.0, 128.0, 50.0 },
	{ 82.0, 128.0, 46.0 },
	{ 86.0, 128.0, 42.0 },
	{ 90.0, 128.0, 38.0 },
	{ 94.0, 128.0, 34.0 },
	{ 98.0, 128.0, 30.0 },
	{ 102.0, 128.0, 26.0 },
	{ 106.0, 128.0, 22.0 },
	{ 110.0, 128.0, 18.0 },
	{ 114.0, 128.0, 14.0 },
	{ 118.0, 128.0, 10.0 },
	{ 122.0, 128.0, 6.0 },
	{ 126.0, 128.0, 2.0 },
	{ 128.0, 126.0, 0.0 },
	{ 128.0, 122.0, 0.0 },
	{ 128.0, 118.0, 0.0 },
	{ 128.0, 114.0, 0.0 },
	{ 128.0, 110.0, 0.0 },
	{ 128.0, 106.0, 0.0 },
	{ 128.0, 102.0, 0.0 },
	{ 128.0, 98.0, 0.0 },
	{ 128.0, 94.0, 0.0 },
	{ 128.0, 90.0, 0.0 },
	{ 128.0, 86.0, 0.0 },
	{ 128.0, 82.0, 0.0 },
	{ 128.0, 78.0, 0.0 },
	{ 128.0, 74.0, 0.0 },
	{ 128.0, 70.0, 0.0 },
	{ 128.0, 66.0, 0.0 },
	{ 128.0, 62.0, 0.0 },
	{ 128.0, 58.0, 0.0 },
	{ 128.0, 54.0, 0.0 },
	{ 128.0, 50.0, 0.0 },
	{ 128.0, 46.0, 0.0 },
	{ 128.0, 42.0, 0.0 },
	{ 128.0, 38.0, 0.0 },
	{ 128.0, 34.0, 0.0 },
	{ 128.0, 30.0, 0.0 },
	{ 128.0, 26.0, 0.0 },
	{ 128.0, 22.0, 0.0 },
	{ 128.0, 18.0, 0.0 },
	{ 128.0, 14.0, 0.0 },
	{ 128.0, 10.0, 0.0 },
	{ 128.0, 6.0, 0.0 },
	{ 128.0, 2.0, 0.0 },
	{ 126.0, 0.0, 0.0 },
	{ 122.0, 0.0, 0.0 },
	{ 118.0, 0.0, 0.0 },
	{ 114.0, 0.0, 0.0 },
	{ 110.0, 0.0, 0.0 },
	{ 106.0, 0.0, 0.0 },
	{ 102.0, 0.0, 0.0 },
	{ 98.0, 0.0, 0.0 },
	{ 94.0, 0.0, 0.0 },
	{ 90.0, 0.0, 0.0 },
	{ 86.0, 0.0, 0.0 },
	{ 82.0, 0.0, 0.0 },
	{ 78.0, 0.0, 0.0 },
	{ 74.0, 0.0, 0.0 },
	{ 70.0, 0.0, 0.0 },
	{ 66.0, 0.0, 0.0 }
};


CLFSTProcessor::CLFSTProcessor(int length, int width)
{
	m_width = width;
	m_length = length;
}

CLFSTProcessor::~CLFSTProcessor()
{

}

void CLFSTProcessor::AddData(double* data)
{
	TLFBuffer* b = new TLFBuffer(m_width, 0);
	b->SetData(data);
	if (m_lines.GetCount() < m_length)
		m_lines.Add(b);
	else
	{
		m_lines.Delete(0);
		m_lines.Add(b);
	}
}

void CLFSTProcessor::Clear()
{
	m_lines.Clear();
}

awpImage* CLFSTProcessor::GetST()
{
	awpImage* result = NULL;
	if (awpCreateImage(&result, m_width, m_length, 1, AWP_DOUBLE) != AWP_OK)
		return result;
	AWPDOUBLE* d = (AWPDOUBLE*)result->pPixels;
	int c = 0;
	for (int i = m_length - 1; i >= 0; i--)
	{
		if (c >= m_lines.GetCount())
			break;
		TLFBuffer* buffer = (TLFBuffer*)m_lines.Get(c);
		c++;

		AWPDOUBLE* p = &d[i*m_width];
		memcpy(p, buffer->GetData(), m_width*sizeof(double));
	}

	return result;
}
awpImage* CLFSTProcessor::GetVST()
{
	awpImage* result = NULL;
	if (awpCreateImage(&result, m_width, m_length, 3, AWP_BYTE) != AWP_OK)
		return result;
	AWPBYTE* b = (AWPBYTE*)result->pPixels;
	int c = 0;
	for (int i = m_length - 1; i >= 0; i--)
	{
		if (c >= m_lines.GetCount())
			break;
		TLFBuffer* buffer = (TLFBuffer*)m_lines.Get(c);
		c++;
		double* d = buffer->GetData();
		for (int j = 0; j < m_width; j++)
		{
			int index = (128 + 20*d[j]) / 2;
			if (index < 0) index = 0;
			if (index > 127) index = 127;
			b[3 * i*m_width + 3 * j] =  HeatmapPal[index].r;
			b[3 * i*m_width + 3 * j + 1] = HeatmapPal[index].g;
			b[3 * i*m_width + 3 * j + 2] =  HeatmapPal[index].b;
		}
	}


	return result;
}
awpImage* CLFSTProcessor::GetBSTIn()
{
	awpImage* result = NULL;
	if (awpCreateImage(&result, m_width, m_length, 1, AWP_BYTE) != AWP_OK)
		return result;
	AWPBYTE* b = (AWPBYTE*)result->pPixels;
	int c = 0;
	for (int i = m_length - 1; i >= 0; i--)
	{
		if (c >= m_lines.GetCount())
			break;
		TLFBuffer* buffer = (TLFBuffer*)m_lines.Get(c);
		c++;
		double* d = buffer->GetData();
		for (int j = 0; j < m_width; j++)
		{
			b[i*m_width + j] = d[j] > 0 ? 20 * d[j] > 255 ? 255 : 20 * d[j] : 0;
		}
	}
	return result;
}
awpImage* CLFSTProcessor::GetBSTOut()
{
	awpImage* result = NULL;
	if (awpCreateImage(&result, m_width, m_length, 1, AWP_BYTE) != AWP_OK)
		return result;
	AWPBYTE* b = (AWPBYTE*)result->pPixels;
	int c = 0;
	for (int i = m_length - 1; i >= 0; i--)
	{
		if (c >= m_lines.GetCount())
			break;
		TLFBuffer* buffer = (TLFBuffer*)m_lines.Get(c);
		c++;
		double* d = buffer->GetData();
		for (int j = 0; j < m_width; j++)
		{
			b[i*m_width + j] = d[j] < 0 ? -20 * d[j] < -255 ? 255 : -20 * d[j]  : 0;
		}
	}
	return result;
}