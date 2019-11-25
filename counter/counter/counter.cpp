// counter.cpp: определяет экспортированные функции для приложения DLL.
//
#include "va_common.h"
#include "vautils.h"
#include "_LF.h"
#include "counter.h"
#include "lftcounter.h"
#include "SDCounter.h"
#include "zones.cpp"
#include "FlowCounter.h"
#include "_counter.h"

extern "C"
{
	#pragma comment(lib, "opencv_world300.lib")
}

/*
	creates trajectory counter 
*/
COUNTER_API HANDLE   tcounterCreate(TVAPoint start, TVAPoint finish, TVARect sizes, double eventSens)
{
	if (eventSens < 0 || eventSens > 1)
		return NULL;
	if (start.X < 0 || start.X >100 || start.Y < 0 || start.Y > 100)
		return NULL;
	if (finish.X < 0 || finish.X >100 || finish.Y < 0 || finish.Y > 100)
		return NULL;
	TVAPoint s = sizes.LeftTop;
	if (s.X < 0 || s.X >100 || s.Y < 0 || s.Y > 100)
		return NULL;
	s = sizes.RightBottom;
	if (s.X < 0 || s.X >100 || s.Y < 0 || s.Y > 100)
		return NULL;

	TheCounter* c = new TheCounter();
	c->size  = sizeof(TheCounter);
	c->options = TRAJECTORY_COUNTER;
	c->c_counter = NULL;
	c->f_counter = NULL;
	c->t_counter = new TLFTCounter(start, finish, sizes, eventSens);

	return (HANDLE)c;
}

COUNTER_API HANDLE   tcounterCreateEx(TVAPoint start, TVAPoint finish, TVARect sizes, double eventSens, int options)
{
	switch (options)
	{
		case SINGLE_DOOR_COUNTER:
		{
			TVAInitParams params;
			memset(&params, 0, sizeof(TVAInitParams));
			params.EventSens = eventSens;
			params.NumZones = 2;
			params.Zones = (TVAZone*)malloc(params.NumZones*sizeof(TVAZone));
			TVARect zone;
			zone.LeftTop = start;
			zone.RightBottom = finish;
			params.Zones[0].IsRect = true;
			memcpy(&params.Zones[0].Rect, &zone, sizeof(TVARect));

			params.Zones[1].IsRect = true;
			memcpy(&params.Zones[1].Rect, &sizes, sizeof(TVARect));

			HANDLE result = counterCreate(&params);
			free(params.Zones);
			return result;
		}
		case TRAJECTORY_COUNTER:
		{
			return tcounterCreate(start, finish, sizes, eventSens); 
		}
		case FLOW_COUNTER:
		{
			return fcounterCreate(start, finish, sizes, eventSens);
		}
		default:
			return NULL;
	}
}

COUNTER_API HRESULT  tcounterProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, double& value)
{
	HANDLE module = hModule;
	TheCounter* c = (TheCounter*)module;
	if (c->size != sizeof(TheCounter))
		return E_FAIL;
	// check the current counter 
	if (c->c_counter != NULL)
	{
		double v1 = 0;
		double v2 = 0;
		int    state = 0;
		HRESULT res = counterProcess(hModule, width, height, bpp, data, v1, v2, state);
		//todo: check state 

		return res;
	}
	else if (c->t_counter != NULL)
	{
		// process data with trajectory counter
		awpImage* tmp = NULL;
		if (awpCreateGrayImage(&tmp, width, height, bpp, data) != S_OK)
			return E_FAIL;
		int num_in = 0;
		int num_out = 0;
		value = 0;
		HRESULT res = S_OK;
		double s = 0;
		if (c->t_counter->ProcessImage(tmp, num_in, num_out, s))
		{
			value = num_in + num_out;
		}
		else
			res = E_FAIL;
		awpReleaseImage(&tmp);
		return res;
	}
	else if (c->f_counter != NULL)
	{
		// process data with flow counter 

		double v1 = 0;
		double v2 = 0;
		HRESULT res = fcounterProcess(hModule, width, height, bpp, data, v1, v2);
		value = v1 + v2;
		return res;
	}
	else
		return E_FAIL;
}

COUNTER_API HRESULT  tcounterRelease(HANDLE* hModule)
{
	HANDLE module = *hModule;
	TheCounter* c = (TheCounter*)module;
	if (c->size != sizeof(TheCounter))
		return E_FAIL;
	if (c->t_counter != NULL)
	{
		delete c->t_counter;
		c->t_counter = NULL;

		delete c;

		*hModule = NULL;
		return S_OK;
	}
	else if (c->f_counter != NULL)
		return fcounterRelease(hModule);
	else
		return counterRelease(hModule);
}

COUNTER_API HRESULT  tcounterGetState(HANDLE hModule, double& value1, double& value2, int& state)
{
	HANDLE module = hModule;
	TheCounter* hcounter = (TheCounter*)module;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;
	if (hcounter->c_counter != NULL)
	{
		value1 = hcounter->c_counter->AValue();
		value2 = hcounter->c_counter->HValue();
		state = hcounter->c_counter->GetState();
	}
	else
	{
		//todo:
	}

	return S_OK;
}

COUNTER_API HRESULT  tcounterGetCount(HANDLE hModule, int* in_count, int* out_count)
{
	HANDLE module = hModule;
	TheCounter* hcounter = (TheCounter*)module;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;
	if (hcounter->c_counter != NULL)
	{
		*in_count = hcounter->c_counter->InCount();
		*out_count = hcounter->c_counter->OutCount();
	}
	else
	{
		//todo:
	}

	return S_OK;
}

COUNTER_API HRESULT  tcounterGetForeground(HANDLE hModule, int width, int height, unsigned char* data)
{
	HANDLE module = hModule;
	TheCounter* hcounter = (TheCounter*)module;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;
	if (hcounter->c_counter == NULL)
	{
		//todo: 
		return S_OK;
	}
	else
		return counterForeground(hModule,width, height, data);
	return S_OK;
}

COUNTER_API HRESULT  tcounterGetLastTrajectory(HANDLE hModule, TVAPoint* startPoint, TVAPoint* endPoint, bool* has_trajectory)
{
	HANDLE module = hModule;
	TheCounter* hcounter = (TheCounter*)module;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;
	if (hcounter->c_counter == NULL)
	{
		//todo: 
		return S_OK;
	}
	else
		return counterGetLastTrajectory(hModule, startPoint,endPoint,has_trajectory);
}