#include "trains.h"

#ifdef WIN32
	#include <windows.h>
#endif 

#include "LFTrainsModule.h"

#ifndef NULL
	#define NULL 0 
#endif

#ifdef _DEBUG
	const int c_naa = 0;
#endif 

static bool _LoadEngineFromResource(const char* lpResourceName, TLFDetectEngine* engine)
{
	try
	{
		HMODULE hModule = GetModuleHandle("trains.dll");
		if (hModule == NULL)
			throw 0;

		HRSRC   hRC = NULL;
		hRC = FindResource(hModule, "#108", lpResourceName);
		if (hRC == NULL)
			throw 0;
		int     DumpSize = 0;
		DumpSize = SizeofResource(hModule, hRC);
		if (DumpSize == 0)
			throw 0;

		HGLOBAL hgTempl = NULL;
		hgTempl = LoadResource(hModule, hRC);
		if (hgTempl == NULL)
			throw 0;

		char*   pDump = NULL;
		pDump = (char*)LockResource(hgTempl);
		if (pDump == NULL)
			throw 0;

		stringstream s;
		string str;
		str = pDump;
		s << str;

		if (!engine->LoadStream(s))
			throw 0;
	}
	catch (...)
	{
		return false;
	}
	return true;
}

#define _SETUP_DIGIT_ENGINE_ \
		engine->SetNeedCluster(false);\
		engine->SetResize(false);\
		engine->GetScanner()->GetParameter(2)->SetValue(1.3);\


TRAINS_API TRAINSHANDLE  trainsInit(TVARect& zone, double sens, int scale)
{	
	TLFZones zones;
	TLF2DRect rect;
	rect.SetVertexes(zone.LeftTop.X, zone.RightBottom.X, zone.LeftTop.Y, zone.RightBottom.Y);
	TLFZone*  z = new TLFZone(rect);
	zones.Add(z);
	TLFTrains* trains = new TLFTrains(zones);
	if (trains == NULL)
		return NULL;
	// Load detector
	TLFDetectEngine* engine = trains->GetDetector();
	TLFObjectList*   list = trains->GetDigitsDetectors();
	
	engine->SetZones(&zones);
	engine->SetNeedTrack(false);
	engine->SetResize(true);
	engine->SetBaseImageWidth(640);

	try
	{
		if (!_LoadEngineFromResource("ENGINE_NUM", engine))
			throw 0;
		engine = (TLFDetectEngine*)list->Get(0);
		if (!_LoadEngineFromResource("ENGINE_0", engine))
			throw 0;
		_SETUP_DIGIT_ENGINE_

		engine = (TLFDetectEngine*)list->Get(1);
		if (!_LoadEngineFromResource("ENGINE_1", engine))
			throw 0;
		_SETUP_DIGIT_ENGINE_

		engine = (TLFDetectEngine*)list->Get(2);
		if (!_LoadEngineFromResource("ENGINE_2", engine))
			throw 0;
		_SETUP_DIGIT_ENGINE_

		engine = (TLFDetectEngine*)list->Get(3);
		if (!_LoadEngineFromResource("ENGINE_3", engine))
			throw 0;
		_SETUP_DIGIT_ENGINE_

		engine = (TLFDetectEngine*)list->Get(4);
		if (!_LoadEngineFromResource("ENGINE_4", engine))
			throw 0;
		_SETUP_DIGIT_ENGINE_

		engine = (TLFDetectEngine*)list->Get(5);
		if (!_LoadEngineFromResource("ENGINE_5", engine))
			throw 0;
		_SETUP_DIGIT_ENGINE_

		engine = (TLFDetectEngine*)list->Get(6);
		if (!_LoadEngineFromResource("ENGINE_6", engine))
			throw 0;
		_SETUP_DIGIT_ENGINE_

		engine = (TLFDetectEngine*)list->Get(7);
		if (!_LoadEngineFromResource("ENGINE_7", engine))
			throw 0;
		_SETUP_DIGIT_ENGINE_

		engine = (TLFDetectEngine*)list->Get(8);
		if (!_LoadEngineFromResource("ENGINE_8", engine))
			throw 0;
		_SETUP_DIGIT_ENGINE_

		engine = (TLFDetectEngine*)list->Get(9);
		if (!_LoadEngineFromResource("ENGINE_9", engine))
			throw 0;
		_SETUP_DIGIT_ENGINE_
	}
	catch (...)
	{
		delete trains;
		return NULL;
	}
#ifdef _DEBUG
	engine = trains->GetDetector();
	printf("engine.Zones: %s\n", engine->GetZones() != NULL? "yes":"no");
	if (engine->GetZones() != NULL)
	{
		printf("engine.NumZones: %i\n", engine->GetZones()->GetCount());
	}
#endif 
	return (TRAINSHANDLE)trains;
}


TRAINS_API int trainsProcess(TRAINSHANDLE trains, unsigned char* data, int width, int height, int bpp, char* result, TVARect& rect)
{
	TLFTrains* t = (TLFTrains*)trains;
	if (t == NULL)
		return 1;
	awpImage* tmp = NULL;
	if (awpCreateGrayImage(&tmp, width, height, bpp, data) != AWP_OK)
		return 1;
	awpRect a_rect;
	int res = 1;
	memset(result, 35, 8 * sizeof(char));
	result[9] = '\0';

	if (t->ProcessImage(tmp, &a_rect, result) > 0)
	{		// 
		rect.LeftTop.X = 100.f*a_rect.left/(float)width;
		rect.LeftTop.Y = 100.f*a_rect.top / (float)height;
		rect.RightBottom.X = 100.f*a_rect.right / (float)width;
		rect.RightBottom.Y = 100.f*a_rect.bottom / (float)height;
		result[9] = '\0';
		res = 0;
	}
	_AWP_SAFE_RELEASE_(tmp)
	return res;
}

TRAINS_API int trainsClose(TRAINSHANDLE trains)
{
	TLFTrains* t = (TLFTrains*)trains;
	if (t != NULL)
		delete t;
	return 0;
}
