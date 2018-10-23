//---------------------------------------------------------------------------

#pragma hdrstop

#include "trackerUtils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

TTrackAnalysisEngine::TTrackAnalysisEngine()
{

}


TTrackAnalysisEngine::~TTrackAnalysisEngine()
{
    Reset();
    delete mutex;
}

bool __fastcall TTrackAnalysisEngine::Init(TVAInitParams* params)
{
    return true;
}
bool __fastcall TTrackAnalysisEngine::Process(awpImage* img)
{
    return true;
}
void __fastcall TTrackAnalysisEngine::Reset()
{

}

awpImage* TTrackAnalysisEngine::GetImage()
{
    return NULL;
}
void TTrackAnalysisEngine::SetImage(awpImage* image)
{

}
void __fastcall TTrackAnalysisEngine::UpdateStatus()
{

}
