#ifndef _track_h_
#define _track_h_
#ifdef WIN32
#ifdef TRACK_EXPORTS
	#define TRACK_API __declspec(dllexport)
#else
	#define TRACK_API __declspec(dllimport)
#endif
#else
	#define TRACK_API 
#endif 

extern "C"
{
	#ifdef WIN32
		#include <Rpc.h>
	#else
		#include <uuid/uuid.h>
	#endif
}

#include "va_common.h"


// экспортируемые функции
extern "C" TRACK_API HANDLE			trackCreate(TVAInitParams* params, int options, int width);
extern "C" TRACK_API HRESULT		trackProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAResult* result);
extern "C" TRACK_API HRESULT		trackForeground(HANDLE hModule, int width, int height, unsigned char* data);
extern "C" TRACK_API HRESULT		trackTrajectories(HANDLE hModule, TVATrajectories* trajectories);
extern "C" TRACK_API HRESULT		trackCheckLine(HANDLE hModule, TVAPoint& p1, TVAPoint& p2, TVATrajectory* t, bool* result, int* dir);
extern "C" TRACK_API HRESULT		trackCheckZone(HANDLE hModule, TVAZone* z, TVATrajectory* t, bool* result);
extern "C" TRACK_API HRESULT		trackRelease(HANDLE* hModule);

#endif // _track_h_
