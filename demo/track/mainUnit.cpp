//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "mainUnit.h"
#include "PhVideo.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
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
bool __fastcall TmainForm::InitParams()
{
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TmainForm::SetSource(TPhMediaSource* source)
{
 //  m_engine.Reset();

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

}
//---------------------------------------------------------------------------

