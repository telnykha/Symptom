//---------------------------------------------------------------------------

#ifndef trackerUtilsH
#define trackerUtilsH
//---------------------------------------------------------------------------
#include <System.SyncObjs.hpp>
#include "track.h"
#include "vautils.h"
#include "awpipl.h"
#include "_LF.h"
#include "trackProcess.h"

class TTrackAnalysisEngine
{
private:

    TVAInitParams* m_params;
    HANDLE    m_module;
    awpImage* m_pImage;
    awpImage* m_fg;
   	TMutex*   mutex;
    TTrackProcess* m_process;
protected:
    TVAInitParams* __fastcall GetParams();

    bool __fastcall CreateDefaultParams(const char* lpFileName);

public:

    TTrackAnalysisEngine();
    ~TTrackAnalysisEngine();

    // загружает параметры конфигурации из файла
    // и инициализирует модуль
    bool __fastcall Init(const char* path);
    // выполняет обработку изображения
    bool __fastcall Process(awpImage* img);
    // выгружает  модуль
    void __fastcall Reset();

    awpImage* GetImage();
    void SetImage(awpImage* image);
    awpImage* GetForeground();

public:
    void __fastcall UpdateStatus();
    __property TVAInitParams* params = {read = GetParams};
};

#endif
