// crowd.cpp : Defines the exported functions for the DLL application.
//
#include "crowd.h"
#include "_LF.h"
#include "LFCrowdModule.h"
typedef struct
{
	int size;
	CCrowdEngine* s;
	TVAResult*    r; 
}TheCrowd;

CROWD_API HANDLE   crowdCreate(TVAInitParams* params)
{
	if (params == NULL)
		return NULL;
	if (params->NumZones != 2)
		return NULL;
	//check zones 
	if (!params->Zones[0].IsRect)
		return NULL;

	TheCrowd* ts = new TheCrowd();
	if (ts == NULL)
		return NULL;
	ts->size = sizeof(TheCrowd);
	ts->s = new CCrowdEngine(*params);
	if (ts->s == NULL)
	{
		delete ts;
		return NULL;
	}

	ts->s->SetResize(true);
	ts->s->SetBaseImageWidth(320);
	ts->s->SetNeedCluster(false);
	ts->s->SetNeedTrack(false);
	ts->s->SetThreshold(params->EventSens);

	ILFObjectDetector* d = ts->s->GetDetector(0);
	if (d != NULL)
	{
		d->SetBaseHeight(8);
		d->SetBaseWidht(8);
	}
	ts->r = new TVAResult();
	ts->r->Num = 0;
	ts->r->blobs = NULL;
	return (HANDLE)ts;
}

static void _processForeground(HANDLE hModule, awpImage* image)
{
	TheCrowd* ts = (TheCrowd*)hModule;
	if (ts->size != sizeof(TheCrowd))
		return;

	TVAResult* result = ts->r;
	if (result->blobs != NULL)
	{
		free(result->blobs);
		result->blobs = NULL;
		result->Num = 0;
	}

	if (image == NULL)
		return;
	//printf("S0 = %f\n", ts->s->GetS0());
	int num = 0;
	awpStrokeObj* str = NULL;
	if (awpGetStrokes(image, &num, &str, 128, NULL) != AWP_OK)
		return;
	if (num > 0)
	{
		
		double a = ts->s->GetResizeCoef();
		awpRect rect;
		for (int i = 0; i < num; i++)
		{
			int s1 = 0;
			awpCalcObjRect(&str[i], &rect);
			awpStrObjSquare(&str[i], &s1);
			awpPoint p;
			p.X = (rect.right + rect.left) / 2;
			p.Y = (rect.top + rect.bottom) / 2;
			double s = (double)(rect.right - rect.left)*(rect.bottom - rect.top);
			double s0 = ts->s->GetS0();
			if (s0 < 0)
				return;
			int    n = floor(s1 / s0 + 0.5);
			int    n1 = floor(s1 / ts->s->SourceSquare() + 0.5);
			if (n1 == 1 && ts->s->GetParams()->EventTimeSens >= 0)
			{
				ts->s->AddNewSquare(s);
			}

			result->Num += n;
			if (n > 0)
			{
				// add data to VAResult
				result->blobs = (TVABlob*)realloc(result->blobs, result->Num*sizeof(TVABlob));
				for (int j = 0; j < n; j++)
				{
					LF_NULL_UUID_CREATE(result->blobs[result->Num - 1 - j].id);
					result->blobs[result->Num - 1 - j].time = 0;
					result->blobs[result->Num - 1 - j].status = 1;
					result->blobs[result->Num - 1 - j].XPos = a*p.X;
					result->blobs[result->Num - 1 - j].YPos= a*p.Y;
					result->blobs[result->Num - 1 - j].Width= a*(rect.right - rect.left);
					result->blobs[result->Num - 1 - j].Height = a*(rect.bottom - rect.top);
				}
			}

		}
		awpFreeStrokes(num, &str);
	}
}

CROWD_API TVAResult* crowdProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data)
{
	TheCrowd* ts = (TheCrowd*)hModule;
	if (ts->size != sizeof(TheCrowd))
		return NULL;
	awpImage* tmp = NULL;
	awpCreateGrayImage(&tmp, width, height, bpp, data);	
	ts->s->SetSourceImage(tmp, true);

	_processForeground(hModule, ts->s->GetForeground());
	
	awpReleaseImage(&tmp);
	return ts->r;
}
CROWD_API HRESULT  crowdRelease(HANDLE* hModule)
{
	HANDLE module = *hModule;
	TheCrowd* ts = (TheCrowd*)module;
	if (ts->size != sizeof(TheCrowd))
		return E_FAIL;

	delete ts->s;
	if (ts->r->blobs != NULL)
	{
		free(ts->r->blobs);
		ts->r->blobs = NULL;
		ts->r->Num = 0;
	}
	delete ts->r;
	delete ts;
	ts = NULL;

	*hModule = NULL;
	return S_OK;
}

CROWD_API HRESULT     ñrowdGetNumElements(HANDLE hModule, int& num)
{
	TheCrowd* ts = (TheCrowd*)hModule;
	if (ts->size != sizeof(TheCrowd))
		return E_FAIL;
	num = ts->s->GetItemsCount();
	return S_OK;
}

CROWD_API HRESULT     crowdGetElements(HANDLE hModule, TVACrowdElement* elements)
{
	TheCrowd* ts = (TheCrowd*)hModule;
	if (ts->size != sizeof(TheCrowd))
		return E_FAIL;
	int num = ts->s->GetItemsCount();
	for (int i = 0; i < num; i++)
	{
		TLFDetectedItem* di = ts->s->GetItem(i);
		TLFRect* r = di->GetBounds();
		elements[i].XPos = r->GetRect().left;
		elements[i].YPos = r->GetRect().top;
		elements[i].Width = r->Width();
		elements[i].Height = r->Height();
	}
	return S_OK;
}
CROWD_API HRESULT  ñrowdGetNumObjects(HANDLE hModule, int& num)
{
	TheCrowd* ts = (TheCrowd*)hModule;
	if (ts->size != sizeof(TheCrowd))
		return E_FAIL;
	num = 0;// ts->s->GetItemsCount();
	return S_OK;
}

