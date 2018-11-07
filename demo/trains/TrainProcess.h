//---------------------------------------------------------------------------

#ifndef TrainProcessH
#define TrainProcessH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class TTrainsAnalysisEngine;
class TTrainProcess : public TThread
{
private:

	void __fastcall UpdateStatus();
protected:
	void __fastcall Execute();
public:
	__fastcall TTrainProcess(bool CreateSuspended);
    TTrainsAnalysisEngine* m_engine;
};
//---------------------------------------------------------------------------
#endif
