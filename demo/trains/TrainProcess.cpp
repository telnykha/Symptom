//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop

#include "TrainProcess.h"
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
//      void __fastcall TTrainProcess::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TTrainProcess::TTrainProcess(bool CreateSuspended)
	: TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall TTrainProcess::Execute()
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
        if (m_engine->ProcessTarget(image))
	        Synchronize(&UpdateStatus);
    }
}
//---------------------------------------------------------------------------
void __fastcall TTrainProcess::UpdateStatus()
{
     this->m_engine->UpdateStatus();
}
