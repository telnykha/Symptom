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
//		Purpose: contains TMainForm class description
//
//      CopyRight 2015-2018 (c) NN-Videolab.net
//M*/
//---------------------------------------------------------------------------
#ifndef mainUnitFullnessH
#define mainUnitFullnessH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "FImage41.h"
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>
#include "PhImageTool.h"
#include "PhPaneTool.h"
#include "FullnessTool.h"
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TPhImage *PhImage1;
	TPopupMenu *PopupMenu1;
	TMenuItem *Openimage1;
	TMenuItem *N1;
	TMenuItem *Exit1;
	TOpenDialog *OpenDialog1;
	TMenuItem *Zoompanemode1;
	TMenuItem *Fullnesstestmode1;
	TMenuItem *N2;
	TMenuItem *Closeimage1;
	TPhPaneTool *PhPaneTool1;
	TActionList *ActionList1;
	TAction *zoompaneAction;
	TAction *toolFullnessAction;
	void __fastcall Exit1Click(TObject *Sender);
	void __fastcall Openimage1Click(TObject *Sender);
	void __fastcall PhImage1AfterOpen(TObject *Sender);
	void __fastcall Closeimage1Click(TObject *Sender);
	void __fastcall zoompaneActionExecute(TObject *Sender);
	void __fastcall zoompaneActionUpdate(TObject *Sender);
	void __fastcall toolFullnessActionExecute(TObject *Sender);
	void __fastcall toolFullnessActionUpdate(TObject *Sender);
private:	// User declarations
    TPhFullnessTool* m_pFullnessTool;
    TAction*         m_modeAction;
    void __fastcall SetMode(TAction* action);
public:		// User declarations
	__fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
