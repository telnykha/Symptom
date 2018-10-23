#ifndef _track_h_
#define _track_h_

#ifdef TRACK_EXPORTS
	#define TRACK_API __declspec(dllexport)
#else
	#define TRACK_API __declspec(dllimport)
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
TRACK_API HANDLE		trackCreate(TVAInitParams* params);
TRACK_API HRESULT		trackProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAResult* result);
TRACK_API HRESULT		trackForeground(HANDLE hModule, int width, int height, unsigned char* data);
TRACK_API HRESULT		trackTrajectories(HANDLE hModule, TVATrajectories* trajectories);
TRACK_API HRESULT		trackCheckLine(HANDLE hModule, TVAPoint& p1, TVAPoint& p2,  TVATrajectory* t, bool* result, int* dir);
TRACK_API HRESULT		trackCheckZone(HANDLE hModule, TVAZone* z, TVATrajectory* t, bool* result);
TRACK_API HRESULT		trackRelease(HANDLE* hModule);

#endif // _track_h_