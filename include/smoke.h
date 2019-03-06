#ifndef _smoke_h_
#define _smoke_h_
#ifdef WIN32
#ifdef SMOKE_EXPORTS
#define SMOKE_API __declspec(dllexport)
#else
#define SMOKE_API __declspec(dllimport)
#endif
#else
#define SMOKE_API
#endif

#include "va_common.h"

typedef struct
{
	int		status;		// ������ ������� (1- ���������, 2 - �����������, 3 - �������)  
	float   XPos;		// ����� ���� ������� �� �����������
	float   YPos;		// ����� ���� ������� �� �����������
	float	Width;		// ������ �������
	float   Height;		// ������ �������
}TVASmokeElement;

SMOKE_API HANDLE	 smokeCreate(TVAInitParams* params);
SMOKE_API HRESULT	 smokeProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, bool* result);
SMOKE_API HRESULT    smokeRelease (HANDLE* hModule );
SMOKE_API HRESULT    smokeGetNumElements(HANDLE hModule, int& num);
SMOKE_API HRESULT    smokeGetElements(HANDLE hModule, TVASmokeElement* elements);

#endif //_smoke_h_
