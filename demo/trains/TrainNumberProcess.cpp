//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop

#include "TrainNumberProcess.h"
#include "TrainsUtils.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TTrainNumberProcess::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TTrainNumberProcess::TTrainNumberProcess(bool CreateSuspended)
	: TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall TTrainNumberProcess::Execute()
{
    awpImage* image = NULL;
	//---- Place thread code here ----
    while(!this->Terminated)
    {
       awpImage* check = m_engine->GetImage();
       if ( check == NULL)
        continue;

        // lock this operation
        awpCopyImage(check, &image);
        _AWP_SAFE_RELEASE_(check)
        if (m_engine->ProcessNumber(image))
	        Synchronize(&UpdateStatus);
    }

}
//---------------------------------------------------------------------------
void __fastcall TTrainNumberProcess::UpdateStatus()
{
     this->m_engine->UpdateStatus();

}
