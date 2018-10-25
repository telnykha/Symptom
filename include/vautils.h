#ifndef _va_utils_
#define _va_utils_

#ifdef WIN32
#ifdef VAUTILS_EXPORTS
#define VAUTILS_API __declspec(dllexport)
#else
#define VAUTILS_API __declspec(dllimport)
#endif 
#else
#define VAUTILS_API
typedef void* HANDLE;
#endif

#include "va_common.h" 
/*
	сохранение и чтение структуры TVAParams
*/
extern "C" VAUTILS_API bool SaveInitParams(const char* lpFileName, TVAInitParams* params);
extern "C" VAUTILS_API bool LoadInitParams(const char* lpFileName, TVAInitParams** params);
extern "C" VAUTILS_API bool AddZone(TVAInitParams* params, TVAZone* zone);
extern "C" VAUTILS_API bool DeleteZone(TVAInitParams* params, int idx);
extern "C" VAUTILS_API void FreeParams(TVAInitParams** params);
extern "C" VAUTILS_API bool CopyParams(TVAInitParams* src, TVAInitParams** dst);


/*===============================================================================
	интерфейс работы с видеокамерой
	Модель
			  пп  - видеокамера
			  /\
			 /  \
			/    \
	----------------- плоскость

	входные параметры камеры:
	f - фокусное расстояние.
	a - угол наклона.
	ws - размер чипа по горизонтали в миллиметрах
	hs - размер чипа по вертикали в миллиметрах
	h  - высота установки
	--------------------------------
	выходные параметры:
	ah - вертикальный угол зрения. 
	aw - горизонтальный угол зрения
	--------------------------------
	пересчет расстояний. 
	(x,y) координата точки на плоскости.
	(X,Y) координата точки на изображении. 
	d0 - расстояние от камеры, до первой видимой точки
	d - расстояние до точки на плоскости. 
    h1 - высота объекта над плоскостью. 
	H - высота объекта на изображении. 
	dh - расстояние до объекта, высотой h 
	w - ширина объекта
	W - ширина объекта на изображении. 

=================================================================================*/
// вычисление aw - горизонтальный угол зрения
VAUTILS_API double Camera_to_aw(TVACamera* pCamera);
// вычисление ah - вертикальный угол зрения 
VAUTILS_API double Camera_to_ah(TVACamera* pCamera);
// расстояние до первой видимой точки на сцене. 
VAUTILS_API double Camera_to_d0(TVACamera* pCamera);
// предполагается, что точка Y изображения size лежит на предметной плоскости, 
// над которой установлена камера на высоте h
VAUTILS_API double Y_to_d(TVACamera* pCamera, TVASize* size,  int Y);
// точка предметной плоскости на удалении d от камеры проецируется на изображение в точку Y 
VAUTILS_API double d_to_Y(TVACamera* pCamera, TVASize* size, double d);
// смещение точки x на предметной плоскости относительно оптической оси камеры. 
VAUTILS_API double XY_to_x(TVACamera* pCamera, TVASize* size, TVAPoint* pPoint);
// смещение горизонтальное смещение объекта, поднятого на высоту h на предметной плоскостью отностительно оптической оси камеры. 
VAUTILS_API double XY_to_xh(TVACamera* pCamera, TVASize* size, TVAPoint* pPoint, int  h);
//расстояние до точки на предметной плоскоти, которая задана координатами XY
VAUTILS_API double XY_to_d(TVACamera* pCamera, TVASize* size, TVAPoint* pPoint);
// расстояние до объекта, который находится на высоте h1 над плоскотью и координаты которого заданы 
// точкой Y на изображении. 
VAUTILS_API double Yh1_to_d(TVACamera* pCamera, TVASize* size,  int Y, int h1);
// размеры объектов на сцене
VAUTILS_API double WH_to_w(TVACamera* pCamera, TVASize* size, TVARect* rect);
VAUTILS_API double WH_to_h(TVACamera* pCamera, TVASize* size, TVARect* rect);
//вычисляет ширину объекта, поднятого над предметной плоскостью на высоту h1 
VAUTILS_API double WHh1_to_w(TVACamera* pCamera, TVASize* size, TVARect* rect, int h1);
// преобразование 3d координаты
VAUTILS_API void Scene_To_ImagePoint(TVACamera* pCamera, TVASize* size, TVA3DPoint* pSrc, TVAPoint* pDst);
VAUTILS_API void Image_To_ScenePoint(TVACamera* pCamera, TVASize* size, double Height,  TVAPoint* pSrc, TVA3DPoint* pDst);
// тетс принадлежности точки зоне наблюдения 
VAUTILS_API bool IsPointInZone(int x, int y, TVAInitParams* params);
VAUTILS_API bool ApplyZonesToImage(HANDLE image, TVAInitParams* params);
VAUTILS_API bool MakeMaskImage(HANDLE image, TVAInitParams* params);
#endif //_va_utils_ 
