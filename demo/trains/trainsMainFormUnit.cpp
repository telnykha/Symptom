//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "trainsMainFormUnit.h"
#include "VerInfoUnit.h"
#include "aboutUnit.h"
#include "PhVideo.h"
#include "TuningUnit.h"
#include "DIBImage41.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FImage41"
#pragma link "PhImageTool"
#pragma link "PhPaneTool"
#pragma link "PhTrackBar"
#pragma resource "*.dfm"

#pragma link "awplflibb.lib"
#pragma link "TinyXML.lib"
#pragma link "awpipl2b.lib"
#pragma link "JPEGLIB.LIB"
#pragma link "trains.lib"
#pragma link "vautils.lib"

TmainForm *mainForm;

#define _CHECK_SOURCE_     \
if (m_videoSource == NULL) \
    return;                \

#define _NAV_ENABLED_(v) \
    v->Enabled = m_videoSource != NULL && m_videoSource->NumFrames > 1 && !this->m_videoSource->IsPlaying;

#define _MODE_ENABLED_(v) \
    v->Enabled = m_videoSource != NULL && m_videoSource->NumFrames >= 0;

//---------------------------------------------------------------------------
__fastcall TmainForm::TmainForm(TComponent* Owner)
	: TForm(Owner)
{
    m_videoSource = NULL;
    m_trains_params = NULL;
    m_target_params = NULL;
    m_trainsTool = NULL;
    m_rect_visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::CloseActionExecute(TObject *Sender)
{
    if (m_videoSource != NULL)
        delete m_videoSource;
    if (m_trains_params != NULL)
        FreeParams(&m_trains_params);
    if (m_target_params != NULL)
        FreeParams(&m_target_params);
    if (m_trainsTool != NULL)
        delete m_trainsTool;

    Close();
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::UpdateImage()
{
        // установка размеров изображения.
        // если размер изображения больше размеров окна -
		// выполняем операцию BestFit
        // иначе выполняем операцию ActualSize

        int w = FImage1->Width;
        int h = FImage1->Height;
        int w1 = FImage1->Bitmap->Width;
        int h1 = FImage1->Bitmap->Height;

        if (w1 > w || h1 > h)
            FImage1->BestFit();
        else
            FImage1->ActualSize();

}
//---------------------------------------------------------------------------
void __fastcall TmainForm::FormResize(TObject *Sender)
{
    UpdateImage();
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::ModePaneActionExecute(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::ModePaneActionUpdate(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::ModeSelRectActionExecute(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::ModeSelRectActionUpdate(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::ActualSizeActionUpdate(TObject *Sender)
{
    ActualSizeAction->Enabled = !FImage1->Bitmap->Empty;
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::ActualSizeActionExecute(TObject *Sender)
{
    FImage1->ActualSize();
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::BestFitActionExecute(TObject *Sender)
{
    FImage1->BestFit();
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::BestFitActionUpdate(TObject *Sender)
{
     BestFitAction->Enabled = !FImage1->Bitmap->Empty;
}
//---------------------------------------------------------------------------


void __fastcall TmainForm::AboutActionExecute(TObject *Sender)
{
 // AboutBox->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::CloseVideoActionExecute(TObject *Sender)
{
   // Закрыть видеоисточник
   if (m_videoSource != NULL)
   {
      delete m_videoSource;
      m_videoSource = NULL;
   }
   FImage1->Close();
   FImage1->Refresh();
   SetSource(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::CloseVideoActionUpdate(TObject *Sender)
{
 // обновление состояния команды. источник закрывается, если
 // он открыт. анализируется IsInitialized

}
//---------------------------------------------------------------------------


void __fastcall TmainForm::FImage1BeforeOpen(TObject *Sender)
{
    //устанавливает тип открываемого видеоисточника.
    ModePaneActionExecute(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::OpenVideoActionExecute(TObject *Sender)
{
	AnsiString cstrVideos = "Videos |*.avi;*.mp4;*.mpg;|Avi videos|*.avi;|MP4 videos|*.mp4|mpeg files|*.mpg";
    OpenDialog1->Filter = cstrVideos;

    if (OpenDialog1->Execute())
    {
        m_engine.Reset();
        TPhMediaSource* videoSource= new TPhVideoSource(FImage1);
        videoSource->Open(OpenDialog1->Files);
        if (videoSource->NumFrames == 0)
        {
            ShowMessage(L"Не могу открыть файл: " + OpenDialog1->FileName);
            return;
        }
        SetSource(videoSource);
        SetMode(modeTuningAction);
    }

}
//---------------------------------------------------------------------------

void __fastcall TmainForm::OpenUSBCameraActionExecute(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::OpenIPCameraActionExecute(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::PlayActionExecute(TObject *Sender)
{
	_CHECK_SOURCE_
    if (!m_videoSource->IsPlaying)
    {
	    m_videoSource->Play();
		SpeedButton9->Caption = "пауза";
        SpeedButton9->Font->Color = clBlue;
        SpeedButton9->Down = true;
    }
    else
    {
        m_videoSource->Stop();
       SpeedButton9->Caption = "воспр.";
       SpeedButton9->Font->Color = clBlack;
    }
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::PlayActionUpdate(TObject *Sender)
{
 PlayAction->Enabled = m_videoSource != NULL && (m_videoSource->NumFrames > 1 || m_videoSource->NumFrames == 0);
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::FirstFrameActionExecute(TObject *Sender)
{
	_CHECK_SOURCE_
    m_videoSource->First();
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::FirstFrameActionUpdate(TObject *Sender)
{
 	_NAV_ENABLED_(FirstFrameAction)
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::PrevFrameActionExecute(TObject *Sender)
{
	_CHECK_SOURCE_
    m_videoSource->Prev();
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::PrevFrameActionUpdate(TObject *Sender)
{
 	_NAV_ENABLED_(PrevFrameAction)
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::NextFrameActionExecute(TObject *Sender)
{
	_CHECK_SOURCE_
    m_videoSource->Next();
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::NextFrameActionUpdate(TObject *Sender)
{
 	_NAV_ENABLED_(NextFrameAction)
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::LastFrameActionExecute(TObject *Sender)
{
	_CHECK_SOURCE_
    m_videoSource->Last();
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::LastFrameActionUpdate(TObject *Sender)
{
	 _NAV_ENABLED_(LastFrameAction)
}


void __fastcall TmainForm::FImage1Frame(TObject *Sender, int widht,
      int height, int bpp, char *data)
{
   awpImage img;
   img.nMagic = AWP_MAGIC2;
   img.bChannels = bpp / 8;
   img.sSizeX =  widht;
   img.sSizeY = height;
   img.dwType = AWP_BYTE;
   img.pPixels = data;

   this->m_copy.SetImage(&img);


   awpImage* tmp = NULL;
   awpCopyImage(&img, &tmp);
//todo:   if (m_trains != NULL && tmp != NULL &&  this->CheckBox5->Checked)
/*   {
       awpRect result;
       char number[32];
       awpConvert(tmp, AWP_CONVERT_3TO1_BYTE);
       if (m_trains->ProcessImage(tmp, &result, number) > 0)
       {
            TVideoEvent ee;
            ee.Frame = FImage1->CurrentTime;
            ee.Duration = 1000;
            ee.Value = 1;
            ee.ExtValue = 1;

            awpDrawRect(&img,&result,2,255, 4);

            FImage1->AddVideoEvent(&ee);
            TVideoEvent*  e = FImage1->VideoEvent[FImage1->NumVideoEvents-1];
            ListView1->SetFocus();

			TListItem* item = ListView1->Items->Add();
            item->Caption = IntToStr(FImage1->NumVideoEvents);
            item->SubItems->Add(FImage1->TimeMsToString(e->Frame));
            item->SubItems->Add("1");
            item->SubItems->Add("1");
            item->Selected = true;
            item->MakeVisible(true);
            AnsiString str = number;
            Label1->Caption = str;
       }
   }*/
   awpReleaseImage(&tmp);
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::RenderZones(awpImage* image)
{
    if (!viewZonesAction->Checked)
        return;
    if (image == NULL)
        return;
    if (m_trains_params == NULL)
        return;
    if (m_trains_params->NumZones != 1)
        return;
    if (!m_trains_params->Zones[0].IsRect)
        return;
    if (m_target_params == NULL)
        return;
    if (m_target_params->NumZones != 1)
        return;
    if (!m_target_params->Zones[0].IsRect)
        return;


    //todo: render VAInitParams
    int w = image->sSizeX;
    int h = image->sSizeY;

    //
    TVARect zone = m_trains_params->Zones[0].Rect;
    awpRect rect;
    rect.left = zone.LeftTop.X * w / 100.;
    rect.top  = zone.LeftTop.Y * h / 100.;
    rect.right = zone.RightBottom.X * w / 100.;
    rect.bottom = zone.RightBottom.Y*h / 100.;

    awpDrawCRect(image, &rect, 0,0,255,1);

    zone = m_target_params->Zones[0].Rect;
    rect;
    rect.left = zone.LeftTop.X * w / 100.;
    rect.top  = zone.LeftTop.Y * h / 100.;
    rect.right = zone.RightBottom.X * w / 100.;
    rect.bottom = zone.RightBottom.Y*h / 100.;

    awpDrawCRect(image, &rect, 0,255,0,1);
}

void __fastcall TmainForm::RenderResult(awpImage* image)
{
    if (!this->viewDetectRectAction->Checked)
        return;
    if (image == NULL)
        return;
    if (!this->m_rect_visible)
        return;

    int w = image->sSizeX;
    int h = image->sSizeY;

    awpRect rect;
    rect.left = nrect.LeftTop.X * w / 100.;
    rect.top  = nrect.LeftTop.Y * h / 100.;
    rect.right = nrect.RightBottom.X * w / 100.;
    rect.bottom = nrect.RightBottom.Y*h / 100.;

    awpDrawCRect(image, &rect, 255,255,0,3);
}


void __fastcall TmainForm::FormShow(TObject *Sender)
{
    TVersionInfo* vi = new TVersionInfo(NULL);
	Caption = L"Обнаружение и распознавание номеров вагонов.  v " + vi->FileVersion;
    delete vi;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

static AnsiString TimeMsToTimeStr(double _time)
{
    _time /= 1000;
    long sec  = _time / 1000;
    long min  = sec / 60;
    sec %= 60;
	double hour = min / 60;
	min %=60;
    long msec = _time - hour*60*60*1000 - min*60*1000 - sec*1000;

    UnicodeString result = FormatFloat("00",(double)hour) + ":" + FormatFloat("00", min) + ":" + FormatFloat("00", sec);
    AnsiString _ansi = result;
    return _ansi;
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::N16Click(TObject *Sender)
{
  TVersionInfo* vi = new TVersionInfo(NULL);
  AboutBox->ProductName->Caption = vi->ProductName;
  AboutBox->Version->Caption = vi->FileVersion;
  AboutBox->Copyright->Caption = vi->LegalCopyright;
  AboutBox->Comments->Caption = vi->Comments;
  AboutBox->ShowModal();
  delete vi;
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::FImage1Resize(TObject *Sender)
{
    //todo:
}
//---------------------------------------------------------------------------


void __fastcall TmainForm::GotoFrameActionExecute(TObject *Sender)
{
  _CHECK_SOURCE_

  UnicodeString InputString = InputBox(L"Trains demo", L"Переход на кадр", IntToStr(FImage1->Frames->CurrentFrame));
  if (InputString != IntToStr(FImage1->Frames->CurrentFrame))
  {
  	 try
     {
     	int f = StrToInt(InputString);
        if (f < 0 || f > m_videoSource->NumFrames)
        {
        	ShowMessage("Недопустимый номер кадра.");
            return;
        }
       this->m_videoSource->CurrentFrame = f;

     }
     catch(EConvertError& e)
     {

     }
  }
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::GotoFrameActionUpdate(TObject *Sender)
{
	_NAV_ENABLED_(GotoFrameAction)
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::FImage1FrameData(TObject *Sender, int w, int h, int c,
          BYTE *data)
{
    if (m_videoSource != NULL)
    {
		StatusBar1->Panels->Items[1]->Text = L"Кадр "  + IntToStr( m_videoSource->CurrentFrame) +
   		" of " + IntToStr(m_videoSource->NumFrames);
	    StatusBar1->Panels->Items[3]->Text = L"Изобр: " + IntToStr(w) + L":" + IntToStr(h) + L":" + IntToStr(c);

        PhTrackBar1->Position = m_videoSource->CurrentFrame;
    }
    else
		StatusBar1->Panels->Items[1]->Text = L" ";

   awpImage img;
   img.nMagic = AWP_MAGIC2;
   img.bChannels = c;
   img.sSizeX =  w;
   img.sSizeY = h;
   img.dwType = AWP_BYTE;
   img.pPixels = data;

   m_engine.SetImage(&img);
   m_copy.SetImage(&img);

   RenderZones(&img);
   RenderResult(&img);
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::SetSource(TPhMediaSource* source)
{
   m_engine.Reset();

   if (m_videoSource != NULL)
   {
      delete m_videoSource;
      m_videoSource = NULL;
   }

   m_videoSource = source;
   if (!this->InitParams())
   {
      delete m_videoSource;
      m_videoSource = NULL;
      ShowMessage("Невозможно инициализировать параметры!");
      return;
   }


   if (m_videoSource  != NULL)
   {
        m_videoSource->First();
		StatusBar1->Panels->Items[1]->Text = L"Кадр "  + IntToStr( m_videoSource->CurrentFrame) +
   		" of " + IntToStr(m_videoSource->NumFrames);
	   StatusBar1->Panels->Items[2]->Text = L"Источник данных: " + m_videoSource->SourceName;
       //StatusBar1->Panels->Items[3]->Text = L"Изображение: " + IntToStr(w) + L":" + IntToStr(h) + L":" + IntToStr(c);
       UnicodeString cap = L"Обнаружение и распознавание номеров вагонов.  [";
       cap += m_videoSource->Source;
       cap  += L"]";
       this->Caption = cap;

        PhTrackBar1->Min = 0;
        PhTrackBar1->Max = m_videoSource->NumFrames;

   }
   else
   {
	   StatusBar1->Panels->Items[1]->Text = L"";
	   StatusBar1->Panels->Items[2]->Text = L"Источник данных: NULL";
       this->Caption = L"Обнаружение и распознавание номеров вагонов.";
  	   StatusBar1->Panels->Items[3]->Text = L"";

       PhTrackBar1->Min = 0;
       PhTrackBar1->Max = 0;
   }

}

void __fastcall TmainForm::SetMode(TAction* action)
{
	m_modeAction = action;
    if (m_modeAction != NULL)
    {
        action->Checked = true;
        StatusBar1->Panels->Items[0]->Text = L"Режим: " + action->Caption;
        TuningForm->Visible = m_modeAction == modeTuningAction;
        if (m_modeAction != modeTuningAction)
        {
            FImage1->SelectPhTool(PhPaneTool1);
            m_engine.Init(m_trains_params, m_target_params);
        }
        else
        {
            TDIBImage* dib = (TDIBImage*) FImage1->Bitmap;
            if (dib == NULL)
                return;
            dib->SetAWPImage(m_copy.GetImage());


            if (m_trainsTool)
                delete m_trainsTool;
            m_trainsTool = new TPhTrainsTool(NULL);
            m_trainsTool->PhImage = this->FImage1;
            m_trainsTool->modelRect = m_target_params->Zones[0].Rect;
            m_trainsTool->numberRect = m_trains_params->Zones[0].Rect;
            m_trainsTool->OnChange = ToolChange;
            FImage1->SelectPhTool(m_trainsTool);
            FImage1->Paint();
        }

    }
    else
    {
        FImage1->SelectPhTool(PhPaneTool1);
        StatusBar1->Panels->Items[0]->Text = L"Режим: ";
        TuningForm->Visible = false;
    }
}


void __fastcall TmainForm::modeAnalysisActionExecute(TObject *Sender)
{
    SetMode(modeAnalysisAction);
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::modeAnalysisActionUpdate(TObject *Sender)
{
	_MODE_ENABLED_(modeAnalysisAction)
    modeAnalysisAction->Enabled = modeAnalysisAction->Enabled && (TuningForm->CheckBox1->Checked || TuningForm->CheckBox2->Checked);
    if (TuningForm->CheckBox2->Checked)
        modeAnalysisAction->Enabled = modeAnalysisAction->Enabled && TuningForm->IsModelAvailable;
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::modeTuningActionExecute(TObject *Sender)
{
    SetMode(modeTuningAction);
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::modeTuningActionUpdate(TObject *Sender)
{
	_MODE_ENABLED_(modeTuningAction)
}
//---------------------------------------------------------------------------
bool __fastcall TmainForm::CreateDefaultParams(UnicodeString& strSourceName)
{
    AnsiString    _ansi = "";


    TVAInitParams params;
    memset(&params, 0, sizeof(params));

    params.SaveLog = false;
    params.NumZones = 1;
    params.Zones = new TVAZone[1];
    params.Zones[0].IsRect = true;
    params.Zones[0].Rect.LeftTop.X = 30;
    params.Zones[0].Rect.LeftTop.Y = 30;
    params.Zones[0].Rect.RightBottom.X = 60;
    params.Zones[0].Rect.RightBottom.Y = 60;
    params.EventSens = 0.5;

    if (!CopyParams(&params, &m_trains_params))
    {
        delete params.Zones;
        return false;
    }
    UnicodeString strFileName = ChangeFileExt(strSourceName, ".trains");
    _ansi = strFileName;
    if (!SaveInitParams(_ansi.c_str(), m_trains_params))
    {
        FreeParams(&m_trains_params);
        delete params.Zones;
        return false;
    }

    params.Zones[0].Rect.LeftTop.X = 45;
    params.Zones[0].Rect.LeftTop.Y = 45;
    params.Zones[0].Rect.RightBottom.X = 55;
    params.Zones[0].Rect.RightBottom.Y = 55;
    params.EventSens = 0.5;

    //
    UnicodeString strDmpName = ChangeFileExt(strSourceName, ".model");
    _ansi = strDmpName;
    params.Path = _ansi.c_str();

    if (!CopyParams(&params, &m_target_params))
    {
        FreeParams(&m_trains_params);
        delete params.Zones;
        return false;
    }
    strFileName = ChangeFileExt(strSourceName, ".target");
    _ansi = strFileName;
    if (!SaveInitParams(_ansi.c_str(), m_target_params))
    {
        FreeParams(&m_trains_params);
        FreeParams(&m_target_params);
        delete params.Zones;
        return false;
    }

    m_engine.Init(m_trains_params, m_target_params);
    return true;
}

bool __fastcall TmainForm::InitParams()
{

   if (m_videoSource == NULL)
    return true;

   	UnicodeString str = this->m_videoSource->Source;

    UnicodeString str_trains = ChangeFileExt(str, ".trains");
    UnicodeString str_target = ChangeFileExt(str, ".target");

    if (m_trains_params != NULL)
        FreeParams(&m_trains_params);
    if (m_target_params != NULL)
        FreeParams(&m_target_params);


    if (FileExists(str_trains, true) && FileExists(str_target, true))
    {


    	AnsiString _ansi_trins  = str_trains;
    	AnsiString _ansi_target = str_target;
        if (LoadInitParams(_ansi_trins.c_str(), &m_trains_params) && LoadInitParams(_ansi_target.c_str(), &m_target_params))
        {
		    m_engine.Init(m_trains_params, m_target_params);
            return true;
        }
        else
	        return this->CreateDefaultParams(str);
    }
    else
    {
        return this->CreateDefaultParams(str);
    }
    return true;
}

void __fastcall TmainForm::UpdateParams()
{
    if (m_videoSource == NULL)
	    return;

   	UnicodeString str = this->m_videoSource->Source;

    UnicodeString str_trains = ChangeFileExt(str, ".trains");
    UnicodeString str_target = ChangeFileExt(str, ".target");

    AnsiString _ansi_trins  = str_trains;
    AnsiString _ansi_target = str_target;

    SaveInitParams(_ansi_trins.c_str(), m_trains_params);
    SaveInitParams(_ansi_target.c_str(), m_target_params);

    m_engine.Init(m_trains_params, m_target_params);

}

TTrainsAnalysisEngine* __fastcall TmainForm::GetEngine()
{
     return &m_engine;
}

bool __fastcall TmainForm::CreateModel()
{
    awpImage* img = NULL;
    bool res = false;

    TDIBImage* dib = (TDIBImage*) FImage1->Bitmap;
    if (dib == NULL)
        return false;
    dib->GetAWPImage(&img);
    if (img != NULL)
    {

        awpImage* model = this->GetModelImage(img);
        if (model != NULL)
        {
            res = m_engine.CreateModel(model);
	        _AWP_SAFE_RELEASE_(model)
        }

        _AWP_SAFE_RELEASE_(img)
    }

    return res;
}

void __fastcall TmainForm::ToolChange(TObject *Sender)
{
    //
    TPhTrainsTool* tool = (TPhTrainsTool*)Sender;
    m_target_params->Zones[0].Rect = tool->modelRect;
    m_trains_params->Zones[0].Rect = tool->numberRect;
    //UpdateParams();
    TuningForm->IsParamsEdited = true;
}


void __fastcall TmainForm::viewDetectRectActionExecute(TObject *Sender)
{
    viewDetectRectAction->Checked = !viewDetectRectAction->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::viewZonesActionExecute(TObject *Sender)
{
    viewZonesAction->Checked = !viewZonesAction->Checked;
}
//---------------------------------------------------------------------------


void __fastcall TmainForm::PhTrackBar1Change(TObject *Sender)
{
    if (this->m_videoSource != NULL)
    {
		StatusBar1->Panels->Items[1]->Text = L"Кадр "  + IntToStr( PhTrackBar1->Position ) +
   		" of " + IntToStr(m_videoSource->NumFrames);

    }
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::PhTrackBar1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    if (this->m_videoSource != NULL)
        m_videoSource->CurrentFrame = PhTrackBar1->Position;

}
//---------------------------------------------------------------------------

void __fastcall TmainForm::PhTrackBar1KeyUp(TObject *Sender, WORD &Key, TShiftState Shift)

{
    if (this->m_videoSource != NULL)
        m_videoSource->CurrentFrame = PhTrackBar1->Position;
}
//---------------------------------------------------------------------------
awpImage* __fastcall TmainForm::GetModelImage(awpImage* image)
{
    if (image == NULL || m_target_params == NULL || m_target_params->NumZones != 1 ||
    !m_target_params->Zones[0].IsRect)
        return NULL;

    //
    int w = image->sSizeX;
    int h = image->sSizeY;
    TVARect vr = m_target_params->Zones[0].Rect;

    awpRect r;
    r.left = vr.LeftTop.X* w / 100.;
    r.right = vr.RightBottom.X*w / 100.;
    r.top = vr.LeftTop.Y * h / 100.;
    r.bottom = vr.RightBottom.Y * h / 100.;

    awpImage* res = NULL;

    if (awpCopyRect(image, &res, &r) != AWP_OK)
        return NULL;
    return res;
}



