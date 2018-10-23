#ifndef _va_common_h_
#define _va_common_h_
extern "C"
{
#ifdef WIN32
	#include <Rpc.h>
#else
	#include <uuid/uuid.h>
	typedef uuid_t UUID;
#endif
}

#define VA_OK		 0
#define VA_ERROR	-1
/*
Структура, описывающая точку 
*/
typedef struct
{
	float X; // X координата в % от ширины изображения
	float Y; // Y координата в % от ширины изображения.
}TVAPoint;
/*
	структура, которая описывает точку в пространстве
*/
typedef struct 
{
	float X;
	float Y;
	float Z;
}TVA3DPoint;
/*
	структура, описывающая размеры изображения. 
*/
typedef struct
{
	int width;
	int height;
}TVASize;
/*
	Структура, описывающая прямоугольник	
*/
typedef struct 
{
	TVAPoint LeftTop;		//Координата левого верхнего угла прямоугольника. 
	TVAPoint RightBottom;	//Координата правого-нижнего угла прямоугольника
}TVARect;

/*
	Структура, описывающая зону наблюдения. 
*/
typedef struct
{
	bool		IsRect;			//Признак прямоугольной зоны наблюдения
	TVARect		Rect;			//Зона наблюдения в виде прямоугольника. 
	int			NumPoints;		//Число точек контура. 
	TVAPoint*	Points;			//Массив точек. 
}TVAZone;

/*
обнаруженный объект
*/
typedef struct
{
	UUID	id;			// идентификатор объекта.
	int		status;		// статус объекта (1- обнаружен, 2 - наблюдается, 3 - потерян)  
	int		time;		// время наблюдения объекта в кадрах
	float   XPos;		// центр масс объекта на изображении
	float   YPos;		// центр масс объекта на изображении
	float	Width;		// ширина объекта
	float   Height;		// высота объекта
}TVABlob;
/*
результат наблюдения
*/
typedef struct
{
	int Num;		// число активных объектов
	TVABlob* blobs; // местоположения и размеры объектов
}TVAResult;
/*
траектория
*/
typedef struct
{
	UUID	id;  //идентификатор объекта
	int		Num; //количество точек траектори
	TVABlob* blobs;// местоположения и размеры объектов
}TVATrajectory;
/*
массив траекторий
*/
typedef struct
{
	int Num; // число траекторий
	TVATrajectory* Trajectories; // траектории 
}TVATrajectories;
/*
	Структура, описывающая параметры видеокамеры. 
*/
typedef struct
{
	double Height;		//Высота установки в миллиметрах
	double Angle;		//Угол наклона камеры в градусах
	double Focus;		//Фокусное расстояние камеры в миллиметрах
	double WChipSize;	//Ширина сенсора в миллиметрах. 
	double HChipSize;	//Высота сенсора в миллиметрах. 
	double dMaxLenght;
}TVACamera;
/*
Структура инициализации модулей. 
*/
typedef struct
{
	bool	  SaveLog;		// для ревизора должен быть всегда установлен в false	
	char*	  Path;			// для ревизора должне быть установлен в ""
    bool      useTrack;     // использовать сопровождение найденных объектов
	TVACamera Camera;		// параметры камеры видеонаблюдения. 
	int		  NumZones;		// число областей наблюдения на кадре
	TVAZone*  Zones;		// области наблюдения. 
	double    EventSens;    // чувствительноность. 0...1; 0 - минимальная, 1 - максимальная, по умолчанию 0.5
	double	  EventTimeSens;// время наступления события в миллисекундах, по умолчанию 1000  
	double	  minWidth;		// минимальная ширина объекта, в % от ширины изображения. 
	double	  minHeight;	// минимальная высота объекта, d % от высоты изображения. 
	double	  maxWidth;		// максимальная шинина объекта в % от ширины изображени. 
	double	  maxHeight;	// максимальная высота объекта в % от высоты изображения. 
	int       numObects;	// максимальное число объектов наблюдения. 
}TVAInitParams;
#endif // _va_common_h_
