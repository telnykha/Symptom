#ifndef _sabotage_h_
#define _sabotage_h_

#ifdef SABOTAGE_EXPORTS
#define SABOTAGE_API __declspec(dllexport)
#else
#define SABOTAGE_API __declspec(dllimport)
#endif

#include "va_common.h"

#define NO_SABOTAGE					0
#define CAMERA_SABOTAGE				1


SABOTAGE_API HANDLE		sabotageCreate(TVAInitParams* params);
SABOTAGE_API HRESULT	sabotageProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, bool* result);
SABOTAGE_API HRESULT	sabotageState(HANDLE hModule, int* state);
SABOTAGE_API HRESULT    sabotageRelease (HANDLE* hModule );

#endif //_sabotage_h_