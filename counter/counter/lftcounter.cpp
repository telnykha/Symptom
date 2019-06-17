//---------------------------------------------------------------------------
#pragma hdrstop
#include "lftcounter.h"
// static utilites
//---------------------------------------------------------------------------
static void awp2DLineSegmentToStraightLine(awp2DLineSegment s, double& A, double& B, double& C)
{
    A = s.end.Y - s.strat.Y;
    B = s.strat.X - s.end.X;
    C = s.end.X*s.strat.Y - s.strat.X*s.end.Y;
}
//---------------------------------------------------------------------------
static awp2DPoint awp2DLineSegmentMiddlePoint(awp2DLineSegment s)
{
	awp2DPoint res;
    res.X = 0.5*(s.strat.X + s.end.X);
    res.Y = 0.5*(s.strat.Y + s.end.Y);
    return res;
}
//---------------------------------------------------------------------------
static awp2DPoint awp2DNormVector(awp2DPoint p, double l = 1)
{
	awp2DPoint res;
    res.X = 0;
    res.Y = 0;
	double L = sqrt(p.X*p.X + p.Y*p.Y);
    if (L == 0)
    	return res;
    res.X = l*p.X / L;
    res.Y = l*p.Y / L;
    return res;
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
static double LineSegmentLength(awp2DLineSegment s)
{
	return sqrt((s.strat.X - s.end.X)*(s.strat.X - s.end.X)+(s.strat.Y - s.end.Y)*(s.strat.Y - s.end.Y));
}

double _awpL2Distance(awp2DPoint p1, awp2DPoint p2)
{
    return sqrt((p1.X -p2.X)*(p1.X - p2.X) + (p1.Y - p2.Y)*(p1.Y - p2.Y));
}

static double awp2DSegmentsAngle(awp2DLineSegment s1, awp2DLineSegment s2)
{
    double norm1 = LineSegmentLength(s1);
    double norm2 = LineSegmentLength(s2);
    double norm = norm1*norm2;
    if (norm == 0)
    	return 0;

    awp2DPoint p1,p2;
    p1.X = s1.end.X - s1.strat.X;
    p1.Y = s1.end.Y - s1.strat.Y;

    p2.X = s2.end.X - s2.strat.X;
    p2.Y = s2.end.Y - s2.strat.Y;

    double scalar = (p1.X*p2.X + p1.Y*p2.Y) / norm;
	double res;
    if (p1.X*p2.Y - p2.X*p1.Y >= 0)
    	res = -180*acos(scalar)/ AWP_PI;
    else
    	res =  180*acos(scalar)/ AWP_PI;
    return res;
}

static bool CheckCrossPoint(awp2DLineSegment s1, awp2DLineSegment s2, awp2DPoint p)
{
	double sd1 =  LineSegmentLength(s1);
    double sd2 =  LineSegmentLength(s2);
    double d1 =  _awpL2Distance(p, s1.strat);
    double d2 =  _awpL2Distance(p, s1.end);
    if (d1 + d2 - sd1 > 0.1)
    	return false;
    d1 =  _awpL2Distance(p, s2.strat);
    d2 =  _awpL2Distance(p, s2.end);
    if (d1 + d2 - sd2 > 0.1)
    	return false;
	return true;
}

static double DistFrom2DPointTo2DSegment(awp2DLineSegment s, awp2DPoint p)
{
    double A, B, C;

    A = s.end.Y - s.strat.Y;
    B = s.strat.X - s.end.X;
    C = s.end.X*s.strat.Y - s.strat.X*s.end.Y;

    if (A*A + B*B == 0)
    	return 0;

    return fabs(A*p.X + B*p.Y + C) / sqrt(A*A + B*B);
}


// tcounter
//---------------------------------------------------------------------------
TLFTCounter::TLFTCounter(TVAPoint& start, TVAPoint& finish, TVARect& sizes, double eventSens, int dir)
{
	this->InitEngine(eventSens);

    // setup zones
    TLFZone* segment = new TLFZone(ZTLineSegment);
    TLFZone* rect    = new TLFZone(ZTRect);

    awp2DPoint p;
    TLF2DLineSegment* ls = segment->GetLineSegmnet();

    p.X = start.X;
    p.Y = start.Y;
    ls->SetStart(p);

    p.X = finish.X;
    p.Y = finish.Y;
    ls->SetFihish(p);

    TLF2DRect* r = rect->GetRect();
    r->SetVertexes(sizes.LeftTop.X,sizes.RightBottom.X,sizes.LeftTop.Y,sizes.RightBottom.Y);

    m_zones.Clear();
    m_zones.AddZone(segment);
    m_zones.AddZone(rect);
	m_average_square = 0;
	m_total_square = 0;
    m_dir = dir;
    memset(&this->out_trajectories, 0, sizeof(TVATrajectories));
}

TLFTCounter::TLFTCounter(TLFZones& zones, double eventSens, int dir)
{
	this->InitEngine(eventSens);

	this->m_zones = zones;

	m_average_square = 0;
	m_total_square = 0;
	m_dir = dir;
	memset(&this->out_trajectories, 0, sizeof(TVATrajectories));
}

void TLFTCounter::InitEngine(double sens)
{
	m_engine.SetNeedCluster(true);
	m_engine.SetNeedTrack(true);
	m_engine.SetAverageBufferSize(5);
	m_engine.SetDelay(0);
	m_engine.SetBgStability(1);
	m_engine.SetBaseImageWidth(256);
	m_engine.SetResize(true);
	m_engine.SetMinAR(0.25);
	m_engine.SetMaxAR(4);
	m_engine.SetMaxSize(3);
	m_engine.SetMaxSize(90);
	ILFObjectDetector* s = m_engine.GetDetector(0);
	s->SetBaseWidht(4);
	s->SetBaseHeight(4);
	s->SetThreshold(sens);

	result.Num = 100;
	result.blobs = (TVABlob*)malloc(result.Num*sizeof(TVABlob));
    num_cross = 0;
}

TLFTCounter::~TLFTCounter()
{
 	if (out_trajectories.Num != 0)
	{
		for (int i = 0; i < out_trajectories.Num; i++)
		{
			free(out_trajectories.Trajectories[i].blobs);
		}
		free(out_trajectories.Trajectories);
	}

    free(result.blobs);
}
bool TLFTCounter::ProcessImage(awpImage* img, int& num_in, int& num_out, double& sq)
{
	if (m_mask.GetImage() == NULL)
    {
		if (!CreateMask(img->sSizeX, img->sSizeY))
        	return false;
    }
    num_in  = 0;
    num_out = 0;
	sq = 0;
    m_engine.SetSourceImage(img, true);
    int n = m_engine.GetItemsCount();
    result.Num = n < 100 ? n : 100;
    for (int i = 0; i < result.Num; i++)
    {
		TLFDetectedItem*  di = m_engine.GetItem(i);
        if (di != NULL)
        {
            awpRect rr = di->GetBounds()->GetRect();
            UUID id;
            di->GetId(id);
            memcpy(&result.blobs[i].id, &id, sizeof(UUID));
			
			
			result.blobs[i].XPos = rr.left;
			result.blobs[i].YPos = rr.top;
			result.blobs[i].Width = (float)floor(0.5 + rr.right - rr.left);
			result.blobs[i].Height = (float)floor(0.5 + rr.bottom - rr.top);
			result.blobs[i].status = di->GetState();

			std::string str_uuid = LFGUIDToString(&result.blobs[i].id);
            TVABlob b;

			memcpy(&b.id, &result.blobs[i].id, sizeof(UUID));
			b.XPos = result.blobs[i].XPos;
			b.YPos = result.blobs[i].YPos;
			b.Width = result.blobs[i].Width;
			b.Height = result.blobs[i].Height;
			b.status = result.blobs[i].status;

            double square1 = (double)(b.Width*b.Height);
            sq = square1/ (double)(img->sSizeX*img->sSizeY);
			if (result.blobs[i].status == 1)
            {
               std::vector<TVABlob> tr;
			   b.time = 1;
               tr.push_back(b);
               Trajectories.insert(std::pair<string, vector<TVABlob> >(str_uuid, tr));
            }
			else if (result.blobs[i].status == 2)
            {
				std::map<string, vector<TVABlob> >::iterator it = Trajectories.begin();
                it = Trajectories.find(str_uuid);
                if (it != Trajectories.end())
                 {

					 it->second[it->second.size() - 1].time++;
					 if (it->second[it->second.size() - 1].time % 4 != 0)
						 continue;
						 
				     b.time = it->second[it->second.size() - 1].time;

                    awp2DLineSegment segment;
                    awp2DPoint s;
                    awp2DPoint e;

                    s.X = it->second[it->second.size()-1].XPos + it->second[it->second.size()-1].Width / 2;
                    s.Y = it->second[it->second.size()-1].YPos + it->second[it->second.size()-1].Height / 2;
                    e.X = b.XPos + b.Width / 2;
                    e.Y = b.YPos + b.Height / 2;
                    segment.strat = s;
                    segment.end   = e;

					it->second.push_back(b);
                    awp2DLineSegment line = m_zones.GetZone(0)->GetLineSegmnet()->GetSegment();


                    double w = m_zones.GetZone(1)->GetRect()->Width();
                    double h = m_zones.GetZone(1)->GetRect()->Height();
                    w = w*img->sSizeX / 100.;
                    h = h*img->sSizeY / 100.;
                    double square0 = w*h;

					line.strat.X = line.strat.X* img->sSizeX / 100.;
                    line.end.X = line.end.X* img->sSizeX / 100.;
                    line.strat.Y = line.strat.Y*img->sSizeY / 100.;
                    line.end.Y = line.end.Y* img->sSizeY / 100.;
                    awp2DPoint cross = FindCrossPoint(line, segment);

                   if(CheckCrossPoint(segment, line, cross) && floor(square1/square0 + 0.5) > 0)
                    {
                        
						num_cross++;
						this->m_total_square += square1;
						this->m_average_square = this->m_total_square / (double)num_cross;
						crect.left = (AWPSHORT)(cross.X - 10);
						crect.right = (AWPSHORT)(cross.X + 10);
						crect.top = (AWPSHORT)(cross.Y - 10);
						crect.bottom = (AWPSHORT)(cross.Y + 10);


                        int a =  awp2DSegmentsAngle(line, segment) > 0 ? 1:-1;
						a *= (int)floor(square1 / m_average_square + 0.5);

                        // event
                        if (a > 0)
                        {
                           if (m_dir == DIR_IN || m_dir == DIR_BOTH)
                           		num_in  += a;
                        }
                        else
                        {
                           if (m_dir == DIR_OUT || m_dir == DIR_BOTH)
	                           num_out += a;
                        }
                     }
                }
           }
			else if (result.blobs[i].status == 3)
            {
                std::map<string, vector<TVABlob> >::iterator it = Trajectories.begin();
                it = Trajectories.find(str_uuid);
                if (it != Trajectories.end())
                    Trajectories.erase(it);
            }
        }
    }
	return true;
}

bool TLFTCounter::CreateMask(int w, int h)
{
    TLF2DRect* rect = m_zones.GetZone(1)->GetRect();
    if (rect == NULL)
    	return false;
    TLF2DLineSegment* ls = m_zones.GetZone(0)->GetLineSegmnet();
    if (ls == NULL)
    	return false;

    int sw,sh;

    sw =(int)floor(0.5 + rect->Width()*w / 100.);
	sh = (int)floor(0.5 + rect->Height()*h / 100.);

    awpImage* img = NULL;
    if (awpCreateImage(&img, w,h, 1, AWP_BYTE) != AWP_OK)
    	return false;
    awp2DLineSegment line = ls->GetSegment();
	line.strat.X  = w*line.strat.X / 100.;
	line.strat.Y  = h*line.strat.Y / 100.;
	line.end.X  = w*line.end.X / 100.;
	line.end.Y  = h*line.end.Y / 100.;

    double radius = sqrt((float)(sw*sw+sh*sh));

    if (img != NULL)
    {
        TLFTileScanner scanner;
        scanner.SetBaseHeight(sh);
        scanner.SetBaseWidth(sw);
		scanner.GetParameter(0)->SetValue(90);

        scanner.Scan(w,h);
		for (int i = 0; i < scanner.GetFragmentsCount(); i++)
        {
			awpRect r = scanner.GetFragmentRect(i);
            awp2DPoint p;
            p.X = (r.left + r.right) / 2.;
            p.Y = (r.top  + r.bottom) / 2.;
            double d = DistFrom2DPointTo2DSegment(line, p);
            if (d < 0.75*radius)
            {
            	awpFillRect(img, &r, 0, 1);
            }
        }
        m_mask.SetImage(img);
        m_engine.SetMaskImage(this->m_mask.GetImage());
		awpReleaseImage(&img);
    }

	return true;
}

awpImage* TLFTCounter::GetMask()
{
	return this->m_mask.GetImage();
}

TLFImage* TLFTCounter::GetForeground()
{
	return m_engine.GetForegroundImage();
}

awpRect   TLFTCounter::GetCross()
{
	return this->crect;
}

void TLFTCounter::trackTrajectories(TVATrajectories* trajectories)
{
	if (out_trajectories.Num != 0)
	{
		for (int i = 0; i < out_trajectories.Num; i++)
		{
			free(out_trajectories.Trajectories[i].blobs);
		}
		free(out_trajectories.Trajectories);
	}

	out_trajectories.Num = Trajectories.size();
	if (out_trajectories.Num > 0)
	{
		out_trajectories.Trajectories = (TVATrajectory*)malloc(out_trajectories.Num*sizeof(TVATrajectory));
		std::map<string, vector<TVABlob> >::iterator it;// = p->Trajectories.begin();
		int count = 0;
		for (it = Trajectories.begin(); it != Trajectories.end(); ++it)
		{
			out_trajectories.Trajectories[count].Num = it->second.size();
			out_trajectories.Trajectories[count].blobs = (TVABlob*)malloc(out_trajectories.Trajectories[count].Num*sizeof(TVABlob));
			memcpy(&out_trajectories.Trajectories[count].id,  &it->second[0].id, sizeof(UUID));
			for (unsigned int i = 0; i < it->second.size(); i++)
				memcpy(&out_trajectories.Trajectories[count].blobs[i], &it->second[i], sizeof(TVABlob));
			count++;
		}
	}

	trajectories->Num = out_trajectories.Num;
	trajectories->Trajectories = out_trajectories.Trajectories;
}

double TLFTCounter::GetThreshold()
{ 
	return 	m_engine.GetDetector(0)->GetThreshold();
}
void   TLFTCounter::SetThreshold(double value)
{
	m_engine.GetDetector(0)->SetThreshold(value);
}

void TLFTCounter::SetZones(TLFZones& zones, int w, int h)
{
	m_zones = zones;
    this->CreateMask(w,h);
}

int    TLFTCounter::GetCountDir()
{
	return m_dir;
}

void   TLFTCounter::SetCountDir(int Value)
{
	if (Value == DIR_IN || Value == DIR_OUT || Value == DIR_BOTH )
    {
    	m_dir = Value;
    }
}



#if  defined(__BCPLUSPLUS__)
	#pragma package(smart_init)
#endif
