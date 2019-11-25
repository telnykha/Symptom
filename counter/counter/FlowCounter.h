#ifndef __flow_counter_h__
#define __flow_counter_h__
#include "va_common.h"
#include "vautils.h"

#include "LFIplFBFlow.h"
#include "LFBorderLine.h"
#include "LFSensorLine.h"
#include "LFSTProcessor.h"

class TLFEvent;
class TLFFlowCounter
{
protected:
	CFBFlow m_flow;
	CSensorLine m_sensor;
	CBorderLine m_border;
	CLFSTProcessor m_processor;
	TLFBuffer m_state_in;
	TLFBuffer m_state_out;

	TLFObjectList m_in_events;
	TLFObjectList m_out_events;
	TLFEvent* m_in_event;
	TLFEvent* m_out_event;

	int m_counter;
	double m_threshold;
	TVAPoint m_sp;
	TVAPoint m_ep;
	bool m_border_done;

	void AnslysisDataLine();
public: 
	TLFFlowCounter(TVAPoint& start, TVAPoint& finish, TVARect& sizes, double eventSens);
	~TLFFlowCounter();
	bool ProcessImage(awpImage* img, int& num_in, int& num_out, double& sq);

	void GetEventsCount(int* in_count, int* out_count);
};

// flow counter c API
HANDLE   fcounterCreate(TVAPoint start, TVAPoint finish, TVARect sizes, double eventSens);
HRESULT  fcounterProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, double& value1, double& value2);
HRESULT  fcounterGetCount(HANDLE hModule, int* in_count, int* out_count);
HRESULT  fcounterRelease(HANDLE* hModule);


#endif // __flow_counter_h__