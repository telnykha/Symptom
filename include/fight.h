#ifndef _fight_h_
#define _fight_h_


#ifdef FIGHT_EXPORTS
#define FIGHT_API __declspec(dllexport)
#else
#define FIGHT_API __declspec(dllimport)
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


FIGHT_API HANDLE   fightCreate(TVAInitParams* params);
FIGHT_API HRESULT fightProcess(HANDLE hModule, int width, int height, int bpp,  unsigned char* data, TVAResult* result);
FIGHT_API HRESULT  fightRelease(HANDLE* hModule);


#endif // _fight_h_