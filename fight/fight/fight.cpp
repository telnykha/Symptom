#include "fight.h"
#include "_LF.h"
#include "LFFightModule.h"
typedef struct
{
	int size;
	CFightEngine* engine;
}TheFight;

FIGHT_API HANDLE   fightCreate(TVAInitParams* params)
{
	if (params == NULL)
		return NULL;

	TheFight* fight = new TheFight();
	if (fight == NULL)
		return NULL;

	fight->size = sizeof(TheFight);
	fight->engine = new CFightEngine(*params);
	if (fight->engine == NULL)
	{
		delete fight;
		return NULL;
	}

	fight->engine->SetResize(true);
	fight->engine->SetBaseImageWidth(640);
	fight->engine->SetNeedCluster(false);
	fight->engine->SetNeedTrack(false);
	//fight->engine->SetThreshold(params->EventSens);


	return (HANDLE)fight;
}

FIGHT_API HRESULT fightProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAResult* result)
{
	if (result == NULL)
		return E_FAIL;

	TheFight* fight = (TheFight*)hModule;
	if (fight->size != sizeof(TheFight))
		return E_FAIL;
	awpImage* tmp = NULL;
	awpCreateGrayImage(&tmp, width, height, bpp, data);
	fight->engine->SetSourceImage(tmp, true);

	result->Num = fight->engine->GetState() > 0 ? 1 : 0;
	HRESULT res = S_OK;
	if (result->Num > 0)
	{
		TVAResult* r = fight->engine->GetFight();
		if (r != NULL)
		{
			result->Num = 1;
			result->blobs[0].XPos = r->blobs[0].XPos;
			result->blobs[0].YPos = r->blobs[0].YPos;
			result->blobs[0].Width = r->blobs[0].Width;
			result->blobs[0].Height = r->blobs[0].Height;
		}
		else
			res = E_FAIL;
	}
	
	awpReleaseImage(&tmp);
	return res;
}

FIGHT_API HRESULT  fightRelease(HANDLE* hModule)
{
	HANDLE module = *hModule;
	TheFight* fight = (TheFight*)module;
	if (fight->size != sizeof(TheFight))
		return E_FAIL;

	delete fight->engine;
	delete fight;
	fight = NULL;

	*hModule = NULL;
	return S_OK;
}