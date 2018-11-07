//---------------------------------------------------------------------------

#ifndef TrainNumberProcessH
#define TrainNumberProcessH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class TTrainsAnalysisEngine;
class TTrainNumberProcess : public TThread
{
private:
	void __fastcall UpdateStatus();
protected:
	void __fastcall Execute();
public:
	__fastcall TTrainNumberProcess(bool CreateSuspended);
    TTrainsAnalysisEngine* m_engine;
};
//---------------------------------------------------------------------------
#endif
