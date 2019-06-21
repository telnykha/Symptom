#ifndef _face_h_
#define _face_h_

#ifdef WIN32
#ifdef FACE_EXPORTS
	#define FACE_API __declspec(dllexport)
#else
	#define FACE_API __declspec(dllimport)
#endif
#else
#define FACE_API 
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
	UUID	id;			// идентификатор объекта (лица).
	int		racurs;		// индикатор ракурса. 0 - front, 1 - left turn, 7 - right turn
	float   XPos;		// центр масс объекта на изображении
	float   YPos;		// центр масс объекта на изображении
	float	Width;		// ширина объекта
	float   Height;		// высота объекта
}TVAFace;

// поиск лиц
extern "C" 
{
FACE_API HANDLE   faceCreate(TVAInitParams* params, double scale, double grow, int BaseSize, bool Tilt,  int NumObjects);
FACE_API HRESULT  faceProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAFace* result,  int* num);
FACE_API HRESULT  faceRelease(HANDLE* hModule);
// поиск и сопровождение лиц
FACE_API HANDLE   facetrackCreate(TVAInitParams* params, int NumObjects);
FACE_API HRESULT  facetrackProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAFace* result, int* num);
FACE_API HRESULT  facetrackRelease(HANDLE* hModule);
}
#endif // _face_h_
