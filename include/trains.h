#ifndef __trains_h__
#define __trains_h__
#include "va_common.h"

#ifdef TRAINS_EXPORTS
#define TRAINS_API  __declspec(dllexport)  
#else
#define TRAINS_API __declspec(dllimport) 
#endif

#define TRAINSHANDLE void*
#define TARGETHANDLE void*
#define FULLNESSHANDLE void*

// trains number API
extern "C" TRAINS_API TRAINSHANDLE  trainsInit(TVARect& zone, double sens, int scale);
extern "C" TRAINS_API int  trainsProcess(TRAINSHANDLE trains, unsigned char* data, int width, int height, int bpp, char* result, TVARect& rect);
extern "C" TRAINS_API int  trainsClose(TRAINSHANDLE trains);

// trains target API
extern "C" TRAINS_API TARGETHANDLE  trainsTargetInit(TVAInitParams& params);
extern "C" TRAINS_API int  trainsTargetCreate(TARGETHANDLE target, unsigned char* data, int width, int height, int bpp);
extern "C" TRAINS_API int  trainsTargetHasModel(TARGETHANDLE target, bool* model);
extern "C" TRAINS_API int  trainsTargetClearModel(TARGETHANDLE target);
extern "C" TRAINS_API int  trainsTargetProcess(TARGETHANDLE target, unsigned char* data, int width, int height, int bpp, double* result, bool* visible);
extern "C" TRAINS_API int  trainsTargetClose(TARGETHANDLE target);

// trains fullness API
extern "C" TRAINS_API FULLNESSHANDLE trainsFullnessInit(TVAInitParams& params);
extern "C" TRAINS_API int  trainsFullnessProcess(FULLNESSHANDLE handle, unsigned char* data, int width, int height, int bpp, double* value);
extern "C" TRAINS_API int  trainsFullnessClose(FULLNESSHANDLE handle);

#endif	