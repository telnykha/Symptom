// smoke.cpp : Defines the exported functions for the DLL application.
//
#include "_LF.h"
#include "LFFireModule.h"
#include "fire.h"

typedef struct 
{
	int size;
	TLFFireEngine* s;
	TVAInitParams   params;

}TheFire;

FIRE_API HANDLE		fireCreate(TVAInitParams* params)
{
	TheFire* ts = new TheFire();
	if (ts == NULL)
		return NULL;
	ts->size = sizeof(TheFire);
	ts->s = new TLFFireEngine();


	if (ts->s == NULL)
	{
		delete ts;
		return NULL;
	}

	ts->s->SetResize(true);
	ts->s->SetBaseImageWidth(320);
	ts->s->SetNeedCluster(false);
	ts->s->SetNeedTrack(false);
	ts->s->SetThreshold(params->EventSens);

	ILFObjectDetector* d = ts->s->GetDetector(0);
	if (d != NULL)
	{
		d->SetBaseHeight(4);
		d->SetBaseWidht(4);
	}
	memcpy(&ts->params, params, sizeof(TVAInitParams));
	return (HANDLE)ts;
}
FIRE_API HRESULT	fireProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, bool* result)
{
	TheFire* ts = (TheFire*)hModule;
	if (ts->size != sizeof(TheFire))
		return E_FAIL;
	awpImage* tmp = NULL;
	int ch = bpp == 1 ? 1 : 3;
	awpCreateMultiImage(&tmp, width, height, ch, bpp, data);

	ts->s->SetSourceImage(tmp, true);
	*result = ts->s->GetState() > 0;

	awpReleaseImage(&tmp);
	return S_OK;
}
FIRE_API HRESULT    fireRelease(HANDLE* hModule)
{
	HANDLE module = *hModule;
	TheFire* ts = (TheFire*)module;
	if (ts->size != sizeof(TheFire))
		return E_FAIL;

	delete ts->s;
	delete ts;
	ts = NULL;

	*hModule = NULL;
	return S_OK;
}

FIRE_API HRESULT     fireGetNumElements(HANDLE hModule, int& num)
{
	TheFire* ts = (TheFire*)hModule;
	if (ts->size != sizeof(TheFire))
		return E_FAIL;
	num = ts->s->GetItemsCount();
	return S_OK;
}
FIRE_API HRESULT     fireGetElements(HANDLE hModule, TVAFireElement* elements)
{
	TheFire* ts = (TheFire*)hModule;
	if (ts->size != sizeof(TheFire))
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
