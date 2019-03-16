#ifndef _crowd_h_
#define _crowd_h_

#ifdef WIN32
#ifdef CROWD_EXPORTS
#define CROWD_API __declspec(dllexport)
#else
#define CROWD_API __declspec(dllimport)
#endif
#else
#define CROWD_API 
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
	int		status;		// ������ ������� (1- ���������, 2 - �����������, 3 - �������)  
	float   XPos;		// ����� ���� ������� �� �����������
	float   YPos;		// ����� ���� ������� �� �����������
	float	Width;		// ������ �������
	float   Height;		// ������ �������
}TVACrowdElement;

extern "C"
{
CROWD_API HANDLE   crowdCreate(TVAInitParams* params);
CROWD_API TVAResult* crowdProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data);
CROWD_API HRESULT  crowdRelease(HANDLE* hModule);

CROWD_API HRESULT  crowdGetNumElements(HANDLE hModule, int& num);
CROWD_API HRESULT  crowdGetElements(HANDLE hModule, TVACrowdElement* elements);
CROWD_API HRESULT  crowdGetNumObjects(HANDLE hModule, int& num);
}
#endif // _crowd_h_
