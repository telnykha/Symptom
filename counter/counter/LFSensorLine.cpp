#include "awpipl.h"
#include "LFSensorLine.h"
//#include "awpLineUtils.h"
CSensorLine::CSensorLine()
{
	m_data = NULL;
	m_num = 0;
	m_line = NULL;
}
CSensorLine::~CSensorLine()
{
	Clear();
}

void CSensorLine::Init(IplImage* img, CBorderLine* line)
{
	if (line == NULL || img == NULL)
	{
		printf("image or line is null.\n");
		return;
	}

	m_line = line;

	awpImage _img;
	_img.nMagic = AWP_MAGIC2;
	_img.bChannels = 1;
	_img.dwType = AWP_FLOAT;
	
	_img.sSizeX = img->width;
	_img.sSizeY = img->height;

	_img.pPixels = img->imageData;
	
	awpPoint p1;
	awpPoint p2;

	p1.X = line->GetSp(img->width, img->height).x;
	p1.Y = line->GetSp(img->width, img->height).y;

	p2.X = line->GetEp(img->width, img->height).x;
	p2.Y = line->GetEp(img->width, img->height).y;

	int length = 0;
	if (awpGetLineDataLenght(&_img, p1, p2, &length) == AWP_OK)
	{
		m_num = length;
		m_data = (double*)malloc(m_num*sizeof(double));
		//printf("lenght = %i\n", m_num);
	}
	else
		printf("CSensorLine::Init error.\n");
	
}

void CSensorLine::Update(IplImage* module, IplImage* angle)
{
	if (module == NULL || angle == NULL)
		return;
	awpImage _img;

	_img.nMagic = AWP_MAGIC2;
	_img.bChannels = 1;
	_img.dwType = AWP_FLOAT;

	_img.sSizeX = module->width;
	_img.sSizeY = module->height;

	_img.pPixels = module->imageData;

	awpPoint p1;
	awpPoint p2;

	p1.X = m_line->GetSp(module->width, module->height).x;
	p1.Y = m_line->GetSp(module->width, module->height).y;

	p2.X = m_line->GetEp(module->width, module->height).x;
	p2.Y = m_line->GetEp(module->width, module->height).y;

	if (awpGetLineData(&_img, p1, p2, m_data) != AWP_OK)
	{
		printf("error obtain data.\n");
	}

	awpImage* __line = NULL;
	awpCreateImage(&__line, m_num, 1, 1, AWP_DOUBLE);
	AWPDOUBLE* d = (AWPDOUBLE*)__line->pPixels;
	memcpy(d, m_data, m_num*sizeof(AWPDOUBLE));

	_img.pPixels = angle->imageData;
	if (awpGetLineData(&_img, p1, p2, m_data) != AWP_OK)
	{
		printf("error obtain data.\n");
	}

	for (int i = 0; i < m_num; i++)
		d[i] *= m_line->Direction(m_data[i]);

	awpResize(__line, 256, 1);
	d = (AWPDOUBLE*)__line->pPixels;
	memcpy(m_data2, d, 256 * sizeof(AWPDOUBLE));
	awpReleaseImage(&__line);
}

int CSensorLine::DataLength()
{
	return m_num;
}
double* CSensorLine::Data()
{
	return m_data2;
}

void CSensorLine::Clear()
{
	if (m_data != NULL)
	{
		free(m_data);
		m_data = NULL;
		m_num = 0;
	}
	m_line = NULL;
}
