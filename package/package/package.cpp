// package.cpp : Defines the exported functions for the DLL application.
//

#include "va_common.h"
#include "package.h"
#include "vautils.h"
#include "awpipl.h"
#include "sabotage.h"

extern "C"
{
#ifdef WIN32
#include <Rpc.h>
#else
#include <uuid/uuid.h>
#endif
}

#include "_LF.h"
/*
TVAInitParams g_params = {
	false,//bool	  SaveLog;		// для ревизора должен быть всегда установлен в false	
	"\0",// char*	  Path;			// для ревизора должне быть установлен в ""
	{
		0,//double Height;		//Высота установки в миллиметрах
		0,//double Angle;		//Угол наклона камеры в градусах
		0,//double Focus;		//Фокусное расстояние камеры в миллиметрах
		0,//double WChipSize;	//Ширина сенсора в миллиметрах. 
		0,//double HChipSize;	//Высота сенсора в миллиметрах. 
		0,//double dMaxLenght;
	},
	//TVACamera Camera;		// параметры камеры видеонаблюдения. 
	0,//int		  NumZones;		// число областей наблюдения на кадре
	NULL,// TVAZone*  Zones;		// области наблюдения. 
	0.5,//double    EventSens;    // чувствительноность. 0...1; 0 - минимальная, 1 - максимальная, по умолчанию 0.5
	10,//double	  EventTimeSens;// время наступления события в миллисекундах, по умолчанию 1000  
	1,//double	  minSquare;	// минимальная площадь объекта, в % от площади изображения 
	15,//double    maxSquare;	// максимальная площадь объекта, в % от площади изображения 
	0,//int       numObects;	// максимальное число объектов наблюдения. 
};*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct 
{
	int size;
	TLFPackageEngine* e;
	TVAInitParams params;
	TVAPackageInit  params1;
}ThePackage;

// Экспортируемые функции. 
PACKAGE_API HANDLE		packageCreate(TVAInitParams* params, TVAPackageInit* params1)
{
	ThePackage* p = new ThePackage();
	p->size = sizeof(ThePackage);
	memcpy(&p->params, params, sizeof(TVAInitParams));
	memcpy(&p->params1, params1, sizeof(TVAPackageInit));

	p->e = new TLFPackageEngine();
	if (p->e == NULL)
	{
		delete p;
		return NULL;
	}

	TLFZones* z = new TLFZones();
	z->Load("notebook.zone");
	p->e->SetZones(z);
	delete z;

	p->e->SetNeedCluster(true);
	p->e->SetNeedTrack(true);
	p->e->SetResize(true);
	p->e->SetBaseImageWidth(320);
	p->e->SetAverageBufferSize(100);
	p->e->SetBGStability(100);

	// 
	int delay = int(p->params.EventTimeSens);

	p->e->SetDelay(delay);
	
	ILFObjectDetector* d = NULL;
	d = p->e->GetDetector(0);
	if (d != NULL)
	{
		d->SetBaseHeight(4);
		d->SetBaseWidht(4);
		d->SetThreshold(p->params.EventSens);
	}

	p->e->SetMaxHeight(params1->maxHeight);
	p->e->SetMaxWidth(params1->maxWidth);
	p->e->SetMinHeight(params1->minHeight);
	p->e->SetMinWidth(params1->minWidth);




	return (HANDLE)p;
}
PACKAGE_API HRESULT		packageProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAPackageResult* result)
{
	ThePackage* p = (ThePackage*)hModule;
	if (p->size != sizeof(ThePackage))
		return E_FAIL;
	HRESULT res = S_OK;
	awpImage* tmp = NULL;
	awpCreateGrayImage(&tmp, width, height, bpp, data);
	p->e->SetSourceImage(tmp, true);
	int n = p->e->GetItemsCount();
	result->num = n < p->params1.numObects ? n : p->params1.numObects;

	for (int i = 0; i < result->num; i++)
	{
		TLFDetectedItem*  di = p->e->GetItem(i);
		if (di != NULL)
		{
			awpRect rr = di->GetBounds()->GetRect();
			UUID id;
			di->GetId(id);
			memcpy(&result->objects[i].id, &id, sizeof(UUID));
			result->objects[i].XPos = rr.left;
			result->objects[i].YPos = rr.top;
			result->objects[i].Width = float(rr.right - rr.left);
			result->objects[i].Height = float(rr.bottom - rr.top);
			result->objects[i].status = di->GetState();
			//printf("state = %d\n", result->objects[i].status);
		}
	}

	awpReleaseImage(&tmp);

	return S_OK;
}
PACKAGE_API HRESULT		packageForeground(HANDLE hModule, int width, int height, unsigned char* data)
{
		ThePackage* p = (ThePackage*)hModule;
		if (p->size != sizeof(ThePackage))
			return E_FAIL;
	
		awpImage* img = p->e->GetForeground();//->GetFGBG()->GetForeground();
		if (img != NULL)
		{
			awpImage* tmp = NULL;
			awpCopyImage(img, &tmp);

			if (tmp->sSizeX != width || tmp->sSizeY != height)
				awpResize(tmp, width, height);
			memcpy(data, tmp->pPixels, width*height*sizeof(unsigned char));
			_AWP_SAFE_RELEASE_(tmp);
		}

		return S_OK;
}
PACKAGE_API HRESULT		packageRelease (HANDLE* hModule )
{
	ThePackage* p = (ThePackage*)(*hModule);
	if (p->size != sizeof(ThePackage))
		return E_FAIL;

	delete p->e;
	delete p;
	p = NULL;

	return S_OK;
}

PACKAGE_API TVAInitParams* packageGetDefaultParams()
{
	//return &g_params;
	return NULL;
}
