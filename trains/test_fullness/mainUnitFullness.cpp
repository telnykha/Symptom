/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                     Symptom Video Analysis System
//
// Copyright (C) 2015-2018, NN-Videolab.net, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the NN-Videolab.net or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//      Symptom Video Analysis System
//		File: mainUnitFullness.cpp
//		Purpose: contains TMainForm class implementation
//
//      CopyRight 2015-2018 (c) NN-Videolab.net
//M*/
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "mainUnitFullness.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FImage41"
#pragma link "PhImageTool"
#pragma link "PhPaneTool"
#pragma link "PhSelectRectTool"
#pragma link "trains.lib"

#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
    m_pFullnessTool = NULL;
    m_modeAction    = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Exit1Click(TObject *Sender)
{
    if (m_pFullnessTool != NULL)
        delete m_pFullnessTool;
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Openimage1Click(TObject *Sender)
{
    if (OpenDialog1->Execute())
    {
        PhImage1->Init(OpenDialog1->Files);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PhImage1AfterOpen(TObject *Sender)
{
    PhImage1->BestFit();
    SetMode(zoompaneAction);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Closeimage1Click(TObject *Sender)
{
   PhImage1->Close();
   PhImage1->Paint();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetMode(TAction* action)
{
    if (action == zoompaneAction)
    {
        m_modeAction = zoompaneAction;
        PhImage1->ClearSelection();
        PhImage1->SelectPhTool(this->PhPaneTool1);
    }
    else if (action == toolFullnessAction)
    {
        m_modeAction = toolFullnessAction;
        if (m_pFullnessTool)
            delete m_pFullnessTool;
        m_pFullnessTool = new TPhFullnessTool(NULL);
        m_pFullnessTool->PhImage = this->PhImage1;
        PhImage1->SelectPhTool(m_pFullnessTool);

    }
}
void __fastcall TMainForm::zoompaneActionExecute(TObject *Sender)
{
    SetMode(zoompaneAction);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::zoompaneActionUpdate(TObject *Sender)
{
    zoompaneAction->Enabled = !PhImage1->Bitmap->Empty;
    zoompaneAction->Checked = m_modeAction == zoompaneAction;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::toolFullnessActionExecute(TObject *Sender)
{
    SetMode(toolFullnessAction);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::toolFullnessActionUpdate(TObject *Sender)
{
       toolFullnessAction->Enabled = !PhImage1->Bitmap->Empty;
       toolFullnessAction->Checked = m_modeAction == toolFullnessAction;
}
//---------------------------------------------------------------------------
