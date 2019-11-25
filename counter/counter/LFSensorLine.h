#ifndef _lf_sensorline_h_
#define _lf_sensorline_h_

#include "LFBorderLine.h"

class CSensorLine
{
protected:
	double*			m_data;
	int				m_num;
	CBorderLine*	m_line;
	double			m_data2[256];
public:
	CSensorLine();
	~CSensorLine();

	void Init(IplImage* img, CBorderLine* line);
	void Update(IplImage* module, IplImage* angle);

	int DataLength();
	double* Data();

	void Clear();
};

#endif //_lf_sensorline_h_