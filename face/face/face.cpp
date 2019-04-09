// face.cpp : Defines the exported functions for the DLL application.
//
#include "face.h"
#include "_LF.h"
#include "LFFaceModule.h"

typedef struct
{
	int size;
	int max_objects;
	TLFFaceEngine* f;
}TheFace;
#ifdef WIN32
static bool _LoadEngine(const char* lpResourceName, TLFFaceEngine* engine)
{
	try
	{
		HMODULE hModule = GetModuleHandle("face.dll");
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
#endif

FACE_API HANDLE   faceCreate(TVAInitParams* params, double scale, double grow, int BaseSize, bool Tilt, int NumObjects)
{
	TheFace* face = new TheFace();
	face->size = sizeof(TheFace);
	face->f = new TLFFaceEngine(false, false, false);
	face->max_objects = NumObjects;
	if (face->f == NULL) 
	{
		delete face;
		return NULL;
	}

	if (BaseSize < 256)
		BaseSize = 256;
	if (BaseSize > 1980)
		BaseSize = 1980;
	face->f->SetBaseImageWidth(BaseSize);
	face->f->SetResize(true);

	try
	{
#ifdef WIN32
		if (!_LoadEngine("ENGINE_FACE", face->f))
			throw 0;
#else
		if (params->Path == 0 || !face->f->Load(params->Path))
			throw 0;
#endif
	}
	catch (...)
	{
		delete face->f;
		delete face;
		return NULL;
	}

	ILFScanner* scr = face->f->GetScanner();
	for (int i = 0; i < scr->GetParamsCount(); i++)
	{
		TLFParameter* p = scr->GetParameter(i);
		if (strcmp(p->GetPName(), "MinSize") == 0)
		{
			if (scale < 1)
				scale = 1;
			p->SetValue(scale);
		}
		if (strcmp(p->GetName(),"grow") == 0)
		{
			if (grow >= 1.1 && grow <= 2)
				p->SetValue(grow);
		}
	}
	// all ok
	return (HANDLE)face;
}
FACE_API HRESULT  faceProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAFace* result,  int* num)
{
	TheFace* p = (TheFace*)hModule;
	if (p->size != sizeof(TheFace))
		return E_FAIL;
#ifdef _DEBUG
	printf("bpp = %i \n", bpp);
#endif  
	awpImage* tmp = NULL;
	awpCreateGrayImage(&tmp, width, height, bpp, data);
	p->f->SetSourceImage(tmp, true);
	int n = p->f->GetItemsCount();
	*num =0;
	for (int i = 0; i < n; i++)
	{
		TLFDetectedItem*  di = p->f->GetItem(i);
		if (di != NULL)
		{
			awpRect rr = di->GetBounds()->GetRect();
			//UuidCreate(&result[i].id);					
			LF_UUID_CREATE(result[i].id);
			result[i].racurs = di->GetRacurs();
			result[i].XPos = rr.left;
			result[i].YPos = rr.top;
			result[i].Width = rr.right - rr.left;
			result[i].Height = rr.bottom - rr.top;
			(*num)++;
		}
	}

	awpReleaseImage(&tmp);

	return S_OK;
}
FACE_API HRESULT  faceRelease(HANDLE* hModule)
{
	TheFace* p = (TheFace*)hModule;
	if (p->size != sizeof(TheFace)) 
		return E_FAIL;

	delete p->f;
	delete p;
	p = NULL;

	return S_OK;
}

FACE_API HANDLE   facetrackCreate(TVAInitParams* params, int NumObjects)
{
	HANDLE hModule = faceCreate(params, 1, 320, 1.41, false, NumObjects);
	if (hModule == NULL)
		return NULL;
	TheFace* p = (TheFace*)hModule;
	p->f->SetNeedTrack(true);
	return hModule;

}
FACE_API HRESULT  facetrackProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAFace* result, int* num)
{
	TheFace* p = (TheFace*)hModule;
	if (p->size != sizeof(TheFace))
		return E_FAIL;
	awpImage* tmp = NULL;
	awpCreateGrayImage(&tmp, width, height, bpp, data);
	p->f->SetSourceImage(tmp, true);

	TLFSemanticImageDescriptor* d = p->f->GetSemantic();
	int n = d->GetCount() > p->max_objects ? p->max_objects : d->GetCount();
	*num = n;
	for (int i = 0; i < n; i++)
	{
		TLFDetectedItem*  di = d->GetDetectedItem(i);
		if (di != NULL)
		{
			awpRect rr = di->GetBounds()->GetRect();
			UUID id;
			di->GetId(id);
			memcpy(&result[i].id, &id, sizeof(UUID));
			result[i].racurs = di->GetRacurs();
			result[i].XPos = rr.left;
			result[i].YPos = rr.top;
			result[i].Width = rr.right - rr.left;
			result[i].Height = rr.bottom - rr.top;
		}
	}
	awpReleaseImage(&tmp);
	return S_OK;

}
FACE_API HRESULT  facetrackRelease(HANDLE* hModule)
{
	return faceRelease(hModule); 
}
