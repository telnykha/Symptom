#ifndef _crowd_h_
#define _crowd_h_


#ifdef CROWD_EXPORTS
#define CROWD_API __declspec(dllexport)
#else
#define CROWD_API __declspec(dllimport)
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

typedef struct
{
	int		status;		// статус объекта (1- обнаружен, 2 - наблюдается, 3 - потерян)  
	float   XPos;		// центр масс объекта на изображении
	float   YPos;		// центр масс объекта на изображении
	float	Width;		// ширина объекта
	float   Height;		// высота объекта
}TVACrowdElement;

CROWD_API HANDLE   crowdCreate(TVAInitParams* params);
CROWD_API TVAResult* crowdProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data);
CROWD_API HRESULT  crowdRelease(HANDLE* hModule);

CROWD_API HRESULT  сrowdGetNumElements(HANDLE hModule, int& num);
CROWD_API HRESULT  crowdGetElements(HANDLE hModule, TVACrowdElement* elements);
CROWD_API HRESULT  сrowdGetNumObjects(HANDLE hModule, int& num);

#endif // _crowd_h_