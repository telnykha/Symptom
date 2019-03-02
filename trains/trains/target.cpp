#include "trains.h"
#include "LFTrainsModule.h"
#include <string>


const char* c_TargetName = "\\target.dmp";

using namespace std;


#define _CHECK_TARGET_IMG_(target)\
	TLFTrainsTarget* t = (TLFTrainsTarget*)target;\
	if (t == NULL)\
		return VA_ERROR;\
	awpImage* tmp = NULL;\
	if (awpCreateGrayImage(&tmp, width, height, bpp, data) != AWP_OK)\
	return VA_ERROR;\


#define _CHECK_TARGET_(target)\
	TLFTrainsTarget* t = (TLFTrainsTarget*)target;\
	if (t == NULL)\
		return VA_ERROR;\

// trains target API
TRAINS_API TARGETHANDLE  trainsTargetInit(TVAInitParams& params)
{
	
	if (params.NumZones != 1 || !params.Zones[0].IsRect)
		return NULL;

	TVARect zone = params.Zones[0].Rect;
	
	TLFZones zones;
	TLF2DRect rect;
	rect.SetVertexes(zone.LeftTop.X, zone.RightBottom.X, zone.LeftTop.Y, zone.RightBottom.Y);
	TLFZone*  z = new TLFZone(rect);
	zones.Add(z);

	TLFTrainsTarget* target = new TLFTrainsTarget(zones, &params);
	if (params.Path != NULL)
	{
		string fileName = params.Path;
		//fileName += c_TargetName;
		if (!target->LoadModel(fileName.c_str()))
		{
			printf("WARNING: cannot load model.\n");
		}
	}
	return (TARGETHANDLE)target;
}

TRAINS_API int  trainsTargetCreate(TARGETHANDLE target, unsigned char* data, int width, int height, int bpp)
{

	_CHECK_TARGET_IMG_(target)



	TVAInitParams* params = t->getModuleParams();
	assert(params != NULL);

	if (!t->InitModel(tmp))
	{
		_AWP_SAFE_RELEASE_(tmp)
		return VA_ERROR;
	}
	if (params->Path != NULL)
	{
		
		string fileName = params->Path;
		if (t->SaveModel(fileName.c_str()))
		{
			_AWP_SAFE_RELEASE_(tmp)
			return VA_OK;
		}
	}

	_AWP_SAFE_RELEASE_(tmp)
	return VA_ERROR;
}

TRAINS_API int  trainsTargetProcess(TARGETHANDLE target, unsigned char* data, int width, int height, int bpp, double* result, bool* visible)
{
	_CHECK_TARGET_IMG_(target)

	*result = t->CompareModel(tmp);
	*visible = t->IsTargetVisible();
	_AWP_SAFE_RELEASE_(tmp)

	return *result >= 0 ? VA_OK: VA_ERROR;
}

TRAINS_API int  trainsTargetHasModel(TARGETHANDLE target, bool* model)
{
	_CHECK_TARGET_(target)
	
	*model = t->HasModel();

	return VA_OK;
}

extern "C" TRAINS_API int  trainsTargetClearModel(TARGETHANDLE target)
{
	_CHECK_TARGET_(target)

	t->ClearModel();

	return VA_OK;
}



TRAINS_API int  trainsTargetClose(TARGETHANDLE target)
{
	_CHECK_TARGET_(target)
	
	delete t; 
	t = NULL;

	return VA_OK;
}


