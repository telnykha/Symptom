//---------------------------------------------------------------------------

#ifndef trackProcessH
#define trackProcessH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class TTrackAnalysisEngine;
class TTrackProcess : public TThread
{
private:
	void __fastcall UpdateStatus();
protected:
	void __fastcall Execute();
public:
	__fastcall TTrackProcess(bool CreateSuspended);
    TTrackAnalysisEngine* m_engine;
};
//---------------------------------------------------------------------------
#endif
