#ifndef _awp_flow_tracker_
#define _awp_flow_tracker_

#include "awpipl.h"

extern "C"
{
#ifdef WIN32
#include <Rpc.h>
#else
#include <uuid/uuid.h>
#endif
}

#ifdef AWPFLOWTRACKER_EXPORTS
#define AWPFLOW_API  __declspec(dllexport)  
#else
#define AWPFLOW_API  //__declspec(dllimport) 
#endif

DECLARE_HANDLE(HAWPFLOW);
DECLARE_HANDLE(HAWPMFLOW);

typedef struct
{
	awpRect rect;
	UUID   id;
	AWPINT   status;
}awpID;

// flow object tracker 
extern "C" AWPFLOW_API HAWPFLOW awpflowCreate();
extern "C" AWPFLOW_API HRESULT  awpflowTrack(HAWPFLOW hflow, awpImage* image1, awpImage* image2,  awpRect* prevRect, awpRect* currRect);
extern "C" AWPFLOW_API HRESULT  awpflowRelease(HAWPFLOW hflow);
// flow object multi tracker 
extern "C" AWPFLOW_API HAWPMFLOW awpmflowCreate();
extern "C" AWPFLOW_API HRESULT   awpmflowTrack(HAWPMFLOW hflow, awpImage* image1, awpImage* image2,  awpID* prevID, awpID* currID, int num);
extern "C" AWPFLOW_API HRESULT   awpmflowRelease(HAWPMFLOW hflow);
#endif 