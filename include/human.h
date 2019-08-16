#ifndef _human_h_
#define _human_h_

#ifdef HUMAN_EXPORTS
#define HUMAN_API __declspec(dllexport)
#else
#define HUMAN_API __declspec(dllimport)
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

HUMAN_API HANDLE   humanCreate(TVAInitParams* params, bool use_gpu);
HUMAN_API HRESULT  humanProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAResult* result);
HUMAN_API HRESULT  humanRelease(HANDLE* hModule);

#endif // _human_h_