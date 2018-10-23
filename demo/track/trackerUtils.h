//---------------------------------------------------------------------------

#ifndef trackerUtilsH
#define trackerUtilsH
//---------------------------------------------------------------------------
#include <System.SyncObjs.hpp>
#include "track.h"
#include "awpipl.h"
#include "_LF.h"

class TTrackAnalysisEngine
{
private:

    TVAInitParams m_params;

//    TTrainProcess* m_processTarget;
//    TTrainNumberProcess* m_processNumber;

    awpImage* m_pImage;
   	TMutex*   mutex;
    double    m_norm;
    double    m_old_norm;
    bool      m_rect_visible;
    TVARect   m_rect;
    DWORD     m_old_time;
protected:
    int count;
public:

    TTrackAnalysisEngine();
    ~TTrackAnalysisEngine();

    bool __fastcall Init(TVAInitParams* params);
    bool __fastcall Process(awpImage* img);
    void __fastcall Reset();

    awpImage* GetImage();
    void SetImage(awpImage* image);
public:
    void __fastcall UpdateStatus();
};

#endif
