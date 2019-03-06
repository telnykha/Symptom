#ifndef _fire_h_
#define _fire_h_

#ifdef FIRE_EXPORTS
	#define FIRE_API __declspec(dllexport)
#else
	#define FIRE_API __declspec(dllimport)
#endif

#include "va_common.h"

typedef struct
{
	int		status;		// статус объекта (1- обнаружен, 2 - наблюдается, 3 - потерян)  
	float   XPos;		// центр масс объекта на изображении
	float   YPos;		// центр масс объекта на изображении
	float	Width;		// ширина объекта
	float   Height;		// высота объекта
}TVAFireElement;

FIRE_API HANDLE	     fireCreate(TVAInitParams* params);
FIRE_API HRESULT	 fireProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, bool* result);
FIRE_API HRESULT     fireRelease(HANDLE* hModule);
FIRE_API HRESULT     fireGetNumElements(HANDLE hModule, int& num);
FIRE_API HRESULT     fireGetElements(HANDLE hModule, TVAFireElement* elements);

#endif //_fire_h_