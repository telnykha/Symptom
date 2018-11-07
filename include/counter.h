#ifndef _counter_h_
#define _counter_h_


#ifdef COUNTER_EXPORTS
#define COUNTER_API __declspec(dllexport)
#else
#define COUNTER_API __declspec(dllimport)
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

//---------------------------- old counter ----------------------------------------
COUNTER_API HANDLE   counterCreate(TVAInitParams* params);
COUNTER_API HRESULT  counterProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, double& value1, double& value2, int& state);
COUNTER_API HRESULT  counterGetCount(HANDLE hModule, int* in_count, int* out_count);
COUNTER_API HRESULT	 counterForeground(HANDLE hModule, int width, int height, unsigned char* data);
COUNTER_API HRESULT  counterRestart(HANDLE hModule);
COUNTER_API HRESULT  counterRelease(HANDLE* hModule);
COUNTER_API HRESULT  counterGetLastTrajectory(HANDLE hModule, TVAPoint* startPoint, TVAPoint* endPoint, bool* has_trajectory);
COUNTER_API HRESULT  counterGetEventImageSize(HANDLE hModule, int& width, int& height);
COUNTER_API HRESULT  counterGetEventImage(HANDLE hModule, int width, int height, double* data);
COUNTER_API TVAInitParams* counterGetDefaultParams();

//--------------------------- tragectory counter -------------------------------------
COUNTER_API HANDLE   tcounterCreate(TVAPoint start, TVAPoint finish, TVARect sizes, double eventSens);
COUNTER_API HRESULT  tcounterProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, double& value);
COUNTER_API HRESULT  tcounterRelease(HANDLE* hModule);

#endif // _counter_h_