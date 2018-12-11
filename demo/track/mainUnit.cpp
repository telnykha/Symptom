//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "mainUnit.h"
#include "PhVideo.h"
#include "DIBImage41.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

#pragma link "awplflibb.lib"
#pragma link "TinyXML.lib"
#pragma link "awpipl2b.lib"
#pragma link "JPEGLIB.LIB"


#pragma link "FImage41"
#pragma link "PhImageTool"
#pragma link "PhPaneTool"
#pragma link "PhTrackBar"
#pragma resource "*.dfm"
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
    m_trj.Num = 0;
    m_trj.Trajectories = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::OpenVideoActionExecute(TObject *Sender)
{
	AnsiString cstrVideos = "Videos |*.avi;*.mp4;*.mpg;|Avi videos|*.avi;|MP4 videos|*.mp4|mpeg files|*.mpg";
    OpenDialog1->Filter = cstrVideos;

    if (OpenDialog1->Execute())
    {
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
void __fastcall TmainForm::SetSource(TPhMediaSource* source)
{
   m_engine.Reset();

   if (m_videoSource != NULL)
   {
      delete m_videoSource;
      m_videoSource = NULL;
   }

   m_videoSource = source;
   if (m_videoSource  != NULL)
   {
       UnicodeString str = m_videoSource->Source;
       AnsiString _ansi = str;
       if (!m_engine.Init(_ansi.c_str()))
       {
          m_engine.Reset();
          delete m_videoSource;
          m_videoSource = NULL;
          ShowMessage("Невозможно инициализировать параметры!");
          return;
       }


        m_videoSource->First();
        StatusBar1->Panels->Items[1]->Text = L"Кадр "  + IntToStr( m_videoSource->CurrentFrame) +
        " of " + IntToStr(m_videoSource->NumFrames);
	   StatusBar1->Panels->Items[2]->Text = L"Источник данных: " + m_videoSource->SourceName;
       UnicodeString cap = L"Видеоаналитика Symptom 3.0.  [";
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
       Caption = L"Видеоаналитика Symptom 3.0.";
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
        //TuningForm->Visible = m_modeAction == modeTuningAction;
        if (m_modeAction != modeTuningAction)
        {
//            FImage1->SelectPhTool(PhPaneTool1);
//            m_engine.Init(m_trains_params, m_target_params);
        }
        else
        {
            TDIBImage* dib = (TDIBImage*) FImage1->Bitmap;
            if (dib == NULL)
                return;
//            dib->SetAWPImage(m_copy.GetImage());
            FImage1->Paint();
        }

    }
    else
    {
        FImage1->SelectPhTool(PhPaneTool1);
        StatusBar1->Panels->Items[0]->Text = L"Режим: ";
        //TuningForm->Visible = false;
    }

}
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

void __fastcall TmainForm::FImage1FrameData(TObject *Sender, int w, int h, int c,
          BYTE *data)
{
    if (m_videoSource != NULL)
    {
		StatusBar1->Panels->Items[1]->Text = L"Кадр "  + IntToStr( m_videoSource->CurrentFrame) +
   		" of " + IntToStr(m_videoSource->NumFrames);
	    StatusBar1->Panels->Items[3]->Text = L"Изображение: " + IntToStr(w) + L":" + IntToStr(h) + L":" + IntToStr(c);

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

   RenderFG(&img);
   RenderResult(&img);
   RenderTrajectories(&img);
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::CloseActionExecute(TObject *Sender)
{
    if (m_videoSource != NULL)
        delete m_videoSource;
    Close();
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

void __fastcall TmainForm::modeAnalysisActionExecute(TObject *Sender)
{
    SetMode(modeAnalysisAction);
}
//---------------------------------------------------------------------------

void __fastcall TmainForm::modeAnalysisActionUpdate(TObject *Sender)
{
	_MODE_ENABLED_(modeAnalysisAction)
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
TTrackAnalysisEngine* __fastcall TmainForm::GetEngine()
{
    return &this->m_engine;
}
//---------------------------------------------------------------------------
awpImage* __fastcall TmainForm::GetFg()
{
    return m_fg;
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::SetFg(awpImage* fg)
{
    _AWP_SAFE_RELEASE_(m_fg)

    if (fg != NULL)
        awpCopyImage(fg, &m_fg);

    _AWP_SAFE_RELEASE_(fg)
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::RenderFG(awpImage* img)
{
    if (img == NULL || m_fg == NULL)
        return;
    if (!viewForegroundAction->Checked)
        return;

    AWPBYTE* _img = (AWPBYTE*)img->pPixels;
    AWPBYTE* _fg = (AWPBYTE*)m_fg->pPixels;

    for (int i = 0; i < img->sSizeX*img->sSizeY; i++)
    {
        _img[3*i] = _fg[i] == 0 ? _img[3*i]:255;
    }
}

void __fastcall TmainForm::viewForegroundActionExecute(TObject *Sender)
{
    viewForegroundAction->Checked = !viewForegroundAction->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::SetResult(TVAResult& result)
{
    if (m_result.Num > 0)
    {
        delete m_result.blobs;
        m_result.blobs = NULL;
        m_result.Num = 0;
    }

    m_result.Num = result.Num;
    if (m_result.Num > 0)
    {
        m_result.blobs = new TVABlob[m_result.Num];
        memcpy(m_result.blobs, result.blobs, m_result.Num*sizeof(TVABlob));
    }
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::RenderResult(awpImage* img)
{
    if (img == NULL || m_result.Num == 0)
        return;
    if (!viewDetectRectAction->Checked)
        return;
    m_engine.mtx->Acquire();
    for (int i = 0; i < m_result.Num; i++)
    {
        awpRect r;
        TVABlob* b = &m_result.blobs[i];
        r.left = b->XPos;
        r.top  = b->YPos;
        r.right = b->XPos + b->Width;
        r.bottom = b->YPos + b->Height;

        awpDrawRect(img, &r, 1, 255, 2);
    }
	m_engine.mtx->Release();
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::viewDetectRectActionExecute(TObject *Sender)
{
   viewDetectRectAction->Checked = !viewDetectRectAction->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::SetTrajectories(TVATrajectories* t)
{
    if (m_trj.Num > 0)
    {
        for (int i = 0; i < m_trj.Num; i++)
            delete m_trj.Trajectories[i].blobs;
        delete m_trj.Trajectories;
        m_trj.Num = 0;
        m_trj.Trajectories = NULL;
    }
    if (t == NULL || t->Num == 0)
        return;
    m_trj.Num = t->Num;
    m_trj.Trajectories = new TVATrajectory[t->Num];
    for (int i = 0; i < t->Num; i++)
    {
        m_trj.Trajectories[i].id = t->Trajectories[i].id;
        m_trj.Trajectories[i].Num = t->Trajectories[i].Num;
        m_trj.Trajectories[i].blobs = new TVABlob[t->Trajectories[i].Num];
        for (int j =0; j < t->Trajectories[i].Num; j++)
        {
            memcpy(&m_trj.Trajectories[i].blobs[j], &t->Trajectories[i].blobs[j], sizeof(TVABlob));
        }
    }
}

void __fastcall TmainForm::viewTrajectoriesActionExecute(TObject *Sender)
{
    viewTrajectoriesAction->Checked = !viewTrajectoriesAction->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::RenderTrajectories(awpImage* img)
{
    if (!viewTrajectoriesAction->Checked)
        return;
    if (img == NULL || m_trj.Num  == 0)
       return;
    m_engine.mtx->Acquire();
    try
    {
        for (int i = 0; i < m_trj.Num; i++)
        {
            awpPoint p0,p1;
            if (m_trj.Trajectories[i].blobs == NULL)
                continue;
            p0.X = m_trj.Trajectories[i].blobs[0].XPos + m_trj.Trajectories[i].blobs[0].Width / 2;
            p0.Y = m_trj.Trajectories[i].blobs[0].YPos + m_trj.Trajectories[i].blobs[0].Height / 2;
            for (int j = 1; j < m_trj.Trajectories[i].Num; j++)
            {
                p1.X = m_trj.Trajectories[i].blobs[j].XPos + m_trj.Trajectories[i].blobs[j].Width / 2;
                p1.Y = m_trj.Trajectories[i].blobs[j].YPos + m_trj.Trajectories[i].blobs[j].Height / 2;
                awpDrawLine(img, p0,p1,1,255,2);
                p0 = p1;
            }
        }
    }
    __finally
    {
	    m_engine.mtx->Release();
    }
}

void __fastcall TmainForm::UpdateResult()
{
    Label1->Caption = IntToStr(m_result.Num);
}


