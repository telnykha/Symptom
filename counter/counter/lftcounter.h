//---------------------------------------------------------------------------

#ifndef lftcounterH
#define lftcounterH
#include "_LF.h"
#include "vautils.h"
#include "track.h"

#define DIR_IN 		0
#define DIR_OUT     1
#define DIR_BOTH	2
//---------------------------------------------------------------------------
// LF trajectory counter

class TLFTCounter
{
protected:
	TLFFGEngine m_engine;
    TLFImage     m_mask;
    TLFZones     m_zones;
	double	     m_average_square;
	double		 m_total_square;
    std::map<string, vector<TVABlob> > Trajectories;
    awpRect crect;
    TVAResult  result;
    int num_cross;
    int m_dir;
    TVATrajectories out_trajectories;
	void InitEngine(double sens);

public:
    TLFTCounter(TVAPoint& start, TVAPoint& finish, TVARect& sizes, double eventSens, int dir = DIR_BOTH);
	TLFTCounter(TLFZones& zones, double eventSens, int dir = DIR_BOTH);
    ~TLFTCounter();

    bool ProcessImage(awpImage* img, int& num_in, int& num_out, double& sq);
    bool CreateMask(int w, int h);

    awpImage* GetMask();
	TLFImage* GetForeground();
    awpRect   GetCross();
	void trackTrajectories(TVATrajectories* trajectories);

    double GetThreshold();
    void   SetThreshold(double value);

    int    GetCountDir();
    void   SetCountDir(int Value);

    void SetZones(TLFZones& zones, int w, int h);
};
#endif
