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
	false,//bool	  SaveLog;		// ��� �������� ������ ���� ������ ���������� � false	
	"\0",// char*	  Path;			// ��� �������� ������ ���� ���������� � ""
	{
		0,//double Height;		//������ ��������� � �����������
		0,//double Angle;		//���� ������� ������ � ��������
		0,//double Focus;		//�������� ���������� ������ � �����������
		0,//double WChipSize;	//������ ������� � �����������. 
		0,//double HChipSize;	//������ ������� � �����������. 
		0,//double dMaxLenght;
	},
	//TVACamera Camera;		// ��������� ������ ���������������. 
	0,//int		  NumZones;		// ����� �������� ���������� �� �����
	NULL,// TVAZone*  Zones;		// ������� ����������. 
	0.5,//double    EventSens;    // ������������������. 0...1; 0 - �����������, 1 - ������������, �� ��������� 0.5
	10,//double	  EventTimeSens;// ����� ����������� ������� � �������������, �� ��������� 1000  
	1,//double	  minSquare;	// ����������� ������� �������, � % �� ������� ����������� 
	15,//double    maxSquare;	// ������������ ������� �������, � % �� ������� ����������� 
	0,//int       numObects;	// ������������ ����� �������� ����������. 
};*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct 
{
	int size;
	TLFPackageEngine* e;
	TVAInitParams params;
	TVAPackageInit  params1;
}ThePackage;

// �������������� �������. 
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
