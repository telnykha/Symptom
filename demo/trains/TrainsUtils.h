//---------------------------------------------------------------------------
#ifndef TrainsUtilsH
#define TrainsUtilsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.SyncObjs.hpp>
#include "trains.h"
#include "awpipl.h"
#include "_LF.h"
#include "TrainProcess.h"
#include "TrainNumberProcess.h"


class TTrainsAnalysisEngine
{
private:
    TRAINSHANDLE m_trains;
    TARGETHANDLE m_target;

    TVAInitParams m_trains_params;
    TVAInitParams m_target_params;

    TTrainProcess* m_processTarget;
    TTrainNumberProcess* m_processNumber;

    awpImage* m_pImage;
   	TMutex*   mutex;
    double    m_norm;
    double    m_old_norm;
    bool      m_rect_visible;
    TVARect   m_rect;
    DWORD     m_old_time;
    AnsiString m_strNum;
protected:
    int count;
public:

    TTrainsAnalysisEngine();
    ~TTrainsAnalysisEngine();

    bool __fastcall Init(TVAInitParams* trains, TVAInitParams* target);
    bool __fastcall ProcessTarget(awpImage* img);
    bool __fastcall ProcessNumber(awpImage* img);
    void __fastcall Reset();
    bool __fastcall CreateModel(awpImage* img);
    bool __fastcall ClearModel();
    bool __fastcall IsModelAvailable();

    awpImage* GetImage();
    void SetImage(awpImage* image);
public:
    void __fastcall UpdateStatus();
};
#endif
