//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop

#include "trackProcess.h"
#include "trackerUtils.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TTrackProcess::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TTrackProcess::TTrackProcess(bool CreateSuspended)
	: TThread(CreateSuspended)
{
    m_engine = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TTrackProcess::Execute()
{
	//---- Place thread code here ----
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
        if (m_engine->Process(image))
	        Synchronize(&UpdateStatus);
    }
}
//---------------------------------------------------------------------------
void __fastcall TTrackProcess::UpdateStatus()
{
   if (m_engine != NULL)
	   m_engine->UpdateStatus();
}
