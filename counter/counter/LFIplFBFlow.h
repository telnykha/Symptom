#ifndef _lf_ipl_fbflow_h_
#define _lf_ipl_fbflow_h_
#include "LFIplImage.h"
#include "LFBorderLine.h"

class CFBFlow
{
public:
	static void DrawFlow(IplImage* img, IplImage* module, IplImage* angle, double t, int result);
	static void DrawFlowWithLine(IplImage* img, IplImage* module, IplImage* angle, double t, CBorderLine* line);

private:
	double m_scale;
	double m_pyrScale;
	int    m_levels;
	int    m_winSize;
	int    m_iterations;
	int	   m_poly_n;
	double m_polySigma;
	int	   m_flags;
	double m_threshold;
	double m_moveCount;
	CvScalar m_diff;


	CImageBuffer m_buffer;

protected:

	// images 
	IplImage* m_cur;
	IplImage* m_prev;
	IplImage* m_dst;
	IplImage* m_dst0;


	IplImage* m_module;
	IplImage* m_angle;
	IplImage* m_vflow;
	IplImage* m_hflow;
	IplImage* m_mask;

	void FreeImages();
	void MakeMask();
	void MakeMoule();
public:
	CFBFlow(double scale = 2);
	~CFBFlow();
	void Process(IplImage* image);
	IplImage* GetVFlow();
	IplImage* GetHFlow();
	IplImage* GetModule();
	IplImage* GetAngle();
	IplImage* GetMask();
	IplImage* GetFG();

	double GetThreshold();
	void   SetThreshold(double value);

	double   GetMoveCount();
	double   GetScale();
};


#endif // _lf_ipl_fbflow_h_