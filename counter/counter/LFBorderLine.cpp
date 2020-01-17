#include "LFBorderLine.h"
#include "LFSensorLine.h"
#include "LFIplImage.h"

CBorderLine::CBorderLine()
{
	m_sp = cvPoint2D32f(0, 0);
	m_ep = cvPoint2D32f(0, 0);
	m_angle = -1;
}

CBorderLine::~CBorderLine()
{
}

void CBorderLine::DrawBorderLine(IplImage* img)
{
	CvPoint sp = GetSp();
	CvPoint ep = GetEp();
	cvLine(img, sp, ep, CV_RGB(0,255,255),3);
}

CvPoint CBorderLine::GetSp(int width, int height)
{
	CvPoint p;
	p.x = 0.01*m_sp.x*width;
	p.y = 0.01*m_sp.y*height;
	return p;
}
void CBorderLine::SetSp(CvPoint p, int width, int height)
{
	m_sp.x = 100.*p.x/(float)width;
	m_sp.y = 100.*p.y /(float)height;
	m_ep = m_sp;
	m_angle = -1;
}

CvPoint CBorderLine::GetEp(int width, int height)
{
	CvPoint p;
	p.x = 0.01*m_ep.x*width;
	p.y = 0.01*m_ep.y*height;
	return p;
}
void CBorderLine::SetEp(CvPoint p, int width, int height)
{
	m_ep.x = 100.*p.x / (float)width;
	m_ep.y = 100.*p.y / (float)height;
	CalcAngle();
}

double CBorderLine::Lenght(int width, int height)
{
	CvPoint sp = GetSp();
	CvPoint ep = GetEp();

	return sqrt((sp.x - ep.x)*(sp.x - ep.x) + (sp.y - ep.y)*(sp.y - ep.y));
}
double CBorderLine::Angle()
{
	return m_angle;
}

void CBorderLine::CalcAngle()
{
	double l = Lenght();
	if (l < 10)
	{
		m_angle = -1;
		return;
	}
	CvPoint s = cvPoint(0, 0);
	CvPoint e = cvPoint(1, 0);
	CvPoint ee = GetEp();
	CvPoint sp = GetSp();
	ee.x -= sp.x;
	ee.y -= sp.y;

	double ll = sqrt((s.x - e.x)*(s.x - e.x) + (s.y - e.y)*(s.y - e.y));
	ll *= l;
	double scalar = e.x*ee.x + e.y*e.y;
	double coss = scalar / ll;
	m_angle =  m_ep.y > m_sp.y ? 180 * acos(coss) / 3.14159 : 360 - 180 * acos(coss) / 3.14159;
}


int CBorderLine::Direction(double angle)
{
	if (fabs(m_angle - angle) < 5)
		return 0;
	if (m_angle > 0 && m_angle <= 90)
	{
		if (angle < m_angle || angle > m_angle + 180)
			return 1;
		else
			return -1;
	}
	else if (m_angle > 90 && m_angle <= 180)
	{
		if (angle < m_angle || angle > m_angle + 180)
			return 1;
		else
			return -1;
	}
	else if (m_angle > 180 && m_angle <= 270)
	{
		if (angle < m_angle && angle >= m_angle - 180)
			return 1;
		else
			return -1;
	}
	else if (m_angle > 270)
	{
		if (angle < m_angle && angle >= m_angle - 180)
			return 1;
		else
			return -1;
	}
}
