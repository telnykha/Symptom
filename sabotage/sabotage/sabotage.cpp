// sabotage.cpp : Defines the exported functions for the DLL application.
//

#include "math.h"
#include "sabotage.h"
#include <stdio.h>
#include "_LF.h"

typedef struct 
{
	int size;
	TLFSabotage* s;

}TheSabotage;

SABOTAGE_API HANDLE		sabotageCreate(TVAInitParams* params)
{
	TheSabotage* ts = new TheSabotage();
	if (ts == NULL)
		return NULL;
	ts->size = sizeof(TheSabotage);
	ts->s = new TLFSabotage(params->EventSens, params->EventTimeSens);
	if (ts->s == NULL)
	{
		delete ts;
		return NULL;
	}
	
	return (HANDLE)ts;
}
SABOTAGE_API HRESULT	sabotageProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, bool* result)
{
	TheSabotage* ts = (TheSabotage*)hModule;
	if (ts->size != sizeof(TheSabotage))
		return E_FAIL;
	HRESULT res = S_OK;
	awpImage* tmp = NULL;
	if (awpCreateGrayImage(&tmp, width, height, bpp, data) != AWP_OK)
		return E_FAIL;
	if (!ts->s->PorocessImage(tmp, result))
		res = E_FAIL;
	awpReleaseImage(&tmp);
	return res;
}

SABOTAGE_API HRESULT	sabotageState(HANDLE hModule, int* state)
{
	TheSabotage* ts = (TheSabotage*)hModule;
	if (ts->size != sizeof(TheSabotage))
		return E_FAIL;
	*state = ts->s->GetState();
	return S_OK;
}
SABOTAGE_API HRESULT    sabotageRelease (HANDLE* hModule )

{
	HANDLE module = *hModule;
	TheSabotage* ts = (TheSabotage*)module;
	if (ts->size != sizeof(TheSabotage))
		return E_FAIL;

	delete ts->s;
	delete ts;
	ts = NULL;

	*hModule = NULL;
	return S_OK;
}

