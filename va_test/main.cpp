#include "stdio.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "SDL2\include\sdl.h"

#include "motion.h"
#include "sabotage.h"
#include "fire.h"
#include "smoke.h"
#include "crowd.h"
#include "track.h"
#include "counter.h"
#include "package.h"
#include "face.h"

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIHT 480

#define VA_MODULE_SABOTAGE 1
#define VA_MODULE_FIRE	   2
#define VA_MODULE_SMOKE    3
#define VA_MODULE_PACKAGE  4
#define VA_MODULE_MOTION   5
#define VA_MODULE_COUNTER  6
#define VA_MODULE_CROWD    7
#define VA_MODULE_HEATMAP  8
#define VA_MODULE_CAR      9
#define VA_MODULE_FACE     10
#define VA_MODULE_HUMAN    11
#define VA_MODULE_TRAINS   12
#define VA_MODULE_FIGHT    13
#define VA_MODULE_TRACK    14


#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD   -12
#define SUCCESS        0

extern "C"
{
	#pragma comment(lib, "opencv_world300.lib")
#ifdef _WIN64
	#pragma comment(lib, "sdl2/lib/x64/sdl2.lib")
	#pragma comment(lib, "sdl2/lib/x64/sdl2main.lib")
#else
	#pragma comment(lib, "sdl2/lib/x86/sdl2.lib")
	#pragma comment(lib, "sdl2/lib/x86/sdl2main.lib")
#endif
}


bool		gquit;
IplImage*   gframe = NULL;
CvCapture* capture = NULL;
SDL_mutex*  mutex;
SDL_Thread* thread;

static int ThreadCapture(void* args)
{
	int i, num = 0;
	gquit = false;
	printf("Enter a thread\n");
	while (1)
	{
		if (gframe == NULL)
		{
			SDL_LockMutex(mutex);
			gframe = cvQueryFrame(capture);
			if (gframe == NULL)
			{
				printf("thread: gframe == NULL\n");
				gquit = true;
			}
			SDL_UnlockMutex(mutex);
		}
		if (gquit)
			break;
	}
	printf("Quit a thread\n");
	SDL_UnlockMutex(mutex);
	return SUCCESS;
}


void Usage()
{
	printf("Usage: \n");
	printf("symptom -key -input [path]\n");
	printf("-key = \n");
	printf("--motion   	Motion detector\n");
	printf("--sabotage 	Sabotage detector\n");
	printf("--fire 		Fire detector\n");
	printf("--smoke 	Smoke detector\n");
	printf("--package	Leaving bags and packages detecor\n");
	printf("--track		BLOB tracking system\n");
	printf("--counter	BLOB counter system \n");
	printf("--crowd		Crowd objects counter\n");
	printf("--heatmap	Motion heatmap creator \n");
	printf("--face		Face detector\n");
	printf("--human		Human detector\n");
	printf("--car 		Car detector \n");
	printf("--trains	Train number detector\n");
	printf("--fight		Fight detector\n");
}

int GetModuleCode(char* key)
{
	if (strcmp(key, "--motion") == 0)
		return VA_MODULE_MOTION;
	else if (strcmp(key, "--sabotage") == 0)
		return VA_MODULE_SABOTAGE;
	else if (strcmp(key, "--fire") == 0)
		return VA_MODULE_FIRE;
	else if (strcmp(key, "--smoke") == 0)
		return VA_MODULE_SMOKE;
	else if (strcmp(key, "--package") == 0)
		return VA_MODULE_PACKAGE;
	else if (strcmp(key, "--track") == 0)
		return VA_MODULE_TRACK;
	else if (strcmp(key, "--counter") == 0)
		return VA_MODULE_COUNTER;
	else if (strcmp(key, "--crowd") == 0)
		return VA_MODULE_CROWD;
	else if (strcmp(key, "--heatmap") == 0)
		return VA_MODULE_HEATMAP;
	else if (strcmp(key, "--face") == 0)
		return VA_MODULE_FACE;
	else if (strcmp(key, "--human") == 0)
		return VA_MODULE_HUMAN;
	else if (strcmp(key, "--car") == 0)
		return VA_MODULE_CAR;
	else if (strcmp(key, "--trains") == 0)
		return VA_MODULE_TRAINS;
	else if (strcmp(key, "--fight") == 0)
		return VA_MODULE_FIGHT;
	else 
		return -1;
}


class CPerfomanceCounter
{
protected: 
	double PCFreq;
	__int64 CounterStart;
public:
	CPerfomanceCounter()
	{
		PCFreq = 0;
		CounterStart = 0;
		LARGE_INTEGER li;

		if (!QueryPerformanceFrequency(&li))
		{
			printf("QueryPerformanceFrequency failed!\n");
		}

		PCFreq = double(li.QuadPart) / 1000.0;

		QueryPerformanceCounter(&li);
		CounterStart = li.QuadPart;
	}
	double GetCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart - CounterStart) / PCFreq;
	}
};

class IVideoAnalysis
{
protected:
	HANDLE		  m_module;
	TVAInitParams m_params;
public:
	IVideoAnalysis(TVAInitParams* params)
	{
		m_module = NULL;
	}
	virtual ~IVideoAnalysis()
	{
	}
	
	virtual void InitModule(TVAInitParams* params) = 0;
	virtual void ReleaseModule() = 0;
	virtual void ProcessData(unsigned char* data, int width, int height, int bpp) = 0;
	virtual void DrawResult(unsigned char* data, int width, int height, int bpp) = 0;
};

class CMotionModule : public IVideoAnalysis
{
private:
	int m_sens;
public:	
	CMotionModule(TVAInitParams* params) : IVideoAnalysis(params){}

	virtual void InitModule(TVAInitParams* params)
	{
		MotionDetectorCNT* m = InitMotionDetector();
		if (m == NULL)
		{
			printf("ERROR: cannot create module MOTION.\n");
			exit(-1);
		}
		m_sens = (int)(100.*params->EventSens);
		m_module = (HANDLE)m;
	}
	virtual void ReleaseModule()
	{
		MotionDetectorCNT* m = (MotionDetectorCNT*)m_module;
		FreeMotionDetector(m);
	}

	virtual void ProcessData(unsigned char* data, int width, int height, int bpp)
	{
		MotionDetectorCNT* m = (MotionDetectorCNT*)m_module;
		AnalyzeMotionDetectorArgb(m, (char*)data, 3*width, width, height, m_sens, 1, 1);
	}
	virtual void DrawResult(unsigned char* data, int width, int height, int bpp)
	{
		MotionDetectorCNT* m = (MotionDetectorCNT*)m_module;
		CvSize s;
		s.width = width;
		s.height = height;
		IplImage* img = cvCreateImageHeader(s, IPL_DEPTH_8U, 3); 
		img->imageData = (char*)data;
		
		int count = 0;
		_CvRect* r = GetMotionDetectorRects(m, count);
		printf("MOTION MODULE: count of moving objects = %i \n", count);

		// draw result 
		for (int i = 0; i < count; i++)
		{
			CvRect rr = cvRect(r[i].x, r[i].y, r[i].width, r[i].height);
			CvPoint p1,p2;
			p1.x = rr.x;
			p1.y = rr.y;
			p2.x = rr.x + rr.width;
			p2.y = rr.y + rr.height;

			cvRectangle(img, p1, p2, CV_RGB(0, 255, 0), 1);
		}
		cvReleaseImageHeader(&img);
	}
};

class CSabotageModule : public IVideoAnalysis
{
public:
	CSabotageModule(TVAInitParams* params) : IVideoAnalysis(params){}
	
	virtual void InitModule(TVAInitParams* params)
	{
		m_module = (HANDLE)sabotageCreate(params);
		if (m_module == NULL)
		{
			printf("ERROR: cannot create module SABOTAGE.\n");
			exit(-1);
		}
	}
	
	virtual void ReleaseModule()
	{
		sabotageRelease (&m_module);
	}

	virtual void ProcessData(unsigned char* data, int width, int height, int bpp)
	{
		bool result = false;
		sabotageProcess(m_module,  width, height, bpp, data, &result);
	}

	virtual void DrawResult(unsigned char* data, int width, int height, int bpp)
	{
		int state = NO_SABOTAGE;
		sabotageState(m_module, &state);
		if (state != NO_SABOTAGE)
			printf("MODULE SABOTAGE: detect sabotage\n");
		else 
			printf("MODULE SABOTAGE: status ok\n");
	}
};

class CFireModule : public IVideoAnalysis
{
public:
	CFireModule(TVAInitParams* params) : IVideoAnalysis(params){}

	virtual void InitModule(TVAInitParams* params)
	{
		m_module = (HANDLE)fireCreate(params);
		if (m_module == NULL)
		{
			printf("ERROR: cannot create module FIRE.\n");
			exit(-1);
		}
	}

	virtual void ReleaseModule()
	{
		fireRelease(&m_module);
	}

	virtual void ProcessData(unsigned char* data, int width, int height, int bpp)
	{
		bool result = false;
		fireProcess(m_module, width, height, bpp, data, &result);
		if (result)
			printf("MODULE FIRE: fire detectede\n");
		else
			printf("MODULE FIRE: status ok\n");
	}

	virtual void DrawResult(unsigned char* data, int width, int height, int bpp)
	{
		int num = 0;
		fireGetNumElements(m_module, num);
		if (num > 0)
		{
			// todo:
		}
	}

};

class CSmokeModule : public IVideoAnalysis
{
public:
	CSmokeModule(TVAInitParams* params) : IVideoAnalysis(params){}

	virtual void InitModule(TVAInitParams* params)
	{
		m_module = (HANDLE)smokeCreate(params);
		if (m_module == NULL)
		{
			printf("ERROR: cannot create module SMOKE.\n");
			exit(-1);
		}
	}

	virtual void ReleaseModule()
	{
		smokeRelease(&m_module);
	}

	virtual void ProcessData(unsigned char* data, int width, int height, int bpp)
	{
		bool result = false;
		smokeProcess(m_module, width, height, bpp, data, &result);
		if (result)
			printf("MODULE SMOKE: smoke detectede\n");
		else
			printf("MODULE SMOKE: status ok\n");
	}

	virtual void DrawResult(unsigned char* data, int width, int height, int bpp)
	{
		int num = 0;
		smokeGetNumElements(m_module, num);
		if (num > 0)
		{
			// todo:
		}
	}

};

class CCrowdModule : public IVideoAnalysis
{
public:
	CCrowdModule(TVAInitParams* params) : IVideoAnalysis(params){}

	virtual void InitModule(TVAInitParams* params)
	{
		m_module = (HANDLE)crowdCreate(params);
		if (m_module == NULL)
		{
			printf("ERROR: cannot create module CROWD.\n");
			exit(-1);
		}
	}

	virtual void ReleaseModule()
	{
		crowdRelease(&m_module);
	}

	virtual void ProcessData(unsigned char* data, int width, int height, int bpp)
	{
		crowdProcess(m_module, width, height, bpp, data);
	}

	virtual void DrawResult(unsigned char* data, int width, int height, int bpp)
	{
		int num = 0;
		crowdGetNumElements(m_module, num);
		if (num > 0)
		{
			// todo:
		}
	}

};


class CTrackModule : public IVideoAnalysis
{
public:
	CTrackModule(TVAInitParams* params) : IVideoAnalysis(params){}

	virtual void InitModule(TVAInitParams* params)
	{

		TVAInitParams p = *params;
		p.SaveLog = false;
		p.Path = "";
		p.useTrack = true;
		memset(&p.Camera, 0, sizeof(p.Camera));
		p.NumZones = 0;
		p.NumZones = NULL;
		p.EventSens = 0.5;
		p.EventTimeSens = 0;
		p.minWidth = 1;
		p.maxHeight = 1;
		p.maxWidth = 10;
		p.maxHeight = 10;
		p.numObects = 4;

		m_module = (HANDLE)trackCreate(&p);
		if (m_module == NULL)
		{
			printf("ERROR: cannot create module TRACK.\n");
			exit(-1);
		}
	}

	virtual void ReleaseModule()
	{
		trackRelease(&m_module);
	}

	virtual void ProcessData(unsigned char* data, int width, int height, int bpp)
	{
		TVAResult result;
		result.Num = 100;
		result.blobs = new TVABlob[100];
		trackProcess(m_module, width, height, bpp, data, &result);

		delete result.blobs;
	}

	virtual void DrawResult(unsigned char* data, int width, int height, int bpp)
	{
		int num = 0;
		if (num > 0)
		{
			// todo:
		}
	}
};

class CCounterModule : public IVideoAnalysis
{
public:
	CCounterModule(TVAInitParams* params) : IVideoAnalysis(params){}

	virtual void InitModule(TVAInitParams* params)
	{

		TVAPoint start;

		start.X = 0.6;
		start.Y = 52;

		TVAPoint finish;

		finish.X = 98;
		finish.Y = 52;

		TVARect sizes;

		sizes.LeftTop.X = 60;
		sizes.LeftTop.Y = 26;

		sizes.RightBottom.X = 98;
		sizes.RightBottom.Y = 98;

		double eventSens = 0.5;
		m_module = (HANDLE)tcounterCreate(start, finish, sizes, eventSens);
		if (m_module == NULL)
		{
			printf("ERROR: cannot create module COUNTER.\n");
			exit(-1);
		}
	}

	virtual void ReleaseModule()
	{
		tcounterRelease(&m_module);
	}

	virtual void ProcessData(unsigned char* data, int width, int height, int bpp)
	{
		double result = 0;
		tcounterProcess(m_module, width, height, bpp, data, result);
		if (result != 0)
			printf("MODULE COUNTER : count = %lf \n", result);
	}

	virtual void DrawResult(unsigned char* data, int width, int height, int bpp)
	{
		int num = 0;
		if (num > 0)
		{
			// todo:
		}
	}
};


class CPackageModule : public IVideoAnalysis
{
private:
	TVAPackageResult m_result;
public:
	CPackageModule(TVAInitParams* params) : IVideoAnalysis(params)
	{
		m_result.objects = new TVAPackageBlob[10];
	}
	~CPackageModule()
	{
		delete m_result.objects;
	}
	virtual void InitModule(TVAInitParams* params)
	{
		TVAPackageInit pparams;
		pparams.maxHeight = 10;
		pparams.maxWidth = 10;
		pparams.minHeight = 1;
		pparams.minWidth = 1;
		pparams.Mode = 0;
		pparams.numObects = 10;

		params->EventTimeSens = 5;

		m_module = (HANDLE)packageCreate(params, &pparams);
		if (m_module == NULL)
		{
			printf("ERROR: cannot create module PACKAGE.\n");
			exit(-1);
		}
	}

	virtual void ReleaseModule()
	{
		packageRelease(&m_module);
	}

	virtual void ProcessData(unsigned char* data, int width, int height, int bpp)
	{

		packageProcess(m_module, width, height, bpp, data, &m_result);
		if (m_result.num >0)
			printf("MODULE PACKAGE : count = %i \n", m_result.num);
	}

	virtual void DrawResult(unsigned char* data, int width, int height, int bpp)
	{

		CvSize s;
		s.width = width;
		s.height = height;
		IplImage* img = cvCreateImageHeader(s, IPL_DEPTH_8U, 3);
		img->imageData = (char*)data;

		// 
		CvRect rr = cvRect(10, 10, 24, 24);
		CvPoint p1, p2;
		p1.x = rr.x;
		p1.y = rr.y;
		p2.x = rr.x + rr.width;
		p2.y = rr.y + rr.height;
		cvRectangle(img, p1, p2, CV_RGB(0, 255, 0), 2);

		if (m_result.num > 0)
		{
			// draw result 
			for (int i = 0; i < m_result.num; i++)
			{
				CvRect rr = cvRect(m_result.objects[i].XPos, m_result.objects[i].YPos, m_result.objects[i].Width, m_result.objects[i].Height);
				CvPoint p1,p2;
				p1.x = rr.x;
				p1.y = rr.y;
				p2.x = rr.x + rr.width;
				p2.y = rr.y + rr.height;
				cvRectangle(img, p1,p2, CV_RGB(0, 255, 0), 1);
			}
		}
		cvReleaseImageHeader(&img);
	}
};


class CFaceModule : public IVideoAnalysis
{
private:
	TVAFace* m_faces;
	int      m_num;
	int		 m_width;
	double	 m_scale;
	double   m_grow;
	bool     m_tilt;
public:
	CFaceModule(TVAInitParams* params) : IVideoAnalysis(params)
	{
		m_faces = new TVAFace[10];
		m_num = 0;

		// settings 
		m_width = 320;
		m_scale = 1;
		m_grow  = 1.1;
		m_tilt = true;
	}
	~CFaceModule()
	{
		delete m_faces;
	}
	virtual void InitModule(TVAInitParams* params)
	{

		params->Path = "../data/face.xml";
		m_module = (HANDLE)faceCreate(params, m_scale,  m_grow, m_width, m_tilt, 10);
		if (m_module == NULL)
		{
			printf("ERROR: cannot create module FACE.\n");
			exit(-1);
		}
	}

	virtual void ReleaseModule()
	{
		faceRelease(&m_module);
	}

	virtual void ProcessData(unsigned char* data, int width, int height, int bpp)
	{
		m_num = 0;
		faceProcess(m_module, width, height, bpp, data, m_faces, &m_num);
	}

	virtual void DrawResult(unsigned char* data, int width, int height, int bpp)
	{

		CvSize s;
		s.width = width;
		s.height = height;
		IplImage* img = cvCreateImageHeader(s, IPL_DEPTH_8U, 3);
		img->imageData = (char*)data;

		// 
		int w = (int)(m_scale*24 * (double)DISPLAY_WIDTH / (double)this->m_width);
		CvRect rr = cvRect(10, 10, w, w);
		CvPoint p1, p2;
		p1.x = rr.x;
		p1.y = rr.y;
		p2.x = rr.x + rr.width;
		p2.y = rr.y + rr.height;
		cvRectangle(img, p1, p2, CV_RGB(0, 0, 255), 2);
		/*
		rr = cvRect(10, 10, 5*w, 5*w);
		p1.x = rr.x;
		p1.y = rr.y;
		p2.x = rr.x + rr.width;
		p2.y = rr.y + rr.height;
		cvRectangle(img, p1, p2, CV_RGB(0, 255, 0), 2);
		*/
		if (m_num > 0)
		{
			CvSize s;
			s.width = width;
			s.height = height;
			IplImage* img = cvCreateImageHeader(s, IPL_DEPTH_8U, 3);
			img->imageData = (char*)data;
			if (m_num > 0)
				printf("MODULE FACE : Face detected. count = %i \n", m_num);
			// draw result 
			for (int i = 0; i < m_num; i++)
			{
				printf("racurs = %i\n",  m_faces[i].racurs); 
				CvRect rr = cvRect(m_faces[i].XPos, m_faces[i].YPos, m_faces[i].Width, m_faces[i].Height);
				CvPoint p1, p2;
				p1.x = rr.x;
				p1.y = rr.y;
				p2.x = rr.x + rr.width;
				p2.y = rr.y + rr.height;
				cvRectangle(img, p1, p2, CV_RGB(0, 255, 0), 1);
			}
		}
		cvReleaseImageHeader(&img);
	}
};


IVideoAnalysis* VideoAnalysisFactory(TVAInitParams* params, int VA_MODULE_ID)
{
	if (VA_MODULE_ID == VA_MODULE_MOTION)
		return new CMotionModule(params);
	else if (VA_MODULE_ID == VA_MODULE_SABOTAGE)
		return new CSabotageModule(params);
	else if (VA_MODULE_ID == VA_MODULE_FIRE)
		return new CFireModule(params);
	else if (VA_MODULE_ID == VA_MODULE_SMOKE)
		return new CSmokeModule(params);
	else if (VA_MODULE_ID == VA_MODULE_CROWD)
		return new CCrowdModule(params);
	else if (VA_MODULE_ID == VA_MODULE_TRACK)
		return new CTrackModule(params);
	else if (VA_MODULE_ID == VA_MODULE_COUNTER)
		return new CCounterModule(params);
	else if (VA_MODULE_ID == VA_MODULE_PACKAGE)
		return new CPackageModule(params);
	else if (VA_MODULE_ID == VA_MODULE_FACE)
		return new CFaceModule(params);
	else
		return NULL;
}

IVideoAnalysis* module = NULL;


int main(int argc, char** argv)
{
	printf("OpenCV = %s\n", CV_VERSION);

	if (argc < 3)
	{
		Usage();
		return 0;
	}
	
	int k = GetModuleCode(argv[1]);
	if (k < 0)
	{
		printf("ERROR: unknown module. \n");
		printf("---------------------- \n");
		Usage();
		return 0;
	}

	TVAInitParams params;
	params.EventSens 		= 0.79;
	params.EventTimeSens 	= 500;
	params.minHeight = 1;
	params.minWidth = 1;
	params.maxWidth = 30;
	params.maxHeight = 30;

	
	module = VideoAnalysisFactory(&params, k);
	if (module == NULL)
	{
		printf("ERROR: Cannot create video analysis: %s\n", argv[1]);
		Usage();
		return 0;
	}
	

	capture = cvCaptureFromFile(argv[2]);
	if (capture == NULL)
	{
		printf("ERROR: Cannot open video source: %s\n", argv[2]);
		return 0;
	}

	int threadReturnValue;
	mutex =  SDL_CreateMutex();
	thread = SDL_CreateThread(ThreadCapture, "ThreadCapture", (void *)NULL);
	
	IplImage* img = NULL;
	char msg[64];
	sprintf(msg, "symptom 3.0 %s", argv[1]);	
	module->InitModule(&params);
	gquit = false;
	printf("start processing\n");
	int c = 0;
	while (true)
	{
		if (gquit == true)
			break;
		IplImage* frame = NULL;
		if (gframe == NULL)
		{
			printf("continue\n");
			continue;
		}

		SDL_LockMutex(mutex);
		frame = (IplImage*)cvClone(gframe);
		gframe = NULL;
		SDL_UnlockMutex(mutex);
		if (!frame)
			break;
	
		if (img == NULL)
		{
			int height = frame->height * DISPLAY_WIDTH / frame->width;
			CvSize s;
			s.width = DISPLAY_WIDTH;
			s.height = height;
			img = cvCreateImage(s, IPL_DEPTH_8U, 3);
		}
		CPerfomanceCounter perfomance;
		cvResize(frame, img);
		module->ProcessData((unsigned char*)img->imageData,img->width, img->height, 3);
		module->DrawResult((unsigned char*)img->imageData, img->width, img->height, 3);
		cvShowImage(msg, img);
		cvReleaseImage(&frame);
		double ff = perfomance.GetCounter();
		printf("frame #%i\t%lf fps\t t= %lf\n", c++, 1000.f / ff, ff);
		int c;
		c = cvWaitKey(10);
		if ((char)c == 27)
			gquit = true;
	}
	printf("finish processing\n");
	SDL_WaitThread(thread, &threadReturnValue);

	cvReleaseImage(&img);
	cvDestroyAllWindows();
	cvReleaseCapture(&capture);
	module->ReleaseModule();
	SDL_DestroyMutex(mutex);
	return 0;
}
