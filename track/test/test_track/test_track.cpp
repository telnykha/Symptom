// test_track.cpp: определ€ет точку входа дл€ консольного приложени€.
////////////////////
#define  TIXML_USE_STL
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include "track.h"
#include "vautils.h"
#include "_LF.h" 

const char _MODULE_[] = "test_track.";
const int  cNumObjects = 10000;

const int  c_fgWidth	= 256;
const int  c_fgHeight	= 192;

string str_source_db;
HANDLE track = NULL;
CvCapture* capture = NULL;
TVAResult result;
TVAInitParams* params = NULL;
int  num_frames;
int  frames;
DWORD dwTime;
bool g_pressed = false;
bool m_pressed = true;
bool p_pressed = false;

CvScalar black = CV_RGB(0, 0, 0);
CvScalar white = CV_RGB(255, 255, 255);
CvScalar red = CV_RGB(255, 0, 0);

const awpColor c_colors[9] = { { 0, 0, 0 }, { 128, 128, 128 }, { 0, 0, 255 }, { 0, 255, 0 }, { 255, 0, 0 }, { 0, 255, 255 }, { 255, 0, 255 }, { 255, 255, 0 }, { 255, 255, 255 } };
std::map<string, CvScalar> gColors;

extern "C"
{
	#pragma comment(lib, "opencv_world300.lib")
}

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
		else
		{
		
			for (int j = 1; j < params->Zones[i].NumPoints; j++)
			{
				CvPoint p1 = cvPoint(int(w*params->Zones[i].Points[j-1].X / 100.), int(h*params->Zones[i].Points[j-1].Y / 100.));
				CvPoint p2 = cvPoint(int(w*params->Zones[i].Points[j].X / 100.), int(h*params->Zones[i].Points[j].Y / 100.));
				cvLine(si, p1, p2, CV_RGB(255, 0, 0));
			}
			CvPoint p1 = cvPoint(int(w*params->Zones[i].Points[0].X / 100.), int(h*params->Zones[i].Points[0].Y / 100.));
			CvPoint p2 = cvPoint(int(w*params->Zones[i].Points[params->Zones[i].NumPoints - 1].X / 100.), int(h*params->Zones[i].Points[params->Zones[i].NumPoints - 1].Y / 100.));
			cvLine(si, p1, p2, CV_RGB(255, 0, 0));

		}
	}
}
// draw result 
void ProcessResult(TVAResult& result, IplImage* si)
{
	TVATrajectories trajectories;
	if (result.Num > 0)
	{
		
		trajectories.Num = 0;
		trajectories.Trajectories = NULL;
		trackTrajectories(track, &trajectories);
#ifdef _DEBUG
		printf("trajectories.num = %i\n", trajectories.Num);
#endif 
		for (int i = 0; i < result.Num; i++)
		{

			CvScalar color = CV_RGB(200,200,200);
			CvPoint p1, p2, c0;
			std::string str_uuid = LFGUIDToString(&result.blobs[i].id);

			p1.x = result.blobs[i].XPos;
			p1.y = result.blobs[i].YPos;

			p2.x = result.blobs[i].XPos + result.blobs[i].Width;
			p2.y = result.blobs[i].YPos + result.blobs[i].Height;

			c0.x = (p1.x + p2.x) / 2;
			c0.y = (p1.y + p2.y) / 2;
			
			if (result.blobs[i].status == 1)
			{
				color = CV_RGB(rand() % 155 + 100, rand() % 155 + 100, rand() % 155 + 100);
				printf("Object detected. id = %s\n", str_uuid.c_str());
				gColors.insert(std::pair<string, CvScalar>(str_uuid, color));
			}
			else if (result.blobs[i].status == 2)
			{
				std::map<string, CvScalar>::iterator it0 = gColors.begin();
				
				it0 = gColors.find(str_uuid);
				color = it0->second;
			}
			else if (result.blobs[i].status == 3)
			{
				printf("Object lost. id = %s\n", str_uuid.c_str());
				color = CV_RGB(128, 128, 128);
				std::map<string, CvScalar>::iterator it0 = gColors.begin();
				it0 = gColors.find(str_uuid);
				if (it0 != gColors.end())
					gColors.erase(it0);
			}
			else
				continue;
			// draw trajectories 
			for (int i = 0; i < trajectories.Num; i++)
			{
				TVATrajectory v = trajectories.Trajectories[i];
				if (v.Num > 0)
				{
					for (int j = 1; j < v.Num; j++)
					{
						TVABlob b1 = v.blobs[j-1];
						TVABlob b2 = v.blobs[j];

						CvPoint pp1 = cvPoint(b1.XPos + b1.Width / 2, b1.YPos + b1.Height / 2);
						CvPoint pp2 = cvPoint(b2.XPos + b2.Width / 2, b2.YPos + b2.Height / 2);
						cvLine(si, pp1, pp2, color, 3);
					}
				}

				// check events 
				bool res = false;
				if (trackCheckZone(track, &params->Zones[1], &v, &res) == S_OK)
				{
//					if (res)
//						printf("trajectory in zone!\n");
				}
				res = false;
				int dir = 0;
				TVAPoint p1 = params->Zones[2].Points[0];
				TVAPoint p2 = params->Zones[2].Points[1];
				if (trackCheckLine(track, p1, p2, &v, &res, &dir) == S_OK)
				{
					if (res)
						printf("trajectory cross line!\n");
				}
			}
			cvRectangle(si, p1, p2, color, 3);

		}
	}
}


void DrawStatus(IplImage* si)
{
	if (m_pressed)
	{
		cvRectangle(si, cvPoint(0, 0), cvPoint(si->width, 50), black, CV_FILLED, 8, 0);
		CvFont font;
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 1, 8);
		char string1[128];
		sprintf_s(string1, "#frame %d of %d  NUM = %d", frames, num_frames, result.Num);
		cvPutText(si, string1, cvPoint(250, 15), &font, white);
		sprintf(string1, "time = %d ms", dwTime);
		cvPutText(si, string1, cvPoint(10, 35), &font, white);

		if (num_frames > 1)
			cvRectangle(si, cvPoint(10, 6), cvPoint(10 + 230 * (double)frames / (double)num_frames, 20), red, CV_FILLED, 8, 0);
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

		trackForeground(track, si->width, si->height, (unsigned char*)fg->imageData);

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
		trackForeground(track, fg->width, fg->height, (unsigned char*)fg->imageData);

		unsigned char* b = (unsigned char*)si->imageData;
		unsigned char* b1 = (unsigned char*)fg->imageData;


		for (int i = 0; i < c_fgHeight; i++)
		{
			for (int j = 0; j < c_fgWidth; j++)
			{
				b[(si->height - c_fgHeight + i)*si->widthStep + 3 * (si->width - c_fgWidth + j)] = c_colors[b1[i*fg->widthStep + j]].bRed;
				b[(si->height - c_fgHeight + i)*si->widthStep + 3 * (si->width - c_fgWidth + j) + 1] = c_colors[b1[i*fg->widthStep + j]].bGreen;
				b[(si->height - c_fgHeight + i)*si->widthStep + 3 * (si->width - c_fgWidth + j) + 2] = c_colors[b1[i*fg->widthStep + j]].bBlue;
			}
		}
		cvReleaseImage(&fg);
	}
}

int process_video(const char* lpFileName, int mode)
{

	std::string strSourceFile = lpFileName;
	std::string strInitFileName = LFChangeFileExt(strSourceFile, ".xml");
	if (!LoadInitParams(strInitFileName.c_str(), &params))
	{
		printf("Cannot load params.\n");
		return -1;
	}
	params->numObects = 10;
#ifdef _DEBUG
//	printf("TVAInitParam.numObects = %i\n", params.numObects);
#endif
	capture = cvCaptureFromFile(lpFileName);
	CvVideoWriter* writer = NULL;
	result.Num = params->numObects;
	result.blobs = (TVABlob*)malloc(result.Num*sizeof(TVABlob));


	if (capture == NULL)
	{
		printf("%s\n", "Cannot open video source.");
		return -1;
	}

	track = trackCreate(params);
	if (track == NULL)
	{
		printf("Cannot create package processor.  Exit. \n");
		return -1;
	}
	if (mode == 0)
		num_frames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	else
		num_frames = 1;

	double _img_width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	double _img_height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	double _alfa = 640. / _img_width;
	double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	frames = 0;
	IplImage* si = cvCreateImage(cvSize(640, floor(_alfa*_img_height + 0.5)), IPL_DEPTH_8U, 3);
	writer = cvCreateVideoWriter("out.avi", CV_FOURCC('D', 'I', 'V', 'X'), fps, cvSize(640, floor(_alfa*_img_height + 0.5)));
	/*÷икл получени€ и обработки изображени€.
	*/
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
		cvResize(frame, si);

		dwTime = LFGetTickCount();
		trackProcess(track, si->width, si->height, si->nChannels, (unsigned char*)si->imageData, &result);
		dwTime = LFGetTickCount() - dwTime;
		DrawForeground(si);
		ProcessResult(result, si);
		DrawStatus(si);
		DrawZones(si);
		cvShowImage(_MODULE_, si);
		cvWriteFrame(writer, si);
		frames++;
	}

	if (trackRelease(&track) != S_OK)
		printf("error release track.\n");
	cvReleaseImage(&si);
	free(result.blobs);
	cvReleaseCapture(&capture);
	cvReleaseVideoWriter(&writer);
	gColors.clear();
	capture = NULL;
	return 0;
}

int proceess_db()
{
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
				process_video(name.c_str(), 0);
			}
			++count;

		} while (!_findnext(handle, &filesInfo));
	}
	_findclose(handle);
	return 0;
}

int main(int argc, char* argv[])
{
	srand(time(NULL));

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

	TiXmlElement* e = pElem->FirstChildElement("test_track");
	if (e == NULL)
	{
		printf("error: invalid configuration file.\n");
		return 1;
	}
	e = e->FirstChildElement("track_db");
	if (e == NULL)
	{
		printf("error: invalid configuration file.\n");
		return 1;
	}
	str_source_db = e->Attribute("source");
	printf("source data: %s\n", str_source_db.c_str());
	if (argc == 1)
		proceess_db();
	else
	{
		string key = argv[1];
		if (key == "camera" || key == "file")
		{
			if (argc > 2)
			{
				string source_name = argv[2];
				int mode = 0;
				if (key == "camera")
					mode = 1;
				else
					mode = 0;

				process_video(source_name.c_str(), mode);

			}
			else
			{
				printf("Error: invalid arguments.\n");
				return 1;
			}
		}
		else
		{
			printf("Error: invalid arguments.\n");
			return 1;
		}
	}
	return 0;
}

