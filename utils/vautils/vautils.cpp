// vautils.cpp : Defines the exported functions for the DLL application.
//
#define  TIXML_USE_STL
#include <tinyxml.h>
#include <math.h>
#include "awpipl.h"
#include "vautils.h"

extern "C" VAUTILS_API bool SaveInitParams(const char* lpFileName, TVAInitParams* params)
{
	if (lpFileName == NULL || params == NULL)
		return false;
	
	TiXmlElement* element = new TiXmlElement("VAPARAMS");
	TiXmlElement* e = new TiXmlElement("Camera");
	
	e->SetDoubleAttribute("height", params->Camera.Height);  
	e->SetDoubleAttribute("angle", params->Camera.Angle);
	e->SetDoubleAttribute("focus", params->Camera.Focus);
	e->SetDoubleAttribute("wsize", params->Camera.WChipSize);
	e->SetDoubleAttribute("hsize", params->Camera.HChipSize);

	element->LinkEndChild(e);

	TiXmlElement* e1 = new TiXmlElement("Zones");
	//
	for (int i = 0; i < params->NumZones; i++)
	{
		if (params->Zones[i].IsRect)
		{
			//сохранить прямоугольник
			TiXmlElement* e3 = new TiXmlElement("rect");
			e3->SetDoubleAttribute("x", params->Zones[i].Rect.LeftTop.X);
			e3->SetDoubleAttribute("y", params->Zones[i].Rect.LeftTop.Y);
			e3->SetDoubleAttribute("bx", params->Zones[i].Rect.RightBottom.X);
			e3->SetDoubleAttribute("by", params->Zones[i].Rect.RightBottom.Y);
			e1->LinkEndChild(e3);
		}
		else
		{
			//сохранить контур
			TiXmlElement* e3 = new TiXmlElement("contour");
			for (int j = 0; j < params->Zones[i].NumPoints; j++)
			{
				TiXmlElement* e2 = new TiXmlElement("Point");
				e2->SetDoubleAttribute("x", params->Zones[i].Points[j].X);
				e2->SetDoubleAttribute("y", params->Zones[i].Points[j].Y);
				e3->LinkEndChild(e2);
			}
			e1->LinkEndChild(e3);
		}
	}
	element->LinkEndChild(e1);

		TiXmlElement* e3 = new TiXmlElement("sensitivity");
	e3->SetDoubleAttribute("eventsens", params->EventSens);
	e3->SetDoubleAttribute("eventtimesens", params->EventTimeSens);
	element->LinkEndChild(e3);
	
	TiXmlElement* e4 = new TiXmlElement("objects");
	e4->SetDoubleAttribute("minWidth", params->minWidth);
	e4->SetDoubleAttribute("minHeight", params->minHeight);
	e4->SetDoubleAttribute("maxWidth", params->maxWidth);
	e4->SetDoubleAttribute("maxHeight", params->maxHeight);
	e4->SetAttribute("numObjects", params->numObects);
	e4->SetAttribute("SaveLog", params->SaveLog);
	e4->SetAttribute("Path", params->Path != NULL ? params->Path: "");
	element->LinkEndChild(e4);

    TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );
	doc.LinkEndChild( element );

	return doc.SaveFile(lpFileName);
}

extern "C" VAUTILS_API bool LoadInitParams(const char* lpFileName, TVAInitParams** p)
{
	if (lpFileName == NULL || p == NULL)
		return false;

	*p = new TVAInitParams();
	TVAInitParams* params = *p;

   TiXmlDocument doc(lpFileName);
   if (!doc.LoadFile())
   {
	   delete params;
	   params = NULL;
	   return false;
   }

   TiXmlHandle hDoc(&doc);
   TiXmlElement* pElem = NULL;
   TiXmlHandle hRoot(0);

   pElem = hDoc.FirstChildElement().Element();
   if (pElem == NULL)
   {
	   delete params;
	   params = NULL;
	   return false;
   }

	if (strcmp(pElem->Value(), "VAPARAMS") != 0)
	{
		delete params;
		params = NULL;
		return false;
	}

	// загрузка параметров камеры. 
	TiXmlElement* e = pElem->FirstChildElement();
	if (e == NULL || strcmp(e->Value(), "Camera") != 0)
	{
		delete params;
		params = NULL;
		return false;
	}

	e->QueryDoubleAttribute("angle", &params->Camera.Angle);
	e->QueryDoubleAttribute("height", &params->Camera.Height);
	e->QueryDoubleAttribute("focus", &params->Camera.Focus);
	e->QueryDoubleAttribute("wsize", &params->Camera.WChipSize);
	e->QueryDoubleAttribute("hsize", &params->Camera.HChipSize);

	e = pElem->FirstChildElement("sensitivity");
	if (e == NULL)
	{
		delete params;
		params = NULL;
		return false;
	}

	e->QueryDoubleAttribute("eventsens",&params->EventSens);
	e->QueryDoubleAttribute("eventtimesens", &params->EventTimeSens);



	e = pElem->FirstChildElement("Zones");
	
	if (e == NULL)
	{
		delete params;
		params = NULL;
		return false;
	}

	params->NumZones = 0;
	params->Zones = NULL;
	for (TiXmlNode* child = e->FirstChild(); child; child = child->NextSibling())
	{
		if (strcmp(child->Value(), "contour") == 0)
		{
		   int np = 0;
		   double x,y= 0;
		   params->NumZones += 1;
		   params->Zones = (TVAZone*)realloc(params->Zones, params->NumZones*sizeof(TVAZone));
		   params->Zones[params->NumZones - 1].IsRect = false;
		   params->Zones[params->NumZones - 1].Points = NULL;
		   for (TiXmlNode* child1 = child->FirstChild(); child1; child1 = child1->NextSibling())
		   {		   
			   child1->ToElement()->QueryDoubleAttribute("x",&x);
			   child1->ToElement()->QueryDoubleAttribute("y",&y);
			   np++;
			   params->Zones[params->NumZones - 1].Points = (TVAPoint*)realloc(params->Zones[params->NumZones - 1].Points, np*sizeof(TVAPoint));
			   params->Zones[params->NumZones - 1].NumPoints = np;
			   params->Zones[params->NumZones - 1].Points[np-1].X = (float)x;
			   params->Zones[params->NumZones - 1].Points[np - 1].Y = (float)y;
		   }
		}
		else if (strcmp(child->Value(), "rect") == 0)
		{
		   double x1,y1,x2,y2;
		   params->NumZones += 1;
		   params->Zones = (TVAZone*)realloc(params->Zones, params->NumZones*sizeof(TVAZone));
		   params->Zones[params->NumZones - 1].IsRect = true;

		   child->ToElement()->QueryDoubleAttribute("x",&x1);
		   child->ToElement()->QueryDoubleAttribute("y",&y1);
		   child->ToElement()->QueryDoubleAttribute("bx",&x2);
		   child->ToElement()->QueryDoubleAttribute("by",&y2);
		   params->Zones[params->NumZones - 1].Rect.LeftTop.X = (float)x1;
		   params->Zones[params->NumZones - 1].Rect.LeftTop.Y = (float)y1;
		   params->Zones[params->NumZones - 1].Rect.RightBottom.X = (float)x2;
		   params->Zones[params->NumZones - 1].Rect.RightBottom.Y = (float)y2;
		}
	}

	e = pElem->FirstChildElement("objects");
	if (e == NULL)
	{
		delete params;
		params = NULL;
		return false;
	}
	e->QueryDoubleAttribute("minWidth", &params->minWidth);
	e->QueryDoubleAttribute("minHeight",&params->minHeight);
	e->QueryDoubleAttribute("maxWidth", &params->maxWidth);
	e->QueryDoubleAttribute("maxHeight", &params->maxHeight);
	e->QueryIntAttribute("numObjects", &params->numObects);

	if (e->Attribute("Path"))
	{
		std::string _tmp = e->Attribute("Path");
		params->Path = (char*)malloc(_tmp.length() + 1);
		strcpy(params->Path, _tmp.c_str());

	}
	else
		params->Path = NULL;


   return true;
}

#define AWP_PI 3.14159265358979323846 

static bool IsRectInImage(TVASize* size, TVARect* rect)
{
	if (rect->LeftTop.X >= 0 && rect->RightBottom.X < size->width &&
		rect->LeftTop.Y >= 0 && rect->RightBottom.Y < size->height)
		return true;

	return false;
}

extern "C" VAUTILS_API void FreeParams(TVAInitParams** p)
{
	if (p == NULL)
		return;
	TVAInitParams* params = *p;
	for (int i = 0; i < params->NumZones; i++)
	{
		if (!params->Zones[i].IsRect)
			free(params->Zones[i].Points);
	}
	params->NumZones = 0;
	free(params->Zones);
	params->Zones = NULL;
	if (params->Path != NULL)
	{
		free(params->Path);
		params->Path = NULL;
	}
	delete params; 
	params = NULL;
}


extern "C" VAUTILS_API bool AddZone(TVAInitParams* params, TVAZone* zone)
{
	if (params == NULL || zone == NULL)
		return false;
	
	params->NumZones++;
	params->Zones = (TVAZone*)realloc(params->Zones, params->NumZones*sizeof(TVAZone));

	if (zone->IsRect)
	{
		params->Zones[params->NumZones - 1].IsRect = true;
		params->Zones[params->NumZones - 1].Rect = zone->Rect;
	}
	else
	{
		params->Zones[params->NumZones - 1].IsRect = false;
		params->Zones[params->NumZones - 1].NumPoints = zone->NumPoints;
		params->Zones[params->NumZones - 1].Points = (TVAPoint*)malloc(zone->NumPoints*sizeof(TVAPoint));
		memcpy(params->Zones[params->NumZones - 1].Points, zone->Points, zone->NumPoints*sizeof(TVAPoint));
	}
	return true;
}
extern "C" VAUTILS_API bool DeleteZone(TVAInitParams* params, int idx)
{
	if (params == NULL)
		return false;
	if (idx < 0 || idx >= params->NumZones)
		return false;
	int c = 0;
	for (int i = 0; i < params->NumZones; i++)
	{
		if (i != idx)
		{
			params->Zones[c] = params->Zones[i];
			c++;
		}
		else
		{
			if (!params->Zones[i].IsRect)
				free(params->Zones->Points);
		}
	}
	params->NumZones--;
	return true;
}

extern "C" VAUTILS_API bool CopyParams(TVAInitParams* src, TVAInitParams** dst)
{
	
	if (src == NULL || dst == NULL)
		return false;
	
	*dst = new TVAInitParams();
	TVAInitParams* params = *dst;

	memcpy(params, src, sizeof(TVAInitParams));
	if (src->Path != NULL && (src->Path) > 0)
	{
		params->Path = new char[strlen(src->Path) + 1];
		strcpy(params->Path, src->Path);
	}
	else
		params->Path = NULL;

	if (src->NumZones > 0)
	{
		params->Zones = new TVAZone[src->NumZones];
		for (int i = 0; i < src->NumZones; i++)
		{
			params->Zones[i].IsRect = src->Zones[i].IsRect;
			if (src->Zones[i].IsRect)
			{
				params->Zones[i] = src->Zones[i];				
			}
			else
			{
				params->Zones[i].NumPoints = src->Zones[i].NumPoints;
				params->Zones[i].Points = new TVAPoint[src->Zones[i].NumPoints];
				for (int j = 0; j < params->Zones[i].NumPoints; j++)
				{
					params->Zones[i].Points[j] = src->Zones[i].Points[j];
				}
			}
			params->Zones[i] = src->Zones[i];
		}
		
	}

	return true;
}

VAUTILS_API double Camera_to_aw(TVACamera* pCamera)
{
    if (pCamera == NULL)
        return 0;

	double a =(double) (2.0F * atan((double)(pCamera->WChipSize / (2.0*pCamera->Focus))));
    double Value =  (double)(180 * a / 3.14);

    return Value;
}
// вычисление ah - вертикальный угол зрения 
VAUTILS_API double Camera_to_ah(TVACamera* pCamera)
{
    if (pCamera == NULL)
        return 0;

	double a = 2.0*atan(pCamera->HChipSize / (2.0*pCamera->Focus));
    return 180 * a / AWP_PI;
}
// расстояние до первой видимой точки на сцене. 
VAUTILS_API double Camera_to_d0(TVACamera* pCamera)
{
    if (pCamera == NULL)
        return 0;

    double vv = Camera_to_ah(pCamera);
  	double alfa = AWP_PI*(pCamera->Angle - 0.5*vv) / 180;
	return pCamera->Height*tan(alfa);

}
// предполагается, что точка Y изображения size лежит на предметной плоскости, 
// над которой установлена камера на высоте h
VAUTILS_API double Y_to_d(TVACamera* pCamera, TVASize* size,  int Y)
{
    if (pCamera == NULL)
        return 0;
    if (size == NULL)
        return 0;

	if (Y <= 0 || Y > size->height)
        return 0;


	double delta = pCamera->HChipSize*((size->height -Y)/size->height - 0.5);
	double alfa = AWP_PI*pCamera->Angle/180.;
	return  pCamera->Height * tan(atan(delta/pCamera->Focus) + alfa);
}
// точка предметной плоскости на удалении d от камеры проецируется на изображение в точку Y 
VAUTILS_API double d_to_Y(TVACamera* pCamera, TVASize* size, double d)
{
    if (pCamera == NULL)
        return -1;
    if (size == NULL)
        return -1;

    if (d <= 0 || d > pCamera->dMaxLenght)
    {
         return  -1;
    }


	double Y = 0;
	double b = atan(d / pCamera->Height);
	Y = (size->height / pCamera->HChipSize)*(0.5*pCamera->HChipSize +
		pCamera->Focus* tan(b + 3.14159*pCamera->Angle/180));
    return -Y;
}
// смещение точки x на предметной плоскости относительно оптической оси камеры. 
VAUTILS_API double XY_to_x(TVACamera* pCamera, TVASize* size, TVAPoint* pPoint)
{

    if (pCamera == NULL)
        return -1;
    if (size == NULL)
        return -1;


	if (pPoint->X < 0 || pPoint->X > size->width)
        return -1;
	if (pPoint->Y < 0 || pPoint->Y > size->height)
        return -1;

    double L = Y_to_d(pCamera, size, (int)pPoint->Y);
	return (pPoint->X / size->width - 0.5)*pCamera->WChipSize*L/pCamera->Focus;
}

// расстояние до объекта, который находится на высоте h1 над плоскотью и координаты которого заданы 
// точкой Y на изображении. 
VAUTILS_API double Yh1_to_d(TVACamera* pCamera, TVASize* size,  int Y, int h1)
{
    if (pCamera == NULL)
        return -1;
    if (size == NULL)
        return -1;

    if (Y <= 0 || Y > size->height)
        return -1;

	double delta = pCamera->HChipSize*((double)(size->height -Y)/(double)size->height- 0.5);
	double alfa = AWP_PI*pCamera->Angle/180.;
	return (pCamera->Height - h1) * tan(atan(delta/pCamera->Focus) + alfa);
}

// смещение горизонтальное смещение объекта, поднятого на высоту h на предметной плоскостью отностительно оптической оси камеры. 
VAUTILS_API double XY_to_xh(TVACamera* pCamera, TVASize* size, TVAPoint* pPoint, int  h)
{
    if (pCamera == NULL)
        return -1;
    if (size == NULL)
        return -1;

	if (pPoint->X < 0 || pPoint->X > size->width)
        return -1;
    if (pPoint->Y < 0 || pPoint->Y > size->height)
        return -1;
	
    double L = Yh1_to_d(pCamera, size, (int)pPoint->Y, h);
	return ((double)pPoint->X / (double)size->width- 0.5)*pCamera->WChipSize*L/pCamera->Focus;
}
//расстояние до точки на предметной плоскоти, которая задана координатами XY
VAUTILS_API double XY_to_d(TVACamera* pCamera, TVASize* size, TVAPoint* pPoint)
{
    if (pCamera == NULL)
        return -1;
    if (size == NULL)
        return -1;
    if (pPoint == NULL)
         return -1;

	if (pPoint->X < 0 || pPoint->X > size->width)
        return -1;
	if (pPoint->Y < 0 || pPoint->Y > size->height)
        return -1;

    double L = Y_to_d(pCamera, size, (int)pPoint->Y);
    double S = XY_to_x(pCamera, size, pPoint);

    return sqrt(L*L + S*S);
}
// размеры объектов на сцене
VAUTILS_API double WH_to_w(TVACamera* pCamera, TVASize* size, TVARect* rect)
{
    if (pCamera == NULL)
        return -1;
    if (size == NULL)
        return -1;
    if (rect == NULL)
         return -1;

	if (!IsRectInImage(size, rect))
		return -1;

	double L = Y_to_d(pCamera, size, (int)(rect->RightBottom.Y + 0.5));
	return fabs(L*pCamera->WChipSize*(rect->RightBottom.X - rect->LeftTop.X)/(pCamera->Focus*size->width));
}
VAUTILS_API double WH_to_h(TVACamera* pCamera, TVASize* size, TVARect* rect)
{
    if (pCamera == NULL)
        return -1;
    if (size == NULL)
        return -1;
    if (rect == NULL)
         return -1;

	if (!IsRectInImage(size, rect))
		return -1;

	double Z = Y_to_d(pCamera, size, (int)(rect->RightBottom.Y + 0.5));

	double delta = pCamera->HChipSize*((size->height - rect->LeftTop.Y )/size->height - 0.5);
	double gamma = atan(delta / pCamera->Focus);
	double tan_alfa = tan(gamma + AWP_PI*pCamera->Angle/180.);
	return pCamera->Height - Z / tan_alfa;

}
//вычисляет ширину объекта, поднятого над предметной плоскостью на высоту h1 
VAUTILS_API double WHh1_to_w(TVACamera* pCamera, TVASize* size, TVARect* rect, int h1)
{
    if (pCamera == NULL)
        return -1;
    if (size == NULL)
        return -1;
    if (rect == NULL)
         return -1;

	if (!IsRectInImage(size, rect))
		return -1;

	double L = Yh1_to_d(pCamera, size, rect->LeftTop.Y, h1);
	return 2*fabs(L*pCamera->WChipSize*(rect->RightBottom.X - rect->LeftTop.X)/(pCamera->Focus*size->width));
}
// преобразование 3d координаты
VAUTILS_API void Scene_To_ImagePoint(TVACamera* pCamera, TVASize* size, TVA3DPoint* pSrc, TVAPoint* pDst)
{
    if (pCamera == NULL)
        return;
    if (size == NULL)
        return;
    if (pSrc == NULL)
        return;
    if (pDst == NULL)
        return;

	pDst->X = size->width*(pSrc->X*pCamera->Focus / (pCamera->WChipSize*pSrc->Z) + 0.5f);
	double gamma = atan(pSrc->Z /(pCamera->Height - pSrc->Y));
	pDst->Y = size->height*(pCamera->Focus*tan(AWP_PI*pCamera->Angle /180 - gamma)/pCamera->HChipSize + 0.5);
}
VAUTILS_API void Image_To_ScenePoint(TVACamera* pCamera, TVASize* size, double Height,  TVAPoint* pSrc, TVA3DPoint* pDst)
{
    if (pCamera == NULL)
        return ;
    if (size == NULL)
        return ;
    if (pSrc == NULL)
        return ;
    if (pDst == NULL)
        return ;

    double X, Y, Z;

    Z = Y_to_d(pCamera, size, pSrc->Y);
    TVAPoint p;
    p.X = pSrc->X;
    p.Y = pSrc->Y;

    X = XY_to_x(pCamera, size, &p);
    
	TVARect r;
	r.LeftTop.X  =  pSrc->X;
	r.RightBottom.X =  r.LeftTop.X + 1;
	r.LeftTop.Y   =  pSrc->Y;
	r.RightBottom.Y = r.LeftTop.Y   + Height;

    Y = WH_to_h(pCamera, size, &r);

    pDst->X = X;
    pDst->Y = Y;
    pDst->Z = Z;

}

static bool IsPointInContour(int x, int y, TVAZone* contour)

{

	int count = 0;

	for (int i = 0; i < contour->NumPoints; i++)

	{
		int j = (i + 1) % contour->NumPoints;
		if ((int)contour->Points[i].Y == (int)contour->Points[j].Y)
			continue;
		else if (contour->Points[i].Y > y && contour->Points[j].Y > y)
			continue;
		else if (contour->Points[i].Y < y && contour->Points[j].Y < y)
			continue;
		else if ((int)AWP_MIN(contour->Points[i].X, contour->Points[j].X) > x)
			continue;
		else if ((int)AWP_MIN(contour->Points[i].Y, contour->Points[j].Y) == (int)y)
			continue;
		else if ((int)AWP_MAX(contour->Points[i].Y, contour->Points[j].Y) == (int)y)
		{
			double x1 = contour->Points[i].Y > contour->Points[j].Y ? contour->Points[i].X : contour->Points[j].X;
			if (x > x1)
				count++;
		}
		else if (contour->Points[i].X == contour->Points[j].X)
		{
			count++;
		}
		else
		{
			double k, b;
			k = (contour->Points[j].Y - contour->Points[i].Y) / (contour->Points[j].X - contour->Points[i].X);
			b = contour->Points[j].Y - k*contour->Points[j].X;
			double t;
			t = (y - b) / k;
			if (t < x)
				count++;
		}
	}
	return count & 1;

}

VAUTILS_API bool IsPointInZone(int x, int y, TVAInitParams* params)

{
	for (int i = 0; i < params->NumZones; i++)

	{

		if (params->Zones[i].IsRect)
		{

			if (x > params->Zones[i].Rect.LeftTop.X && x < params->Zones[i].Rect.RightBottom.X &&

				y > params->Zones[i].Rect.LeftTop.Y && y < params->Zones[i].Rect.RightBottom.Y)

				return true;
		}
		else
		{
			if (IsPointInContour(x, y, &params->Zones[i]))
				return true;
		}
	}

	return false;

}

VAUTILS_API bool ApplyZonesToImage(HANDLE image, TVAInitParams* params)
{
	if (image == NULL)
		return false;
	awpImage* img = (awpImage*)image;
	if (awpCheckImage(img) != AWP_OK)
		return false;
	for (int y = 0; y < img->sSizeY; y++)
	{
		int _y = (int)floor(100 * (double)y / (double)img->sSizeY + 0.5);
		for (int x = 0; x < img->sSizeX; x++)
		{
			int _x = (int)floor(100 * (double)x / (double)img->sSizeX + 0.5);
			if (!IsPointInZone(_x, _y, params))
			{
				AWPBYTE* pixels = (AWPBYTE*)img->pPixels;
				pixels += (y*img->sSizeX + x)*img->bChannels;
				for (int i = 0; i < img->bChannels; i++)
				{
					pixels[i] = 0;
				}
			}

		}
	}
	return true;
}

VAUTILS_API bool MakeMaskImage(HANDLE image, TVAInitParams* params)
{
	if (image == NULL || params == NULL)
		return false;
	awpImage* img = (awpImage*)image;
	if (awpCheckImage(img) != AWP_OK)
		return false;
	if (params->NumZones == 0)
	{

		awpFill(img, 1);
		return true;
	}


	for (int y = 0; y < img->sSizeY; y++)
	{
		int _y = (int)floor(100 * (double)y / (double)img->sSizeY + 0.5);
		for (int x = 0; x < img->sSizeX; x++)
		{
			int _x = (int)floor(100 * (double)x / (double)img->sSizeX + 0.5);
			if (!IsPointInZone(_x, _y, params))
			{
				AWPBYTE* pixels = (AWPBYTE*)img->pPixels;
				pixels += (y*img->sSizeX + x)*img->bChannels;
				for (int i = 0; i < img->bChannels; i++)
				{
					pixels[i] = 0;
				}
			}
			else
			{
				AWPBYTE* pixels = (AWPBYTE*)img->pPixels;
				pixels += (y*img->sSizeX + x)*img->bChannels;
				for (int i = 0; i < img->bChannels; i++)
				{
					pixels[i] = 1;
				}
			}

		}
	}
	return true;
}
