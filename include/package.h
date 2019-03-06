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
	float minWidth;    // ����������� ������ ������������ ������� � �����������
	float maxWidth;	   // ������������ ������ ������������ ������� � �����������
	float minHeight;   // ����������� ������ ������������ ������� � �����������
	float maxHeight;   // ������������ ������ ������������ ������� � �����������
	int   numObects;   // ������������ ����� �������� ����������. 
	int   Mode;
}TVAPackageInit;

typedef struct
{
	UUID	id;			// ������������� �������.
	int		status;		// ������ ������� (1- ���������, 2 - �����������, 3 - �������)  
	int		time;		// ����� ���������� ������� � ������
	float   XPos;		// ����� ���� ������� �� �����������
	float   YPos;		// ����� ���� ������� �� �����������
	float	Width;		// ������ �������
	float   Height;		// ������ �������
}TVAPackageBlob;

typedef struct 
{
	int num;			// ����� ��������. ����� ���� ������ ��� TVAPackageInit.numObjects ��� �������������
	TVAPackageBlob* objects; // ������ ����������� ��������. 
}TVAPackageResult;


// �������������� �������
PACKAGE_API HANDLE		packageCreate(TVAInitParams* params, TVAPackageInit* params1);
PACKAGE_API HRESULT		packageProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAPackageResult* result);
PACKAGE_API HRESULT		packageForeground(HANDLE hModule, int width, int height, unsigned char* data);
PACKAGE_API HRESULT		packageRelease (HANDLE* hModule );

PACKAGE_API TVAInitParams* packageGetDefaultParams();


#endif // _package_h_
