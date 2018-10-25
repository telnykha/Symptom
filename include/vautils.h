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
	���������� � ������ ��������� TVAParams
*/
extern "C" VAUTILS_API bool SaveInitParams(const char* lpFileName, TVAInitParams* params);
extern "C" VAUTILS_API bool LoadInitParams(const char* lpFileName, TVAInitParams** params);
extern "C" VAUTILS_API bool AddZone(TVAInitParams* params, TVAZone* zone);
extern "C" VAUTILS_API bool DeleteZone(TVAInitParams* params, int idx);
extern "C" VAUTILS_API void FreeParams(TVAInitParams** params);
extern "C" VAUTILS_API bool CopyParams(TVAInitParams* src, TVAInitParams** dst);


/*===============================================================================
	��������� ������ � ������������
	������
			  ��  - �����������
			  /\
			 /  \
			/    \
	----------------- ���������

	������� ��������� ������:
	f - �������� ����������.
	a - ���� �������.
	ws - ������ ���� �� ����������� � �����������
	hs - ������ ���� �� ��������� � �����������
	h  - ������ ���������
	--------------------------------
	�������� ���������:
	ah - ������������ ���� ������. 
	aw - �������������� ���� ������
	--------------------------------
	�������� ����������. 
	(x,y) ���������� ����� �� ���������.
	(X,Y) ���������� ����� �� �����������. 
	d0 - ���������� �� ������, �� ������ ������� �����
	d - ���������� �� ����� �� ���������. 
    h1 - ������ ������� ��� ����������. 
	H - ������ ������� �� �����������. 
	dh - ���������� �� �������, ������� h 
	w - ������ �������
	W - ������ ������� �� �����������. 

=================================================================================*/
// ���������� aw - �������������� ���� ������
VAUTILS_API double Camera_to_aw(TVACamera* pCamera);
// ���������� ah - ������������ ���� ������ 
VAUTILS_API double Camera_to_ah(TVACamera* pCamera);
// ���������� �� ������ ������� ����� �� �����. 
VAUTILS_API double Camera_to_d0(TVACamera* pCamera);
// ��������������, ��� ����� Y ����������� size ����� �� ���������� ���������, 
// ��� ������� ����������� ������ �� ������ h
VAUTILS_API double Y_to_d(TVACamera* pCamera, TVASize* size,  int Y);
// ����� ���������� ��������� �� �������� d �� ������ ������������ �� ����������� � ����� Y 
VAUTILS_API double d_to_Y(TVACamera* pCamera, TVASize* size, double d);
// �������� ����� x �� ���������� ��������� ������������ ���������� ��� ������. 
VAUTILS_API double XY_to_x(TVACamera* pCamera, TVASize* size, TVAPoint* pPoint);
// �������� �������������� �������� �������, ��������� �� ������ h �� ���������� ���������� ������������� ���������� ��� ������. 
VAUTILS_API double XY_to_xh(TVACamera* pCamera, TVASize* size, TVAPoint* pPoint, int  h);
//���������� �� ����� �� ���������� ��������, ������� ������ ������������ XY
VAUTILS_API double XY_to_d(TVACamera* pCamera, TVASize* size, TVAPoint* pPoint);
// ���������� �� �������, ������� ��������� �� ������ h1 ��� ��������� � ���������� �������� ������ 
// ������ Y �� �����������. 
VAUTILS_API double Yh1_to_d(TVACamera* pCamera, TVASize* size,  int Y, int h1);
// ������� �������� �� �����
VAUTILS_API double WH_to_w(TVACamera* pCamera, TVASize* size, TVARect* rect);
VAUTILS_API double WH_to_h(TVACamera* pCamera, TVASize* size, TVARect* rect);
//��������� ������ �������, ��������� ��� ���������� ���������� �� ������ h1 
VAUTILS_API double WHh1_to_w(TVACamera* pCamera, TVASize* size, TVARect* rect, int h1);
// �������������� 3d ����������
VAUTILS_API void Scene_To_ImagePoint(TVACamera* pCamera, TVASize* size, TVA3DPoint* pSrc, TVAPoint* pDst);
VAUTILS_API void Image_To_ScenePoint(TVACamera* pCamera, TVASize* size, double Height,  TVAPoint* pSrc, TVA3DPoint* pDst);
// ���� �������������� ����� ���� ���������� 
VAUTILS_API bool IsPointInZone(int x, int y, TVAInitParams* params);
VAUTILS_API bool ApplyZonesToImage(HANDLE image, TVAInitParams* params);
VAUTILS_API bool MakeMaskImage(HANDLE image, TVAInitParams* params);
#endif //_va_utils_ 
