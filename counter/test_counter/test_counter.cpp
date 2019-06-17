// test_counter.cpp: определ€ет точку входа дл€ консольного приложени€.
//
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include "counter.h"
#include "vautils.h"
#include "_LF.h" 
#include "..\..\common\event_log.h"

#define _CRT_SECURE_NO_WARNINGS

const char _MODULE_[] = "test_counter.";
extern "C"
{
	#pragma comment(lib, "opencv_world300.lib")
}

const int  c_fgWidth = 256;
const int  c_fgHeight = 192;

CvScalar black = CV_RGB(0, 0, 0);
CvScalar white = CV_RGB(255, 255, 255);
CvScalar red = CV_RGB(255, 0, 0);
CvScalar gray = CV_RGB(128, 128, 128);
CvScalar green = CV_RGB(0, 255, 0);
CvScalar yellow = CV_RGB(255, 255, 0);

CvCapture* capture = NULL;
string str_source_db;
TVAInitParams* params = NULL;
HANDLE counter = NULL;
int num_frames;
int frames;
bool g_pressed = false;
bool m_pressed = true;
bool p_pressed = false;
bool result = false;

double g_avalue;
double g_hvalue;
double g_aintegr;
double g_hintegr;

TLFBuffer g_abuffer(256, 0);
TLFBuffer g_hbuffer(256, 0);
TLFBuffer g_sbuffer(256, 0);

CEventLog* gevents_log;

void DrawZones(IplImage* si)
{
	int w = si->width;
	int h = si->height;

	for (int i = 0; i < params->NumZones; i++)
	{
		if (params->Zones[i].IsRect)
		{
			CvRect rect = cvRect(int(w*params->Zones[i].Rect.LeftTop.X / 100.), int(h*params->Zones[i].Rect.LeftTop.Y / 100.), 
				int(w*(params->Zones[i].Rect.RightBottom.X - params->Zones[i].Rect.LeftTop.X) / 100.), 
				int(h*(params->Zones[i].Rect.RightBottom.Y - params->Zones[i].Rect.LeftTop.Y) / 100.));
			CvPoint p1 = cvPoint(rect.x, rect.y);
			CvPoint p2 = cvPoint(p1.x + rect.width, p1.y + rect.height);
			cvRectangle(si, p1, p2, CV_RGB(255, 0, 0));
		}
	}
}

void DrawStatus(IplImage* si)
{
	if (m_pressed)
	{
		int num_in = 0;
		int num_out = 0;
		tcounterGetCount(counter, &num_in, &num_out);
		cvRectangle(si, cvPoint(0, 0), cvPoint(si->width, 50), black, CV_FILLED, 8, 0);
		CvFont font;
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 1, 8);
		char string1[256];

		sprintf_s(string1, "#frame %d of %d", frames, num_frames);
		cvPutText(si, string1, cvPoint(250, 15), &font, white);

		sprintf_s(string1, "num_in = %d num_out = %d avalue = %f hvalue = %f", num_in, num_out, g_avalue, g_hvalue);
		cvPutText(si, string1, cvPoint(10, 35), &font, white);

		if (num_frames > 1)
			cvRectangle(si, cvPoint(10, 6), cvPoint(10 + int (230 * (double)frames / (double)num_frames), 20), red, CV_FILLED, 8, 0);

		cvRectangle(si, cvPoint(10, si->height - 64), cvPoint(256 + 10, si->height - 10), gray);
		for (int i = 1; i < 256; i++)
		{
			cvLine(si, cvPoint(10 + i -1, si->height - 10 - 54 * g_abuffer.GetValue(i - 1) / 100), cvPoint(10+ i, si->height - 10 - 54 * g_abuffer.GetValue(i) / 100), red);
			cvLine(si, cvPoint(10 + i - 1, si->height - 10 - 54 * g_sbuffer.GetValue(i - 1) / 100), cvPoint(10 + i, si->height - 10 - 54 * g_sbuffer.GetValue(i) / 100), yellow);
			cvLine(si, cvPoint(10 + i - 1, si->height - 10 - 54 * g_hbuffer.GetValue(i - 1) / 100), cvPoint(10 + i, si->height - 10 - 54 * g_hbuffer.GetValue(i) / 100), green);
		}

		TVAPoint sp;
		TVAPoint ep;
		bool has_t = false;
		memset(&ep, 0, sizeof(TVAPoint));
		memset(&sp, 0, sizeof(TVAPoint));
		tcounterGetLastTrajectory(counter, &sp, &ep, &has_t);
		if (has_t)
		{
			CvScalar color = green;
			if (ep.Y < sp.Y)
				color = yellow;
			cvLine(si, cvPoint(sp.X, sp.Y), cvPoint(ep.X, ep.Y), color);
		}

	}
}

void DrawForeground(IplImage* si)
{
	if (!g_pressed)
	{
		CvSize s;
		s.width = si->width;
		s.height = si->height;
		IplImage* fg = cvCreateImage(s, IPL_DEPTH_8U, 1);

		tcounterGetForeground(counter, si->width, si->height, (unsigned char*)fg->imageData);

		unsigned char* b = (unsigned char*)si->imageData;
		unsigned char* b1 = (unsigned char*)fg->imageData;

		for (int y = 0; y < si->height; y++)
		{
			for (int x = 0; x < si->width; x++)
			{
				if (b1[x + y*fg->widthStep] == 0)
				{
					b[3 * x + y*si->widthStep] /= 2;
					b[3 * x + 1 + y*si->widthStep] /= 2;
					b[3 * x + 2 + y*si->widthStep] /= 2;
				}
			}
		}
		cvReleaseImage(&fg);
	}
	else
	{
		CvSize s;
		s.width = c_fgWidth;
		s.height = c_fgHeight;
		IplImage* fg = cvCreateImage(s, IPL_DEPTH_8U, 1);
		tcounterGetForeground(counter, fg->width, fg->height, (unsigned char*)fg->imageData);

		unsigned char* b = (unsigned char*)si->imageData;
		unsigned char* b1 = (unsigned char*)fg->imageData;


		for (int i = 0; i < c_fgHeight; i++)
		{
			for (int j = 0; j < c_fgWidth; j++)
			{
				if (b1[j + i*fg->widthStep] != 0)
				{
					b[(si->height - c_fgHeight + i)*si->widthStep + 3 * (si->width - c_fgWidth + j)] = 255;
					b[(si->height - c_fgHeight + i)*si->widthStep + 3 * (si->width - c_fgWidth + j) + 1] = 0;
					b[(si->height - c_fgHeight + i)*si->widthStep + 3 * (si->width - c_fgWidth + j) + 2] = 0;
				}
				else
				{
					b[(si->height - c_fgHeight + i)*si->widthStep + 3 * (si->width - c_fgWidth + j)] = 0;
					b[(si->height - c_fgHeight + i)*si->widthStep + 3 * (si->width - c_fgWidth + j) + 1] = 0;
					b[(si->height - c_fgHeight + i)*si->widthStep + 3 * (si->width - c_fgWidth + j) + 2] = 0;
				}
			}
		}
		cvReleaseImage(&fg);
	}
}
int process_video(const char* lpFileName)
{

	std::string strSourceFile = lpFileName;
	std::string strInitFileName = LFChangeFileExt(strSourceFile, ".xml");
	std::string strEventsName = LFChangeFileExt(strSourceFile, ".events");
	
	// try to load params 
	if (!LoadInitParams(strInitFileName.c_str(), &params))
	{
		printf("Cannot load params.\n");
		return -1;
	}

	capture = cvCaptureFromFile(lpFileName);

	if (capture == NULL)
	{
		printf("%s\n", "Cannot open video source.");
		return -1;
	}
	TVAPoint start;
	TVAPoint finish;
	TVARect  sizes;
	start   = params->Zones[0].Rect.LeftTop;
	finish  = params->Zones[0].Rect.RightBottom;
	sizes = params->Zones[1].Rect;

	counter = tcounterCreateEx(start, finish, sizes, params->EventSens, SINGLE_DOOR_COUNTER);
	if (counter == NULL)
	{
		printf("Cannot create counter processor.  Exit. \n");
		return -1;
	}

	num_frames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);

	double _img_width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	double _img_height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	double _alfa = 640. / _img_width;
	double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	frames = 0;
	IplImage* si = cvCreateImage(cvSize(640, floor(_alfa*_img_height + 0.5)), IPL_DEPTH_8U, 3);

	CEventLog events_log(strEventsName.c_str());

	/*÷икл получени€ и обработки изображени€.
	*/
	FILE* log = NULL; fopen_s(&log, "counter.txt", "a");
	bool event1 = false;
	int start_frame = 0;
	for (;;)
	{

		int c;
		c = cvWaitKey(10);
		if ((char)c == 27)
			break;
		if ((char)c == 'g' || (char(c) == 'G'))
			g_pressed = !g_pressed;
		if ((char)c == 'm' || (char(c) == 'M'))
			m_pressed = !m_pressed;
		if ((char)c == 'p' || (char(c) == 'P'))
			p_pressed = !p_pressed;
		if (p_pressed)
			continue;


		IplImage* frame = NULL;
		frame = cvQueryFrame(capture);
		if (!frame)
			break;
		frames++;
		cvResize(frame, si);
		int state = 0;
		double value;
		if (tcounterProcess(counter, si->width, si->height, si->nChannels, (unsigned char*)si->imageData, value) != S_OK)
			break;
		if (tcounterGetState(counter, g_avalue, g_hvalue, state))
			break;
		if (state == 1 && !event1)
		{
			event1 = true;
			start_frame = frames;
			g_aintegr = 0;
			g_hintegr = 0;
		}
		if (event1)
		{
			g_aintegr += g_avalue;
			g_hintegr += g_hvalue;
		}
		if (event1 && state == 0)
		{
			event1 = false;
			events_log.AddEvent(start_frame, frames - start_frame, g_aintegr, g_hintegr);
			gevents_log->AddEvent(start_frame, frames - start_frame, g_aintegr, g_hintegr);
		}

		g_abuffer.Push(g_avalue);
		g_hbuffer.Push(g_hvalue);
		g_sbuffer.Push((double)state*100.);


		DrawForeground(si);
		DrawStatus(si);
		DrawZones(si);
		cvShowImage(_MODULE_, si);
	}

	int in, out;
	tcounterGetCount(counter, &in, &out);
	fprintf(log, "%s\t %d\t %d\t %d\n", lpFileName, frames, out, in);
	fclose(log);

	tcounterRelease(&counter);
	cvReleaseImage(&si);
	cvReleaseCapture(&capture);
	capture = NULL;
	return 0;

}


int process_video2(const char* lpFileName)
{
	return 0;
}


int process_db()
{
	FILE* log = NULL; fopen_s(&log, "counter.txt", "w+t");
	fclose(log);
	gevents_log = new CEventLog("couneter.events");
	_finddata_t filesInfo;
	long handle = 0;
	int count = 0;
	if ((handle = _findfirst((char*)((str_source_db + "*.*").c_str()), &filesInfo)) != -1)
	{
		do
		{
			string name = str_source_db + filesInfo.name;
			string ext = LFGetFileExt(name);
			if (ext == ".avi" || ext == ".mp4" || ext == ".mkv")
			{
				printf("%s\n", name.c_str());
				process_video(name.c_str());
			}
			++count;

		} while (!_findnext(handle, &filesInfo));
	}
	delete gevents_log;
	_findclose(handle);
	return 0;
}

int main(int argc, char* argv[])
{
	TiXmlDocument doc;
	if (!doc.LoadFile("init.xml"))
	{
		printf("Cannot load configuration file.\n");
		return 1;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem = NULL;
	pElem = hDoc.FirstChildElement().Element();
	if (!pElem)
	{
		printf("error: invalid configuration file.\n");
		return 1;
	}

	TiXmlElement* e = pElem->FirstChild("test_counter")->ToElement();
	if (e == NULL)
	{
		printf("error: invalid configuration file.\n");
		return 1;
	}
	e = e->FirstChild("counter_db")->ToElement();
	str_source_db = e->Attribute("source");
	printf("source data: %s\n", str_source_db.c_str());

	if (argc < 2)
		process_db();
	else
		process_video(argv[1]);

	return 0;
}