#ifndef _lf_fight_module_
#define _lf_fight_module_
#include "_LF.h"
#include "vautils.h"
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include "opencv2\video.hpp"
#include "opencv2\video\background_segm.hpp"

class CIplImage : public TLFObject
{
private:
	IplImage* m_pImage;
public:
	CIplImage();
	virtual ~CIplImage();

	void SetImage(IplImage* image);
	IplImage* GetImage();
};

class CImageBuffer : public TLFImageList
{
private:
	int m_bufferSize;
public:
	CImageBuffer(int bufferSize = 10);
	void AddImage(CIplImage* image);
	void AddImage(IplImage* image);
	CIplImage* GetImage(int index);
	IplImage*  Average();
};

class CBGWrapper
{
private:
	IplImage* m_foreground;
	TLFMotionEngine m_motion;
	int count;
public:
	CBGWrapper();
	~CBGWrapper();

	void apply(IplImage* src);
	IplImage* GetForeground();
};

class CFBFlow
{
public:
	static void DrawFlow(IplImage* img, IplImage* module, IplImage* angle, double t, int result);
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

	CBGWrapper m_fgbg;
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

class CFlowAnalysis
{
private:
	awpPoint m_centroid;
	double   m_teta;
	double   m_mi;
	double   m_ma;
	int      m_result;
public:
	CFlowAnalysis();
	void ProcessFlow(CFBFlow* flow, bool draw);
	awpPoint GetCentroid();
	CvPoint  GetCvCentroid();
	double GetTeta();
	double GetMi();
	double GetMa();

	int GetResult();
};



class CFightEngine : public ILFDetectEngine
{
protected: 
	CFBFlow processor;
	CFlowAnalysis analysis;
	CIplImage image;
	TVAInitParams		m_params;
	TLFBuffer* m_buffer;
	DWORD m_start_time;
	TVAResult m_fight;
protected:
	virtual void OverlapsFilter(TLFSemanticImageDescriptor* descriptor);
	virtual void InitDetectors();
	virtual bool FindObjects();
public:
	CFightEngine(TVAInitParams& params);
	virtual ~CFightEngine();
	virtual void Clear();
	virtual bool LoadXML(TiXmlElement* parent);
	virtual TiXmlElement*  SaveXML();
	virtual bool DetectInRect(awpRect* rect);
	TVAInitParams* GetParams();

	CFlowAnalysis* GetAnalyser();
	CFBFlow*	   GetFlow();
	TVAResult*     GetFight();
};
#endif 