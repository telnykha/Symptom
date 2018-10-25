//---------------------------------------------------------------------------
#pragma hdrstop
#include <string>
#include "trackerUtils.h"
#include "mainUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

#pragma link "track.lib"
#pragma link "vautils.lib"


using namespace std;

TTrackAnalysisEngine::TTrackAnalysisEngine()
{
      m_params = NULL;
      m_module = NULL;
      mutex = new TMutex(true);
      m_process = NULL;
      m_fg = NULL;
}

TTrackAnalysisEngine::~TTrackAnalysisEngine()
{
    Reset();
    delete mutex;
}

bool __fastcall TTrackAnalysisEngine::Init(const char* path)
{
    Reset();

    bool res = false;
    if (path == NULL)
        return false;

    string str = path;
    str = LFChangeFileExt(str, ".track");
    if (!LFFileExists(str))
    {
        res = this->CreateDefaultParams(str.c_str());
        if (!res)
            return false;
    }
    if (!LoadInitParams(str.c_str(), &m_params))
        return false;

    m_module = trackCreate(m_params);

    m_process = new TTrackProcess(true);
    m_process->m_engine = this;
    m_process->Start();


    return true;
}
bool __fastcall TTrackAnalysisEngine::Process(awpImage* img)
{
    if (img != NULL && m_module != NULL && mainForm->Mode != NULL && mainForm->Mode == mainForm->modeAnalysisAction)
    {
            TVAResult result;
            result.Num = 100;
            result.blobs = new TVABlob[100];
            trackProcess(m_module, img->sSizeX, img->sSizeY, img->bChannels, (unsigned char*)img->pPixels, &result);
            awpImage* fg = NULL;
            awpCreateImage(&fg, img->sSizeX, img->sSizeY, 1, AWP_BYTE);
            trackForeground(m_module, fg->sSizeX, fg->sSizeY, (unsigned char*)fg->pPixels);

            mutex->Acquire();

            _AWP_SAFE_RELEASE_(m_fg)
            awpCopyImage(fg, &m_fg);

            mutex->Release();

            awpReleaseImage(&fg);
            delete result.blobs;
            return true;
    }
    return true;
}


void __fastcall TTrackAnalysisEngine::Reset()
{
    if (m_process != NULL)
    {
       m_process->Terminate();
    }

    if (m_module != NULL)
    {
	    trackRelease(&m_module);
        m_module = NULL;
    }
    if (m_params != NULL)
    {
        FreeParams(&m_params);
        m_params = NULL;
    }
    UpdateStatus();
}

awpImage* TTrackAnalysisEngine::GetImage()
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

awpImage* TTrackAnalysisEngine::GetForeground()
{
    if (m_fg == NULL)
        return NULL;
    mutex->Acquire();
    awpImage* res = NULL;
    awpCopyImage(m_fg, &res);
    _AWP_SAFE_RELEASE_(m_fg)
    mutex->Release();
    return res;
}


void TTrackAnalysisEngine::SetImage(awpImage* image)
{
    mutex->Acquire();
    awpCopyImage(image, &m_pImage);
    mutex->Release();
}

void __fastcall TTrackAnalysisEngine::UpdateStatus()
{
    mainForm->Foregroung = GetForeground();
}

TVAInitParams* __fastcall TTrackAnalysisEngine::GetParams()
{
    return m_params;
}

// создает параметры по умолчанию и записывает их в файл
bool __fastcall TTrackAnalysisEngine::CreateDefaultParams(const char* lpFileName)
{
    TVAInitParams p;

    p.SaveLog = false;
    p.Path    = "";
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

    return SaveInitParams(lpFileName, &p);

}

