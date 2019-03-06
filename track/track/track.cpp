// track.cpp: определяет экспортированные функции для приложения DLL.
//

#include "track.h"
#include "_LF.h"
#include "LFTrack.h"

typedef struct
{
	int size;
//	TVAInitParams params;
	int NumObjects;
	TLFTrackEngine* engine;
	std::map<string, vector<TVABlob> > Trajectories;
	TVATrajectories out_trajectories;
}TheTrack;


// экспортируемые функции

extern "C" TRACK_API HANDLE		trackCreate(TVAInitParams* params)
{
	
	if (params == NULL)
		return NULL;
	
	TheTrack* track = new TheTrack();
	track->size = sizeof(TheTrack);

	track->out_trajectories.Num = 0;
	track->out_trajectories.Trajectories = NULL;

//	memcpy(&track->params, params, sizeof(TVAInitParams));
	track->NumObjects = params->numObects;
	track->engine = new TLFTrackEngine(*params);
	track->engine->SetNeedTrack(true);
	track->engine->SetResize(true);
	track->engine->SetBaseImageWidth(320);
	track->engine->SetNeedCluster(true);

	ILFObjectDetector* s = track->engine->GetDetector(0);
	s->SetBaseWidht(4);
	s->SetBaseHeight(4);
	s->SetThreshold(params->EventSens);
	return (HANDLE)track;
}
extern "C" TRACK_API HRESULT		trackProcess(HANDLE hModule, int width, int height, int bpp, unsigned char* data, TVAResult* result)
{
	TheTrack* p = (TheTrack*)hModule;
	if (p->size != sizeof(TheTrack))
		return E_FAIL;
	awpImage* tmp = NULL;
	awpCreateGrayImage(&tmp, width, height, bpp, data);


	p->engine->SetSourceImage(tmp, true);
	int n = p->engine->GetItemsCount();

	result->Num = n < p->NumObjects ? n : p->NumObjects;
	for (int i = 0; i < result->Num; i++)
	{
		TLFDetectedItem*  di = p->engine->GetItem(i);
		if (di != NULL)
		{
			awpRect rr = di->GetBounds()->GetRect();
			UUID id;
			di->GetId(id);
			memcpy(&result->blobs[i].id, &id, sizeof(UUID));
			result->blobs[i].XPos = rr.left;
			result->blobs[i].YPos = rr.top;
			result->blobs[i].Width = rr.right - rr.left;
			result->blobs[i].Height = rr.bottom - rr.top;
			result->blobs[i].status = di->GetState();

			std::string str_uuid = LFGUIDToString(&result->blobs[i].id);
			TVABlob b;
			memcpy(&b.id, (const void*)&result->blobs[i].id, sizeof(UUID));
			b.XPos = result->blobs[i].XPos;
			b.YPos = result->blobs[i].YPos;
			b.Width = result->blobs[i].Width;
			b.Height = result->blobs[i].Height;
			b.status = result->blobs[i].status;

			if (result->blobs[i].status == 1)
			{
				std::vector<TVABlob> tr;
				b.time = 1;
				tr.push_back(b);
				p->Trajectories.insert(std::pair<string, vector<TVABlob> >(str_uuid, tr));
			}
			else if (result->blobs[i].status == 2)
			{
				std::map<string, vector<TVABlob> >::iterator it = p->Trajectories.begin();
				it = p->Trajectories.find(str_uuid);
				if (it != p->Trajectories.end())
				{
					it->second[it->second.size() - 1].time++;
					if (it->second[it->second.size() - 1].time % 2 == 0)
					{
						b.time = it->second[it->second.size() - 1].time;
						it->second.push_back(b);
					}
					
				}
					
			}
			else if (result->blobs[i].status == 3)
			{
				std::map<string, vector<TVABlob> >::iterator it = p->Trajectories.begin();
				it = p->Trajectories.find(str_uuid);
				if (it != p->Trajectories.end())
					p->Trajectories.erase(it);
			}
		}
	}

	awpReleaseImage(&tmp);
	return S_OK;
}

extern "C" TRACK_API HRESULT		trackForeground(HANDLE hModule, int width, int height, unsigned char* data)
{
	TheTrack* p = (TheTrack*)hModule;
	if (p->size != sizeof(TheTrack))
		return E_FAIL;

	awpImage* img = p->engine->GetForeground();
	if (img != NULL)
	{
		awpImage* tmp = NULL;
		awpCopyImage(img, &tmp);

		if (tmp->sSizeX != width || tmp->sSizeY != height)
			awpResize(tmp, width, height);
		memcpy(data, tmp->pPixels, width*height*sizeof(unsigned char));
		_AWP_SAFE_RELEASE_(tmp);
	}

	return S_OK;

}

extern "C" TRACK_API HRESULT		trackRelease(HANDLE* hModule)
{
	TheTrack* p = (TheTrack*)(*hModule);
	if (p->size != sizeof(TheTrack))
		return E_FAIL;

	delete p->engine;
	delete p;
	p = NULL;

	return S_OK;
}

extern "C" TRACK_API HRESULT		trackTrajectories(HANDLE hModule, TVATrajectories* trajectories)
{
	TheTrack* p = (TheTrack*)hModule;
	if (p->size != sizeof(TheTrack))
		return E_FAIL;

	if (p->out_trajectories.Num != 0)
	{
		for (int i = 0; i < p->out_trajectories.Num; i++)
		{
	//		for (int j = 0; j < p->out_trajectories.Trajectories[i].Num; j++)
				free(p->out_trajectories.Trajectories[i].blobs);
		}
		free(p->out_trajectories.Trajectories);
	}

	p->out_trajectories.Num = p->Trajectories.size();
	if (p->out_trajectories.Num > 0)
	{
		p->out_trajectories.Trajectories = (TVATrajectory*)malloc(p->out_trajectories.Num*sizeof(TVATrajectory));
		std::map<string, vector<TVABlob> >::iterator it;// = p->Trajectories.begin();
		int count = 0;
		for (it = p->Trajectories.begin(); it != p->Trajectories.end(); ++it)
		{
			p->out_trajectories.Trajectories[count].Num = it->second.size();
			p->out_trajectories.Trajectories[count].blobs = (TVABlob*)malloc(p->out_trajectories.Trajectories[count].Num*sizeof(TVABlob));
			memcpy(&p->out_trajectories.Trajectories[count].id, &it->second[0].id, sizeof(UUID));
			for (int i = 0; i < it->second.size(); i++)
				memcpy(&p->out_trajectories.Trajectories[count].blobs[i], &it->second[i], sizeof(TVABlob));
			count++;
		}
	}

	trajectories->Num = p->out_trajectories.Num;
	trajectories->Trajectories = p->out_trajectories.Trajectories;
	return S_OK;
}

static double LineSegmentLength(awp2DLineSegment s)
{
	return sqrt((s.strat.X - s.end.X)*(s.strat.X - s.end.X) + (s.strat.Y - s.end.Y)*(s.strat.Y - s.end.Y));
}
double _awpL2Distance(awp2DPoint p1, awp2DPoint p2)
{
	return sqrt((p1.X - p2.X)*(p1.X - p2.X) + (p1.Y - p2.Y)*(p1.Y - p2.Y));
}
static awp2DPoint FindCrossPoint(awp2DLineSegment s1, awp2DLineSegment s2)
{
	double A1, B1, C1, A2, B2, C2;

	A1 = s1.end.Y - s1.strat.Y;
	B1 = s1.strat.X - s1.end.X;
	C1 = s1.end.X*s1.strat.Y - s1.strat.X*s1.end.Y;

	A2 = s2.end.Y - s2.strat.Y;
	B2 = s2.strat.X - s2.end.X;
	C2 = s2.end.X*s2.strat.Y - s2.strat.X*s2.end.Y;

	awp2DPoint result;
	result.X = 0;
	result.Y = 0;

	double det = (A1*B2 - A2*B1);
	if (det == 0)
		return result;
	double detx = (-C1)*B2 - (-C2)*B1;
	double dety = A1*(-C2) - A2*(-C1);

	result.X = detx / det;
	result.Y = dety / det;

	return result;

}
static bool CheckCrossPoint(awp2DLineSegment s1, awp2DLineSegment s2, awp2DPoint p)
{
	double sd1 = LineSegmentLength(s1);
	double sd2 = LineSegmentLength(s2);
	double d1 = _awpL2Distance(p, s1.strat);
	double d2 = _awpL2Distance(p, s1.end);
	if (d1 + d2 - sd1 > 0.1)
		return false;
	d1 = _awpL2Distance(p, s2.strat);
	d2 = _awpL2Distance(p, s2.end);
	if (d1 + d2 - sd2 > 0.1)
		return false;
	return true;
}

static double awp2DSegmentsAngle(awp2DLineSegment s1, awp2DLineSegment s2)
{
	double norm1 = LineSegmentLength(s1);
	double norm2 = LineSegmentLength(s2);
	double norm = norm1*norm2;
	if (norm == 0)
		return 0;

	awp2DPoint p1, p2;
	p1.X = s1.end.X - s1.strat.X;
	p1.Y = s1.end.Y - s1.strat.Y;

	p2.X = s2.end.X - s2.strat.X;
	p2.Y = s2.end.Y - s2.strat.Y;

	double scalar = (p1.X*p2.X + p1.Y*p2.Y) / norm;
	double res;
	if (p1.X*p2.Y - p2.X*p1.Y >= 0)
		res = -180 * acos(scalar) / AWP_PI;
	else
		res = 180 * acos(scalar) / AWP_PI;
	return res;
}

extern "C" TRACK_API HRESULT		trackCheckLine(HANDLE hModule, TVAPoint& p1, TVAPoint& p2, TVATrajectory* t, bool* result, int* dir)
{
	TheTrack* p = (TheTrack*)hModule;
	if (p->size != sizeof(TheTrack))
		return E_FAIL;
	if (t == NULL || t->Num < 2 || t->blobs == NULL)
		return E_FAIL;

	awp2DPoint point1;
	awp2DPoint point2;
	point1.X = t->blobs[t->Num - 2].XPos + t->blobs[t->Num - 2].Width / 2;
	point1.Y = t->blobs[t->Num - 2].YPos + t->blobs[t->Num - 2].Height / 2;
	point2.X = t->blobs[t->Num - 1].XPos + t->blobs[t->Num - 1].Width / 2;
	point2.Y = t->blobs[t->Num - 1].YPos + t->blobs[t->Num - 1].Height / 2;

	int bw = p->engine->GetBaseImageWidth();
	double    alfa = p->engine->GetResizeCoef();
	alfa *= bw;

	point2.X = 100 * point2.X / alfa;
	point2.Y = 100 * point2.Y / alfa;
	point1.X = 100 * point1.X / alfa;
	point1.Y = 100 * point1.Y / alfa;

	awp2DLineSegment line;
	awp2DPoint s;
	awp2DPoint e;

	s.X = p1.X;
	s.Y = p1.Y;

	e.X = p2.X;
	e.Y = p2.Y;

	line.strat = s;
	line.end = e;

	awp2DLineSegment segment;
	segment.strat = point1;
	segment.end   = point2;

	awp2DPoint cross = FindCrossPoint(line, segment);
	*result = false;
	if (CheckCrossPoint(segment, line, cross))
	{
		int a = awp2DSegmentsAngle(line, segment) > 0 ? 1 : -1;
		*dir = a;
		*result = true;
	}

	return S_OK;
}

extern "C" TRACK_API HRESULT		trackCheckZone(HANDLE hModule, TVAZone* z, TVATrajectory* t, bool* result)
{
	TheTrack* p = (TheTrack*)hModule;
	if (p->size != sizeof(TheTrack))
		return E_FAIL;
	if (t == NULL || t->Num == 0 || t->blobs == NULL)
		return E_FAIL;
	if (z == NULL)
		return E_FAIL;

	awp2DPoint point;
	point.X = t->blobs[t->Num - 1].XPos + t->blobs[t->Num - 1].Width / 2;
	point.Y = t->blobs[t->Num - 1].YPos + t->blobs[t->Num - 1].Height / 2;

	int bw = p->engine->GetBaseImageWidth();
	double    alfa = p->engine->GetResizeCoef();
	alfa *= bw;

	point.X = 100 * point.X / alfa;
	point.Y = 100 * point.Y / alfa;

	TLFZones zones;
	// ковертация зоны в TLFZone 
	if (z->IsRect)
	{
		TLF2DRect rect;
		rect.SetVertexes(z->Rect.LeftTop.X, z->Rect.RightBottom.X, z->Rect.LeftTop.Y, z->Rect.RightBottom.Y);
		TLFZone*  zone = new TLFZone(rect);
		zones.AddZone(zone);
	}
	else
	{
		TLF2DContour* countour = new TLF2DContour();
		for (int i = 0; i < z->NumPoints; i++)
		{
			awp2DPoint pp;
			pp.X = z->Points[i].X;
			pp.Y = z->Points[i].Y;
			countour->AddPoint(pp);
		}
		TLFZone*  zone = new TLFZone(*countour);
		zones.AddZone(zone);
	}
	
	*result = zones.TestPoint(point.X, point.Y, 0);

	return S_OK;
}
