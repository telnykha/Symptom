#ifndef _lf_stprocessor_h_
#define _lf_stprocessor_h_
#include "_LF.h"
class CLFSTProcessor
{
protected:
	TLFObjectList m_lines;
	int m_width;
	int m_length;

public:
	CLFSTProcessor(int length, int width);
	~CLFSTProcessor();

	void AddData(double* data);
	void Clear();

	awpImage* GetST();
	awpImage* GetVST();
	awpImage* GetBSTIn();
	awpImage* GetBSTOut();
};
#endif //_lf_stprocessor_h_