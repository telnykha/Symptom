#ifndef _lf_borderline_h_
#define _lf_borderline_h_

#include "opencv\cv.h"

class CSensorLine;
class CIplImage;

class CBorderLine
{
protected:
	CvPoint2D32f m_sp;
	CvPoint2D32f m_ep;

	double m_angle;
	void CalcAngle();

public: 
	CBorderLine();
	~CBorderLine();
	void DrawBorderLine(IplImage* img);

	CvPoint GetSp(int width = 640, int height = 480);
	void SetSp(CvPoint p, int width = 640, int height = 480);

	CvPoint GetEp(int width = 640, int height = 480);
	void SetEp(CvPoint p, int width = 640, int height = 480);

	double Lenght(int width = 640, int height = 480);
	double Angle();
	int Direction(double angle);
};

#endif //_lf_borderline_h_