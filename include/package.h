#ifndef _package_h_
#define _package_h_
#ifdef WIN32
#ifdef PACKAGE_EXPORTS
#define PACKAGE_API __declspec(dllexport)
#else
#define PACKAGE_API __declspec(dllimport)
#endif
#else
#define PACKAGE_API 
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

#define PACKAGE_FIX_BACKGROUND		0
#define PACKAGE_EVENT_BACKGROUND	1
#define PACKAGE_CONT_BACKGROUND     2 


typedef struct 
{
	float minWidth;    // минимальная ширина оставленного объекта в миллиметрах
	float maxWidth;	   // максимальная ширина оставленного объекта в миллиметрах
	float minHeight;   // минимальная высота оставленного объекта в миллиметрах
	float maxHeight;   // максимальная высота оставленного объекта в миллиметрах
	int   numObects;   // максимальное число объектов наблюдения. 
	int   Mode;
}TVAPackageInit;

typedef struct
{
	UUID	id;			// идентификатор объекта.
	int		status;		// статус объекта (1- обнаружен, 2 - наблюдается, 3 - потерян)  
	int		time;		// время наблюдения объекта в кадрах
	float   XPos;		// центр масс объекта на изображении
	float   YPos;		// центр масс объекта на изображении
	float	Width;		// ширина объекта
	float   Height;		// высота объекта
}TVAPackageBlob;

typedef struct 
{
	int num;			// число объектов. ДОЖНО БЫТЬ МЕНЬШЕ ЧЕМ TVAPackageInit.numObjects при инициализации
	TVAPackageBlob* objects; // массив наблюдаемых объектов. 
}TVAPackageResult;


// экспортируемые функции
PACKAGE_API HANDLE		packageCreate(TVAInitParams* params, TVAPackageInit* params1);
PACKAGE_API HRESULT		packageProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAPackageResult* result);
PACKAGE_API HRESULT		packageForeground(HANDLE hModule, int width, int height, unsigned char* data);
PACKAGE_API HRESULT		packageRelease (HANDLE* hModule );

PACKAGE_API TVAInitParams* packageGetDefaultParams();


#endif // _package_h_
