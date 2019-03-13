#ifndef _va_common_h_
#define _va_common_h_
extern "C"
{
#ifdef WIN32
	#include <Rpc.h>
#else
	#include <uuid/uuid.h>
	#include <stdbool.h>	
	typedef uuid_t UUID;
	#define HRESULT int
	#define E_FAIL -1
	#define S_OK	0
	#define DWORD  unsigned long
#endif
}

typedef void* HANDLE;

#define VA_OK		 0
#define VA_ERROR	-1
/*

*/
typedef struct
{
	float X; // 
	float Y; // 
}TVAPoint;
/*
*/
typedef struct 
{
	float X;
	float Y;
	float Z;
}TVA3DPoint;
/*
*/
typedef struct
{
	int width;
	int height;
}TVASize;
/*
*/
typedef struct 
{
	TVAPoint LeftTop;		//
	TVAPoint RightBottom;	//
}TVARect;

/*
*/
typedef struct
{
	bool		IsRect;			//
	TVARect		Rect;			//
	int			NumPoints;		//
	TVAPoint*	Points;			//
}TVAZone;

/*
*/
typedef struct
{
	UUID	id;			// 
	int		status;		// 
	int		time;		// 
	float   XPos;		// 
	float   YPos;		// 
	float	Width;		// 
	float   Height;		// 
}TVABlob;
/*
*/
typedef struct
{
	int Num;		// 
	TVABlob* blobs; // 
}TVAResult;
/*
*/
typedef struct
{
	UUID	id;  //
	int		Num; //
	TVABlob* blobs;// 
}TVATrajectory;
/*
*/
typedef struct
{
	int Num; // 
	TVATrajectory* Trajectories; // 
}TVATrajectories;
/*
*/
typedef struct
{
	double Height;		//
	double Angle;		//
	double Focus;		//
	double WChipSize;	//
	double HChipSize;	//
	double dMaxLenght;
}TVACamera;
/*
*/
typedef struct
{
	bool	  SaveLog;		// 
	char*	  Path;			// 
    bool      useTrack;     // 
	TVACamera Camera;		// 
	int		  NumZones;		// 
	TVAZone*  Zones;		// 
	double    EventSens;    // 
	double	  EventTimeSens;// 
	double	  minWidth;		// 
	double	  minHeight;	// 
	double	  maxWidth;		// 
	double	  maxHeight;	// 
	int       numObects;	// 
}TVAInitParams;
#endif // _va_common_h_
