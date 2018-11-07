//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "TrainsUtils.h"
#include "trainsMainFormUnit.h"
#include "TuningUnit.h"
//---------------------------------------------------------------------------

TTrainsAnalysisEngine::TTrainsAnalysisEngine()
{
    m_trains = NULL;
    m_target = NULL;
    m_processTarget = NULL;
    m_processNumber = NULL;
    m_pImage = NULL;
    mutex = new TMutex(true);
    count = 0;
    m_old_norm = -1;
    m_rect_visible= false;
    m_old_time = -1;
}

TTrainsAnalysisEngine::~TTrainsAnalysisEngine()
{
    Reset();
    delete mutex;
}

bool __fastcall TTrainsAnalysisEngine::Init(TVAInitParams* trains, TVAInitParams* target)
{
      Reset();
      if (TuningForm->CheckBox1->Checked)
      {
	      m_trains = trainsInit(trains->Zones[0].Rect, trains->EventSens, 0);
          m_processNumber = new TTrainNumberProcess(true);
          m_processNumber->m_engine = this;
          m_processNumber->Start();
      }

      if (TuningForm->CheckBox2->Checked)
      {
          m_target = trainsTargetInit(*target);
          m_processTarget = new TTrainProcess(true);
          m_processTarget->m_engine = this;
          m_processTarget->Start();
      }

}

bool __fastcall TTrainsAnalysisEngine::ProcessTarget(awpImage* img)
{
    if (img != NULL && m_target != NULL && mainForm->Mode != NULL && mainForm->Mode == mainForm->modeAnalysisAction)
    {
            double d;
            bool b;
            trainsTargetProcess(m_target, (unsigned char*)img->pPixels, img->sSizeX, img->sSizeY, img->bChannels, &d, &b);
            m_norm = d;
            return true;
    }

    return false;
}

bool __fastcall TTrainsAnalysisEngine::ProcessNumber(awpImage* img)
{
    if (img != NULL && m_trains != NULL && mainForm->Mode != NULL && mainForm->Mode == mainForm->modeAnalysisAction)
    {
            char number[10];
            TVARect rect;
            m_strNum = "########";
            int res = trainsProcess(m_trains, (unsigned char*)img->pPixels, img->sSizeX, img->sSizeY, img->bChannels, number, rect);
            m_rect_visible = res == 0;
            m_rect = rect;
            m_strNum = number;
            return true;
    }

    return false;
}


bool __fastcall TTrainsAnalysisEngine::CreateModel(awpImage* img)
{
    if (m_target == NULL)
    {
        ShowMessage("Не могу создать модель. Модуль не инициализирован.");
        return false;
    }

    int res = trainsTargetCreate(m_target, (unsigned char*)img->pPixels, img->sSizeX, img->sSizeY, img->bChannels);
    if (res != VA_OK)
    {
        ShowMessage("Не могу создать модель.");
    }

    return res == VA_OK;
}


void __fastcall TTrainsAnalysisEngine::Reset()
{
    if (m_processTarget != NULL)
    {
        m_processTarget->Terminate();
    }

    if (m_processNumber != NULL)
    {
        m_processNumber->Terminate();
    }

    if (m_trains != NULL)
	    trainsClose(m_trains);
    if (m_target != NULL)
	    trainsTargetClose(m_target);
    m_old_norm = -1;
    m_old_time = -1;
    count = 0;
    m_rect_visible= false;
    UpdateStatus();
}


awpImage* TTrainsAnalysisEngine::GetImage()
{
    if (m_pImage == NULL)
    	return NULL;
    mutex->Acquire();
    awpImage* res = NULL;
    awpCopyImage(m_pImage, &res);
    _AWP_SAFE_RELEASE_(m_pImage)
    mutex->Release();
    return res;
}

void TTrainsAnalysisEngine::SetImage(awpImage* image)
{
    mutex->Acquire();
    awpCopyImage(image, &m_pImage);
    mutex->Release();
}

void __fastcall TTrainsAnalysisEngine::UpdateStatus()
{
    mainForm->Panel2->Color = m_norm > 0.70 ? clLime : clRed;
    mainForm->Label3->Font->Color = m_rect_visible ? clBlue : clGray;
    mainForm->ProgressBar1->Position = int(m_norm*100.);

    if (m_old_time == -1)
       mainForm->Label4->Caption = FormatFloat("000.00", 0) + L" км/ч";


    if (m_old_time != -1)
    {
        DWORD t = GetTickCount();
        DWORD dt = t - m_old_time;

        if (dt > 60000)
        {
            m_old_time = -1;
            mainForm->Label4->Caption = FormatFloat("000.00", 0) + L" км/ч";
        }

    }


    if (m_old_norm != -1)
    {
        if (m_old_norm < 0.70 && m_norm > 0.70)
        {
            count++;
            DWORD t = GetTickCount();
            if (m_old_time == -1)
            {
                m_old_time = t;
            }
            else
            {
                DWORD dt = t - m_old_time;

                double v = 13. / (double)dt;
                v *= 3600.;
                mainForm->Label4->Caption = FormatFloat("000.00", v) + L" км/ч";
                m_old_time = t;
            }
        }
    }
    mainForm->Label2->Caption = IntToStr(count);
    m_old_norm = m_norm;
    mainForm->rect_visible = m_rect_visible;
    mainForm->nrect = this->m_rect;
    mainForm->Label3->Caption = this->m_strNum;
}

bool __fastcall TTrainsAnalysisEngine::ClearModel()
{
    if (m_target != NULL)
        trainsTargetClearModel(m_target);
}

bool __fastcall TTrainsAnalysisEngine::IsModelAvailable()
{
    if (m_target != NULL)
    {
        bool res = false;
        trainsTargetHasModel(m_target , &res);
        return res;
    }
    return false;
}


#pragma package(smart_init)
