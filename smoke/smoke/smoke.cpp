// smoke.cpp : Defines the exported functions for the DLL application.
//
#include "smoke.h"
#include "_LF.h"
#include "LFSmokeModule.h"

typedef struct 
{
	int size;
	TLFSmokeEngine* s;
	TVAInitParams   params;
	int state;
	int num;
	DWORD st;
}TheSmoke;

SMOKE_API HANDLE		smokeCreate(TVAInitParams* params)
{
	TheSmoke* ts = new TheSmoke();
	if (ts == NULL)
		return NULL;
	ts->size = sizeof(TheSmoke);
	ts->s = new TLFSmokeEngine();


	if (ts->s == NULL)
	{
		delete ts;
		return NULL;
	}

	ts->s->SetResize(true);
	ts->s->SetBaseImageWidth(320);
	ts->s->SetNeedCluster(false);
	ts->s->SetNeedTrack(false);

	ILFObjectDetector* d = ts->s->GetDetector(0);
	if (d != NULL)
	{
		d->SetBaseHeight(48);
		d->SetBaseWidht(48);
	}
	ts->state = 0;
	ts->num = 0;
	ts->st = 0;
	memcpy(&ts->params, params, sizeof(TVAInitParams));
	return (HANDLE)ts;
}
SMOKE_API HRESULT	smokeProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, bool* result)
{
	TheSmoke* ts = (TheSmoke*)hModule;
	if (ts->size != sizeof(TheSmoke))
		return E_FAIL;
	awpImage* tmp = NULL;
	awpCreateGrayImage(&tmp, width, height, bpp, data);

	ts->s->SetSourceImage(tmp, true);
	*result = false;
	bool state = ts->s->GetItemsCount() > 15 - 10*ts->params.EventSens;
	if (state)
	{

		ts->state = 1;
		if (ts->num == 0)
			ts->st = GetTickCount();
		else
		{
			if (GetTickCount() - ts->st > ts->params.EventTimeSens)
				*result = true;
		}
		ts->num++;
	}
	else
	{
		ts->state = 0;
		ts->num = 0;
	}
#ifdef _DEBUG 
	printf("items count %i\n", ts->s->GetItemsCount());
#endif
	awpReleaseImage(&tmp);
	return S_OK;
}
SMOKE_API HRESULT    smokeRelease (HANDLE* hModule )
{
	HANDLE module = *hModule;
	TheSmoke* ts = (TheSmoke*)module;
	if (ts->size != sizeof(TheSmoke))
		return E_FAIL;

	delete ts->s;
	delete ts;
	ts = NULL;

	*hModule = NULL;
	return S_OK;
}

SMOKE_API HRESULT    smokeGetNumElements(HANDLE hModule, int& num)
{
	TheSmoke* ts = (TheSmoke*)hModule;
	if (ts->size != sizeof(TheSmoke))
		return E_FAIL;
	num = ts->s->GetItemsCount();
	return S_OK;

}
SMOKE_API HRESULT    smokeGetElements(HANDLE hModule, TVASmokeElement* elements)
{
	TheSmoke* ts = (TheSmoke*)hModule;
	if (ts->size != sizeof(TheSmoke))
		return E_FAIL;
	int num = ts->s->GetItemsCount();
	for (int i = 0; i < num; i++)
	{
		TLFDetectedItem* di = ts->s->GetItem(i);
		TLFRect* r = di->GetBounds();
		elements[i].XPos = r->GetRect().left;
		elements[i].YPos = r->GetRect().top;
		elements[i].Width = r->Width();
		elements[i].Height = r->Height();
	}
	return S_OK;
}

