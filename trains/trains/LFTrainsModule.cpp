//---------------------------------------------------------------------------
#pragma hdrstop
#include "LFTrainsModule.h"
#define MODEL_WIDTH		32
#define MODEL_HEIGHT	32


//---------------------------------------------------------------------------
TLFTrains::TLFTrains(TLFZones& zones)
{
   m_zones = zones;
	for (int i = 0; i < 10; i++)
	{
		TLFDetectEngine* e = new TLFDetectEngine();
		m_digits.Add(e);
	}

	m_handle = awpflowCreate();
	m_pBox = NULL;
}
TLFTrains::~TLFTrains()
{
	awpflowRelease(m_handle);
	if (m_pBox != NULL)
		delete m_pBox;
}

void TLFTrains::TrackBox(awpImage* currentImage)
{
	if (m_prevImage.GetImage() == NULL)
		m_prevImage.SetImage(currentImage);
	else
	{
		awpRect rect;
		if (awpflowTrack(m_handle, m_prevImage.GetImage(), currentImage, m_pBox, &rect) == S_OK)
		{
			if (this->CheckBoxScale(rect))
			{
				*m_pBox = rect;
				 m_prevImage.SetImage(currentImage);
			}
			else
			{
				delete m_pBox;
				m_pBox = NULL;
				m_prevImage.FreeImages();
				return;
			}
		}
		else
		{
			delete m_pBox;
			m_pBox = NULL;
			m_prevImage.FreeImages();
		}

	}

}

awpRect TLFTrains::GetZoneRect()
{
	awpRect r;
	memset(&r, 0, sizeof(r));

	if (m_detector.GetSourceImage() == NULL)
		return r;
	int w = m_detector.GetSourceImage()->sSizeX;
	int h = m_detector.GetSourceImage()->sSizeY;

	TLFZone* zone = m_zones.GetZone(0);
	if (zone == NULL || !zone->IsRect())
		return r;

	TLF2DRect* rect = zone->GetRect();
	if (rect == NULL)
		return r;

	// преобразование rect к абсолютным координатам 

	r.left = (AWPSHORT)floor(rect->GetLeftTop().X*w / 100. + 0.5);
	r.top = (AWPSHORT)floor(rect->GetLeftTop().Y*h / 100. + 0.5);
	r.right = (AWPSHORT)floor(rect->GetRightBottom().X*w / 100. + 0.5);
	r.bottom = (AWPSHORT)floor(rect->GetRightBottom().Y*h / 100. + 0.5);

	return r;
}

bool  TLFTrains::CheckBoxScale(awpRect& r)
{
	awpRect zone = this->GetZoneRect();
	if (r.left <= zone.left || r.right >= zone.right || r.top <= zone.top || r.bottom >= zone.bottom)
		return false;
	return true;
}




int CompareItems(void* item1, void* item2)
{
	TLFDetectedItem* a = (TLFDetectedItem*)item1;
	TLFDetectedItem* b = (TLFDetectedItem*)item2;

	if (a->GetBounds()->Left() > b->GetBounds()->Left())
		return 1;
	else if (a->GetBounds()->Left() < b->GetBounds()->Left())
		return -1;
	else
		return 0;
}
int getTypeIndex(std::string str)
{
	if (str == "0")
		return 0;
	if (str == "1")
		return 1;
	if (str == "2")
		return 2;
	if (str == "3")
		return 3;
	if (str == "4")
		return 4;
	if (str == "5")
		return 5;
	if (str == "6")
		return 6;
	if (str == "7")
		return 7;
	if (str == "8")
		return 8;
	if (str == "9")
		return 9;
	return 0;
}
std::string getTypeString(int idx)
{
	if (idx == 0)
		return "0";
	if (idx == 1)
		return "1";
	if (idx == 2)
		return "2";
	if (idx == 3)
		return "3";
	if (idx == 4)
		return "4";
	if (idx == 5)
		return "5";
	if (idx == 6)
		return "6";
	if (idx == 7)
		return "7";
	if (idx == 8)
		return "8";
	if (idx == 9)
		return "9";
	return 0;
}

int getMaxTypeIndex(int* hst)
{
	int idx = 0;
	int max = hst[0];
	for (int i = 1; i < 10; i++)
	{
		if (hst[i] > max)
		{
			max = hst[i];
			idx = i;
		}
	}
	return idx;
}
// эта функция на вход получает дескриптор OCR изображения, состоящий 
// из некластеризованных результатов работы детекторов цифр. 
// возвращает кластеризованный дескриптор, состоящий из прямоугольников 
// и ассоциированными с ними цифрами. Для кластеризации используется 
// модификация алгоритма кластеризации в TLFDetectEngine. 
// Модификация заключается в том, что с каждым найденным класетром 
// ассоциирована гистограмма распередения цифр (hst_type) в этом кластере. 
// Кластеру присваивается та метка (цифра), у которой максимальное количетво 
// решений в гистограмме распределения цифр для кластера. 
TLFSemanticImageDescriptor* _ClusterDescriptor(TLFSemanticImageDescriptor* d)
{
	if (d == NULL)
		return NULL;

	TLFSemanticImageDescriptor* result = new TLFSemanticImageDescriptor();
	TLFObjectList objectDescr; // кластер. 
	int  hst_type[10];
	while (d->GetCount() > 0)
	{
		objectDescr.Clear();
		TLFDetectedItem* de = new TLFDetectedItem((TLFDetectedItem*)d->First());
		d->Delete(0);
		memset(hst_type, 0, sizeof(hst_type));

		objectDescr.Add(de);
		hst_type[getTypeIndex(de->GetType())]++;
		// overlap analysis
		for (int i = d->GetCount() - 1; i >= 0; i--)
		{
			TLFDetectedItem* di = (TLFDetectedItem*)d->Get(i);
			double e = 0.3;
//			if (strcmp(de->GetType().c_str(), "1") == 0)
//				e = 0.5;
			if (di->GetBounds()->IsCover(*de->GetBounds()) || de->GetBounds()->IsCover(*di->GetBounds()) || de->GetBounds()->RectOverlap(*di->GetBounds()) > e)// <- this is overlap threshold
			{
				//
				TLFDetectedItem* dd = new  TLFDetectedItem(di);
				objectDescr.Add(dd);
				hst_type[getTypeIndex(di->GetType())]++;
				d->Delete(i);
			}
		}
		// objectDescr analysis
		awpPoint p; p.X = 0; p.Y = 0;
		AWPSHORT w = 0;
		AWPSHORT h = 0;
		double max_raiting = 0;

		TLFDetectedItem* firstObject = (TLFDetectedItem*)objectDescr.Get(0);
		max_raiting = firstObject->GetRaiting();
		for (int i = 0; i < objectDescr.GetCount(); i++)
		{
			TLFDetectedItem* d = (TLFDetectedItem*)objectDescr.Get(i);
			p.X += d->GetBounds()->Center().X;
			p.Y += d->GetBounds()->Center().Y;
			w += d->GetBounds()->Width();
			h += d->GetBounds()->Height();
			if (max_raiting < d->GetRaiting())
				max_raiting = d->GetRaiting();
		}
		p.X /= objectDescr.GetCount();
		p.Y /= objectDescr.GetCount();
		w /= objectDescr.GetCount();
		h /= objectDescr.GetCount();
		// Object attributes
		awpRect r;
		double raiting;
		int   bw;
		int   bh;
		int   angle;
		std::string   type;
		r.left = p.X - w / 2;
		r.right = p.X + w / 2;
		r.top = p.Y - h / 2;
		r.bottom = p.Y + h / 2;
		angle = firstObject->GetAngle();
		type = getTypeString(getMaxTypeIndex(hst_type));
		raiting = max_raiting;
		bw = firstObject->GetBW();
		bh = firstObject->GetBH();
		UUID id;
		// memset(id, 0, sizeof(UUID));
		LF_NULL_UUID_CREATE(id);
		// add object to result list
		TLFDetectedItem* dd = new  TLFDetectedItem(&r, raiting, type, firstObject->GetAngle(), firstObject->GetRacurs(), bw, bh, firstObject->GetDetectorName(), id);
		result->AddDetectedItem(dd);
		delete dd;
	}
	return result;
}
/*
Вычисление евклидова расстояния между двумя точками
*/
AWPRESULT awpDistancePoints(awpPoint p1, awpPoint p2, AWPDOUBLE* distance)
{
	if (distance == NULL)
		return AWP_BADARG;
	*distance = fabs((double)(p1.X - p2.X));//sqrt((AWPFLOAT)((p1.X - p2.X)*(p1.X - p2.X) + (p1.Y - p2.Y)*(p1.Y - p2.Y)));
	return AWP_OK;
}
static double _ItemDistance(TLFDetectedItem* item1, TLFDetectedItem* item2)
{
	TLFRect* r1 = item1->GetBounds();
	TLFRect* r2 = item2->GetBounds();
	awpPoint p1 = r1->Center();
	awpPoint p2 = r2->Center();
	AWPDOUBLE distance = -1;
	awpDistancePoints(p1, p2, &distance);
	return distance;
}

struct SLFDistPos
{
	int		pos;
	double  distance;
};

// расстояние от кластера до точки. 
static double _DistancePointToCluster(TLFObjectList* cluster, TLFDetectedItem* item)
{
	double result;
	result = 1e38;
	for (int i = 0; i < cluster->GetCount(); i++)
	{
		TLFDetectedItem* di = (TLFDetectedItem*)cluster->Get(i);
		double value = _ItemDistance(item, di);
		if (value < result)
		{
			result = value;
		}
	}
	return result;
}

// расстояние от кластера до массива точек. 
static SLFDistPos _DistalceListToCluster(TLFObjectList* cluster, TLFObjectList* items)
{
	SLFDistPos result;
	result.pos = -1;
	result.distance = 1e38;
	for (int i = 0; i < items->GetCount(); i++)
	{
		TLFDetectedItem* di = (TLFDetectedItem*)items->Get(i);
		double value = _DistancePointToCluster(cluster, di);
		if (value <= result.distance)
		{
			result.distance = value;
			result.pos = i;
		}
	}
	return result;
}

TLFSemanticImageDescriptor* _ClusterDescriptorPoins(TLFSemanticImageDescriptor* d)
{
	if (d == NULL)
		return NULL;

	TLFSemanticImageDescriptor* result = new TLFSemanticImageDescriptor();
	TLFObjectList objectDescr;
	int  hst_type[10];
	while (d->GetCount() > 0)
	{
		objectDescr.Clear();
		TLFDetectedItem* de = new TLFDetectedItem((TLFDetectedItem*)d->First());
		d->Delete(0);
		memset(hst_type, 0, sizeof(hst_type));
		objectDescr.Add(de);
		hst_type[getTypeIndex(de->GetType())]++;
		double t = 8;
		SLFDistPos dp;
		do
		{
			dp = _DistalceListToCluster(&objectDescr, d);
			if (dp.distance <= t)
			{
				TLFDetectedItem* di = (TLFDetectedItem*)d->Get(dp.pos);
				TLFDetectedItem* dd = new  TLFDetectedItem(di);
				objectDescr.Add(dd);
				hst_type[getTypeIndex(di->GetType())]++;
				d->Delete(dp.pos);

			}
		} while (dp.distance <= t);

		// objectDescr analysis
		awpPoint p; p.X = 0; p.Y = 0;
		AWPSHORT w = 0;
		AWPSHORT h = 0;
		double max_raiting = 0;

		TLFDetectedItem* firstObject = (TLFDetectedItem*)objectDescr.Get(0);
		max_raiting = firstObject->GetRaiting();
		for (int i = 0; i < objectDescr.GetCount(); i++)
		{
			TLFDetectedItem* d = (TLFDetectedItem*)objectDescr.Get(i);
			p.X += d->GetBounds()->Center().X;
			p.Y += d->GetBounds()->Center().Y;
			w += d->GetBounds()->Width();
			h += d->GetBounds()->Height();
			if (max_raiting < d->GetRaiting())
				max_raiting = d->GetRaiting();
		}
		p.X /= objectDescr.GetCount();
		p.Y /= objectDescr.GetCount();
		w /= objectDescr.GetCount();
		h /= objectDescr.GetCount();
		// Object attributes
		awpRect r;
		double raiting;
		int   bw;
		int   bh;
		int   angle;
		std::string   type;
		r.left = p.X - w / 2;
		r.right = p.X + w / 2;
		r.top = p.Y - h / 2;
		r.bottom = p.Y + h / 2;
		angle = firstObject->GetAngle();
		type = getTypeString(getMaxTypeIndex(hst_type));//firstObject->GetType();
		if (result->GetCount() == 0 && strcmp(type.c_str(), "1") == 0)
			continue;
		raiting = max_raiting;
		bw = firstObject->GetBW();
		bh = firstObject->GetBH();
		UUID id;
		// memset(id, 0, sizeof(UUID));
		LF_NULL_UUID_CREATE(id);
		// add object to result list
		TLFDetectedItem* dd = new  TLFDetectedItem(&r, raiting, type, firstObject->GetAngle(), firstObject->GetRacurs(), bw, bh, firstObject->GetDetectorName(), id);
		result->AddDetectedItem(dd);
		delete dd;
	}
	return result;
}

bool  TLFTrains::SetupBox()
{
	double max_s = 0;
	awpRect rect;
	for (int i = 0; i < m_detector.GetItemsCount(); i++)
	{

		TLFDetectedItem* item = m_detector.GetItem(i);
		TLFRect* bounds = item->GetBounds();
		if (bounds != NULL)
		{
			int ww = bounds->Width();
			int hh = bounds->Height();
			if (ww*hh > max_s)
			{
				max_s = ww*hh;
				rect = bounds->GetRect();
			}
		}
	}
	awpRect r = rect;
	r.left /= m_detector.GetResizeCoef();
	r.top /= m_detector.GetResizeCoef();
	r.right /= m_detector.GetResizeCoef();
	r.bottom /= m_detector.GetResizeCoef();

	if (max_s == 0 || !this->CheckBoxScale(r))
		return false;
	

	if (m_pBox == NULL)
	{
		// setup m_pBox 
		m_pBox = new awpRect;
		*m_pBox = r;
	}
	return true;
}


/*
	return result = num found
    negative  = invalid params (NULL) or state (invalid zones)
*/
int TLFTrains::ProcessImage(awpImage* img, awpRect* rect, char* number)
{
	int result = -1;
	if (img == NULL || rect == NULL || number == NULL)
        return -1;

//#if 0
	m_detector.SetSourceImage(img, m_pBox == NULL);
	if (m_pBox != NULL)
	{
		TrackBox(m_detector.GetSourceImage());
		if (m_pBox != NULL)
		{
			*rect = *m_pBox;
			rect->left *= m_detector.GetResizeCoef();
			rect->top *= m_detector.GetResizeCoef();
			rect->right *= m_detector.GetResizeCoef();
			rect->bottom *= m_detector.GetResizeCoef();
			//todo: remove this return after attach OCR
			//return 1;
		}
		// this code skip one frame 
		else
			return 0;
	}
//#else 
//	m_detector.SetSourceImage(img, true);
//#endif 


    if (m_detector.GetItemsCount() == 0)
      	return 0;

      //we have an image as number detector responce
      //setup output params number as ????????
      //and fing biggest rect from responce

      //number = "????????";
	if (m_pBox == NULL && !this->SetupBox())
		return 0;
	// here is OCR processing 
	if (m_pBox != NULL)
      {
		 *rect = *m_pBox;
		 rect->left *= m_detector.GetResizeCoef();
		 rect->top *= m_detector.GetResizeCoef();
		 rect->right *= m_detector.GetResizeCoef();
		 rect->bottom *= m_detector.GetResizeCoef();

		 TLFRect r;
		 r.SetRect(*rect);
		 r.Inflate(20*r.Width()/100, 10*r.Height()/100);
		 *rect = r.GetRect();
		 awpImage* ocr_image = NULL;
         if (awpCopyRect (img, &ocr_image, rect) != AWP_OK)
         {
			 return 0;
         }
		 awpResizeBilinear(ocr_image, 240, 78);
#ifdef _DEBUG
		 awpSaveImage("ocr.awp", ocr_image);
#endif 
		 m_OCRImage.SetImage(ocr_image);
		 for (int i = 0; i < 10; i++)
		 {
			 TLFDetectEngine* e = (TLFDetectEngine*)m_digits.Get(i);
			 e->SetSourceImage(ocr_image, true);
		 }
		 TLFSemanticImageDescriptor res0;
		 res0.Clear();
		 for (int i = 0; i < m_digits.GetCount(); i++)
		  {
		 	 ILFDetectEngine* od = (ILFDetectEngine*)m_digits.Get(i);
			 TLFSemanticImageDescriptor* d = od->GetSemantic();
			 for (int j = 0; j < d->GetCount(); j++)
			 {
				 TLFDetectedItem* id = d->GetDetectedItem(j);
				 TLFDetectedItem* n = new TLFDetectedItem(id);
				 std::string str = LFIntToStr(i);
				 n->SetType(str.c_str());
				 res0.Add(n);
			 }
		  }
		  res0.Sort(CompareItems);

		  m_res0.Clear();
		  for (int i = 0; i < res0.GetCount(); i++)
		  {
			  TLFDetectedItem* di = res0.GetDetectedItem(i);
			  TLFDetectedItem* d = new TLFDetectedItem(di);
			  m_res0.Add(d);
		  }

		  char c[18];
		  memset(c, 35, 8 * sizeof(char));
		 
		  // вызываем функцию кластеризации результата работы детекторов. 
		  TLFSemanticImageDescriptor* res = _ClusterDescriptorPoins(&res0);// _ClusterDescriptor(&res0);
		  if (res->GetCount() < 7)
		  {
			  _AWP_SAFE_RELEASE_(ocr_image);
			  delete res;
			  delete m_pBox;
			  m_pBox = NULL;
			  return 0;
		  }
		  if (res != NULL)
		  {
			  res->Sort(CompareItems);
			  double avg = 0;
			  for (int i = 0; i < res->GetCount()-1; i++)
			  {
				  TLFDetectedItem* di1 = res->GetDetectedItem(i);
				  TLFDetectedItem* di2 = res->GetDetectedItem(i+1);
				  avg += _ItemDistance(di1, di2);
			  }
			  avg /= res->GetCount();
			  int count = res->GetCount() > 8 ? 8 : res->GetCount();
			  int i = 0;
			  int k = 0;
			  while (k < count)
			  {
				  TLFDetectedItem* id = res->GetDetectedItem(k);
				  double d = avg;
				  if (k < count - 1)
				  {
					  TLFDetectedItem* id2 = res->GetDetectedItem(k + 1);
					  d = _ItemDistance(id, id2);
				  }
				  k++; 
					std::string s = id->GetType();
					const char* cc = s.c_str();
					c[i] = cc[0];
					i++;
					if (i > 8)
						break;
					if (d > 1.5*avg && res->GetCount() < 8)
					{
						c[i] = 35;
						i++;

					}
			  }
				memcpy(number, c, 8 * sizeof(char));
				result = 1;
			  _AWP_SAFE_RELEASE_(ocr_image);
			  delete res;
		  }
	  }

	  return result;
}
void TLFTrains::SetZones(TLFZones& zones)
{
   m_zones = zones;
}

double TLFTrains::GetThreshold()
{
	return 0;
}

void   TLFTrains::SetThreshold(double value)
{

}

TLFDetectEngine* TLFTrains::GetDetector()
{
	return &m_detector;
}

TLFObjectList*   TLFTrains::GetDigitsDetectors()
{
	return &m_digits;
}


bool TLFTrains::InitOCR(const char* lpFileName)
{
	string strPath = lpFileName;
	string strName = strPath;
	strName += "engine.xml";
    m_detector.Clear();
    m_digits.Clear();
	if (!m_detector.Load(strName.c_str()))
		return false;
	for (int i = 0; i < 10; i++)
	{
		TLFDetectEngine* e = new TLFDetectEngine();
		std::string name = strPath;
		name += LFIntToStr(i);
		name += ".xml";
		if (!e->Load(name.c_str()))
			return false;
		m_digits.Add(e);
	}
	return true;
}
TLFSemanticImageDescriptor* TLFTrains::GetRawSescriptor()
{
	return &m_res0;
}

TLFImage*  TLFTrains::GetOCRImage()
{
	return &m_OCRImage;
}
//---------- target 
TLFTrainsTarget::TLFTrainsTarget(TLFZones& zones, TVAInitParams* params)
{
	m_zones = zones; 
	m_model_visible = false;
	CopyParams(params, &this->m_params);
}
TLFTrainsTarget::~TLFTrainsTarget()
{
	if (this->m_params != NULL)
		FreeParams(&this->m_params);
}

// модель 
bool TLFTrainsTarget::InitModel(awpImage* image)
{
	if (image == NULL)
		return false;
	awpImage* copy = NULL;
	if (awpCopyImage(image, &copy) != AWP_OK)
		return false;

//	if (awpRectInImage(image, &rect) != AWP_OK)
//		return false;
// todo: compare source model and resized model 
//	if (awpResizeBilinear(copy, MODEL_WIDTH, MODEL_HEIGHT) != AWP_OK)
//	{
//		_AWP_SAFE_RELEASE_(copy)
//			return false;
//	}
	if (awpConvert(copy, AWP_CONVERT_3TO1_BYTE) != AWP_OK)
	{
		_AWP_SAFE_RELEASE_(copy)
			return false;
	}

	bool result = m_model.SetImage(copy);

	_AWP_SAFE_RELEASE_(copy)
		return result;
}
bool TLFTrainsTarget::LoadModel(const char* lpFileName)
{
	bool result = m_model.LoadDump(lpFileName);
	if (!result)
		return result;
	awpImage* img = m_model.GetImage();
	if (awpConvert(img, AWP_CONVERT_3TO1_BYTE) != AWP_OK)
	{
		m_model.FreeImages();
		return false;
	}
	return true;
}
bool TLFTrainsTarget::SaveModel(const char* lpFileName)
{
	return m_model.SaveDump(lpFileName);
}
bool TLFTrainsTarget::HasModel()
{
	return m_model.GetImage() != NULL;
}
/*
по умолчанию модель не видна и m_model_visible = false
*/
bool TLFTrainsTarget::IsTargetVisible()
{
	return m_model_visible;
}
/*
	выполняет сравнение изоюражения  image 
	с моделью m_model. 
	Фрагмент изображения image, взяиый из второй заны m_zonesобесцвечивается и приводится 
	к размеру MODEL_WIDTH и MODEL_HEIGHT
	затем выполняется сравнения с использованием алгоритма NCC 

	1  - изображения полностью совпадают. 
	0  - не совпадают 
	-1 - ошибка во время выполнения 
*/
double TLFTrainsTarget::CompareModel(awpImage* image)
{
	double result = -1;
	bool   visible = false;
	if (!this->HasModel())
		return result;
	if (m_zones.GetCount() != 1)
		return result;
	// выбираем первую зону. 
	TLFZone* zone = m_zones.GetZone(0);
	if (zone == NULL || !zone->IsRect())
		return result;

	TLF2DRect* rect = zone->GetRect();
	if (rect == NULL)
		return result;

	// преобразование rect к абсолютным координатам 
	awpRect r;
	r.left  = (AWPSHORT)floor(rect->GetLeftTop().X*image->sSizeX / 100. + 0.5);
	r.top = (AWPSHORT)floor(rect->GetLeftTop().Y*image->sSizeY / 100. + 0.5);
	r.right = (AWPSHORT)floor(rect->GetRightBottom().X*image->sSizeX / 100. + 0.5);
	r.bottom = (AWPSHORT)floor(rect->GetRightBottom().Y*image->sSizeY / 100. + 0.5);

	// копируем фрагмент изображения. 
	awpImage* dst = NULL;
	if (awpCopyRect(image, &dst, &r) != AWP_OK)
		return result;

	if (awpResizeBilinear(dst, m_model.GetImage()->sSizeX, m_model.GetImage()->sSizeY) != AWP_OK)
	{
		_AWP_SAFE_RELEASE_(dst)
			return result;
	}
	if (awpConvert(dst, AWP_CONVERT_3TO1_BYTE) != AWP_OK)
	{
		_AWP_SAFE_RELEASE_(dst)
		return result;
	}

	if (awpDistance(dst, m_model.GetImage(), AWP_DIST_NCC, &result) != AWP_OK)
	{
		result = -1;
		_AWP_SAFE_RELEASE_(dst)
		return result;
	}

	_AWP_SAFE_RELEASE_(dst)

	double t = 0.5 + (1 - this->m_params->EventSens) / 2.;
	this->m_model_visible = result > t;

	return result >=0 ? result:0;
}

TVAInitParams* TLFTrainsTarget::getModuleParams()
{
	return this->m_params;
}

void TLFTrainsTarget::ClearModel()
{
	this->m_model.FreeImages();
}



#if  defined(__BCPLUSPLUS__)
	#pragma package(smart_init)
#endif
