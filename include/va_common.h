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
���������, ����������� ����� 
*/
typedef struct
{
	float X; // X ���������� � % �� ������ �����������
	float Y; // Y ���������� � % �� ������ �����������.
}TVAPoint;
/*
	���������, ������� ��������� ����� � ������������
*/
typedef struct 
{
	float X;
	float Y;
	float Z;
}TVA3DPoint;
/*
	���������, ����������� ������� �����������. 
*/
typedef struct
{
	int width;
	int height;
}TVASize;
/*
	���������, ����������� �������������	
*/
typedef struct 
{
	TVAPoint LeftTop;		//���������� ������ �������� ���� ��������������. 
	TVAPoint RightBottom;	//���������� �������-������� ���� ��������������
}TVARect;

/*
	���������, ����������� ���� ����������. 
*/
typedef struct
{
	bool		IsRect;			//������� ������������� ���� ����������
	TVARect		Rect;			//���� ���������� � ���� ��������������. 
	int			NumPoints;		//����� ����� �������. 
	TVAPoint*	Points;			//������ �����. 
}TVAZone;

/*
������������ ������
*/
typedef struct
{
	UUID	id;			// ������������� �������.
	int		status;		// ������ ������� (1- ���������, 2 - �����������, 3 - �������)  
	int		time;		// ����� ���������� ������� � ������
	float   XPos;		// ����� ���� ������� �� �����������
	float   YPos;		// ����� ���� ������� �� �����������
	float	Width;		// ������ �������
	float   Height;		// ������ �������
}TVABlob;
/*
��������� ����������
*/
typedef struct
{
	int Num;		// ����� �������� ��������
	TVABlob* blobs; // �������������� � ������� ��������
}TVAResult;
/*
����������
*/
typedef struct
{
	UUID	id;  //������������� �������
	int		Num; //���������� ����� ���������
	TVABlob* blobs;// �������������� � ������� ��������
}TVATrajectory;
/*
������ ����������
*/
typedef struct
{
	int Num; // ����� ����������
	TVATrajectory* Trajectories; // ���������� 
}TVATrajectories;
/*
	���������, ����������� ��������� �����������. 
*/
typedef struct
{
	double Height;		//������ ��������� � �����������
	double Angle;		//���� ������� ������ � ��������
	double Focus;		//�������� ���������� ������ � �����������
	double WChipSize;	//������ ������� � �����������. 
	double HChipSize;	//������ ������� � �����������. 
	double dMaxLenght;
}TVACamera;
/*
��������� ������������� �������. 
*/
typedef struct
{
	bool	  SaveLog;		// ��� �������� ������ ���� ������ ���������� � false	
	char*	  Path;			// ��� �������� ������ ���� ���������� � ""
    bool      useTrack;     // ������������ ������������� ��������� ��������
	TVACamera Camera;		// ��������� ������ ���������������. 
	int		  NumZones;		// ����� �������� ���������� �� �����
	TVAZone*  Zones;		// ������� ����������. 
	double    EventSens;    // ������������������. 0...1; 0 - �����������, 1 - ������������, �� ��������� 0.5
	double	  EventTimeSens;// ����� ����������� ������� � �������������, �� ��������� 1000  
	double	  minWidth;		// ����������� ������ �������, � % �� ������ �����������. 
	double	  minHeight;	// ����������� ������ �������, d % �� ������ �����������. 
	double	  maxWidth;		// ������������ ������ ������� � % �� ������ ����������. 
	double	  maxHeight;	// ������������ ������ ������� � % �� ������ �����������. 
	int       numObects;	// ������������ ����� �������� ����������. 
}TVAInitParams;
#endif // _va_common_h_
