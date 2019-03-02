#include "trains.h"
#include "LFTrainsModule.h"


#define _CHECK_FULLNESS_HADLE_(handle)\
	TLFTrainsFullness* t = (TLFTrainsFullness*)handle;\
	if (t == NULL)\
		return VA_ERROR;\

TRAINS_API FULLNESSHANDLE trainsFullnessInit(TVAInitParams& params)
{
	if (params.NumZones != 1 || !params.Zones[0].IsRect)
		return NULL;

	TVARect zone = params.Zones[0].Rect;

	TLFZones zones;
	TLF2DRect rect;
	rect.SetVertexes(zone.LeftTop.X, zone.RightBottom.X, zone.LeftTop.Y, zone.RightBottom.Y);
	TLFZone*  z = new TLFZone(rect);
	zones.Add(z);

	TLFTrainsFullness* handle = new TLFTrainsFullness(zones, &params);

	return (FULLNESSHANDLE)handle;
}

TRAINS_API int  trainsFullnessProcess(FULLNESSHANDLE handle, unsigned char* data, int width, int height, int bpp, double* value)
{
	_CHECK_FULLNESS_HADLE_(handle)
	
	awpImage* tmp = NULL; 
	if (awpCreateGrayImage(&tmp, width, height, bpp, data) != AWP_OK)
		return VA_ERROR; 

	*value = t->ProcessImage(tmp);

	_AWP_SAFE_RELEASE_(tmp)
	return VA_OK;
}
TRAINS_API int  trainsFullnessClose(FULLNESSHANDLE handle)
{
	_CHECK_FULLNESS_HADLE_(handle)

	delete t;
	t = NULL;

	return VA_OK;
}

