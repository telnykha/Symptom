// counter.cpp: определяет экспортированные функции для приложения DLL.
//
#include "va_common.h"
#include "vautils.h"
#include "_LF.h"
#include "counter.h"
#include "lftcounter.h"
#include "SDCounter.h"
#include "zones.cpp"


//---------------------------- SINGLE_DOOR_COUNTER   API ----------------------------------------
HANDLE   counterCreate(TVAInitParams* params);
HRESULT  counterProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, double& value1, double& value2, int& state);
HRESULT  counterGetCount(HANDLE hModule, int* in_count, int* out_count);
HRESULT	 counterForeground(HANDLE hModule, int width, int height, unsigned char* data);
HRESULT  counterRestart(HANDLE hModule);
HRESULT  counterRelease(HANDLE* hModule);
HRESULT  counterGetLastTrajectory(HANDLE hModule, TVAPoint* startPoint, TVAPoint* endPoint, bool* has_trajectory);
HRESULT  counterGetEventImageSize(HANDLE hModule, int& width, int& height);
HRESULT  counterGetEventImage(HANDLE hModule, int width, int height, double* data);

typedef struct
{
	int size;
	int options;
	CCounter*		c_counter; // sigle door counter 
	TLFTCounter*	t_counter; // trajectory counter 
}TheCounter;

HANDLE   counterCreate(TVAInitParams* params)
{
	if (params == NULL)
		return NULL;
	bool create_zone = false;
	if (params->NumZones == 0)
	{
		create_zone = true;
	}
	else
		if (params->NumZones != 2 && params->Zones[0].IsRect == false)
		return NULL;

	TheCounter* hcounter = new TheCounter();
	
	hcounter->size = sizeof(TheCounter);
	hcounter->options = SINGLE_DOOR_COUNTER;
	hcounter->t_counter = NULL;

	hcounter->c_counter = new CCounter(*params);
	hcounter->c_counter->SetNeedTrack(false);
	hcounter->c_counter->SetNeedCluster(false);
	hcounter->c_counter->SetResize(true);
	hcounter->c_counter->SetBaseImageWidth(320);
	hcounter->c_counter->SetAverageBufferSize(5);
	hcounter->c_counter->SetBGStability(1);
	ILFObjectDetector* s = hcounter->c_counter->GetDetector(0);
	s->SetBaseWidht(8);
	s->SetBaseHeight(8);
	s->SetThreshold(params->EventSens);

	return (HANDLE)hcounter;
}
HRESULT  counterProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, double& value1, double& value2, int& state)
{
	HANDLE module = hModule;
	TheCounter* hcounter = (TheCounter*)module;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;
	if (hcounter->c_counter == NULL)
		return E_FAIL;
	awpImage* tmp = NULL;
	awpCreateGrayImage(&tmp, width, height, bpp, data);

	hcounter->c_counter->SetSourceImage(tmp, true);
	value1 = hcounter->c_counter->AValue();
	value2 = hcounter->c_counter->HValue();
	state = hcounter->c_counter->GetState();
	awpReleaseImage(&tmp);
	return S_OK;
}
HRESULT  counterGetCount(HANDLE hModule, int* in_count, int* out_count)
{
	HANDLE module = hModule;
	TheCounter* hcounter = (TheCounter*)module;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;
	*in_count = hcounter->c_counter->InCount();
	*out_count = hcounter->c_counter->OutCount();
	return S_OK;
}
HRESULT  counterGetLastTrajectory(HANDLE hModule, TVAPoint* startPoint, TVAPoint* endPoint, bool* has_trajectory)
{
	HANDLE module = hModule;
	TheCounter* hcounter = (TheCounter*)module;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;
	*has_trajectory = false;
	startPoint->X = 0;
	startPoint->Y = 0;
	endPoint->X = 0;
	endPoint->Y = 0;
	if (hcounter->c_counter->GetHasTrajectory())
	{
		*has_trajectory = true;
		double rc = hcounter->c_counter->GetResizeCoef();
		awpPoint p = hcounter->c_counter->GetStartPoint();
		startPoint->X = (float)(p.X*rc);
		startPoint->Y = (float)(p.Y*rc);
		p = hcounter->c_counter->GetEndPoint();
		endPoint->X = (float)(p.X*rc);
		endPoint->Y = (float)(p.Y*rc);
	}
	return S_OK;
}
HRESULT  counterRestart(HANDLE hModule)
{
	HANDLE module = hModule;
	TheCounter* hcounter = (TheCounter*)module;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;
	hcounter->c_counter->Clear();
	return S_OK;
}
HRESULT  counterRelease(HANDLE* hModule)
{
	HANDLE module = *hModule;
	TheCounter* hcounter = (TheCounter*)module;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;


	delete hcounter->c_counter;
	hcounter->c_counter = NULL;

	delete hcounter;

	*hModule = NULL;
	return S_OK;
}

HRESULT	 counterForeground(HANDLE hModule, int width, int height, unsigned char* data)
{
	TheCounter*  hcounter  = (TheCounter*)hModule;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;

	awpImage* img = hcounter->c_counter->GetForeground();
	if (img != NULL)
	{
		awpImage* tmp = NULL;
		awpCopyImage(img, &tmp);

		if (tmp->sSizeX != width || tmp->sSizeY != height)
			awpResize(tmp, width, height);
		memcpy(data, tmp->pPixels, width*height*sizeof(unsigned char));
		_AWP_SAFE_RELEASE_(tmp);
	}

	return S_OK;
}

HRESULT  counterGetEventImageSize(HANDLE hModule, int& width, int& height)
{
	TheCounter*  hcounter = (TheCounter*)hModule;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;
	width = 0;
	height = 0;
	awpImage* img = hcounter->c_counter->GetEventImage();
	if (img != NULL)
	{
		width = img->sSizeX;
		height = img->sSizeY;
	}
	return S_OK;
}
HRESULT  counterGetEventImage(HANDLE hModule, int width, int height, double* data)
{
	TheCounter*  hcounter = (TheCounter*)hModule;
	if (hcounter->size != sizeof(TheCounter))
		return E_FAIL;

	awpImage* img = hcounter->c_counter->GetEventImage();
	if (img != NULL)
	{
		if (width != img->sSizeX)
			return E_FAIL;

		if (height != img->sSizeY)
			return E_FAIL;
		
		if (data == NULL)
			return E_FAIL;

		memcpy(data, img->pPixels, width*height*sizeof(double));
	}
	return S_OK;

}

//---------------------------------------
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
	if (c->t_counter == NULL)
	{
		double v1 = 0;
		double v2 = 0;
		int    state = 0;
		HRESULT res = counterProcess(hModule, width, height, bpp, data, v1, v2, state);
		//todo: check state 

		return res;
	}
	else
	{
		// process data with t_counter 
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