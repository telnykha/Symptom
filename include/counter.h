#ifndef _counter_h_
#define _counter_h_

#ifdef WIN32
	#ifdef COUNTER_EXPORTS
		#define COUNTER_API __declspec(dllexport)
	#else
		#define COUNTER_API __declspec(dllimport)
	#endif
#else
		#define COUNTER_API
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



#define TRAJECTORY_COUNTER		0
#define SINGLE_DOOR_COUNTER     1


//--------------------------- counter API ------------------------------------------------------------------------------
COUNTER_API HANDLE   tcounterCreate(TVAPoint start, TVAPoint finish, TVARect sizes, double eventSens);
COUNTER_API HANDLE   tcounterCreateEx(TVAPoint start, TVAPoint finish, TVARect sizes, double eventSens,  int options);
COUNTER_API HRESULT  tcounterProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, double& value);
COUNTER_API HRESULT  tcounterRelease(HANDLE* hModule);
//--------------------------- counter advanced API -----------------------------------------------------------------------
COUNTER_API HRESULT  tcounterGetState(HANDLE hModule, double& value1, double& value2, int& state);
COUNTER_API HRESULT  tcounterGetCount(HANDLE hModule, int* in_count, int* out_count);
COUNTER_API HRESULT  tcounterGetForeground(HANDLE hModule, int width, int height, unsigned char* data);
COUNTER_API HRESULT  tcounterGetLastTrajectory(HANDLE hModule, TVAPoint* startPoint, TVAPoint* endPoint, bool* has_trajectory);


#endif // _counter_h_
