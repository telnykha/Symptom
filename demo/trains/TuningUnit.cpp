//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "TuningUnit.h"
#include "trainsMainFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cspin"
#pragma link "PhTrackBar"
#pragma resource "*.dfm"
TTuningForm *TuningForm;
//---------------------------------------------------------------------------
__fastcall TTuningForm::TTuningForm(TComponent* Owner)
	: TForm(Owner)
{
        m_ParamsEdited     = false;
}
//---------------------------------------------------------------------------
void __fastcall TTuningForm::ApplicationEvents1Idle(TObject *Sender, bool &Done)
{
      Button3->Enabled = m_ParamsEdited;
      Label4->Caption =  IsModelAvailable ? L"Модель мишени готова к использованию" : L"Модель мишени не существует";
      Label4->Font->Color = IsModelAvailable ? clBlue : clRed;
}
//---------------------------------------------------------------------------


void __fastcall TTuningForm::CheckBox1Click(TObject *Sender)
{
    GroupBox1->Enabled = CheckBox1->Checked;

    for (int i = 0; i < GroupBox1->ControlCount; i++)
    {
        TControl* c = GroupBox1->Controls[i];
        if (c != NULL)
            c->Enabled = CheckBox1->Checked;
    }

}
//---------------------------------------------------------------------------

void __fastcall TTuningForm::CheckBox2Click(TObject *Sender)
{
    GroupBox2->Enabled = CheckBox2->Checked;

    for (int i = 0; i < GroupBox2->ControlCount; i++)
    {
        TControl* c = GroupBox2->Controls[i];
        if (c != NULL)
            c->Enabled = CheckBox2->Checked;
    }
}
//---------------------------------------------------------------------------
void __fastcall TTuningForm::Button1Click(TObject *Sender)
{
    if (mainForm->targetParams != NULL)
    {
    	if (!mainForm->CreateModel())
	        ShowMessage("Не могу обновить модель!");
    }
    else
        ShowMessage("Не могу создать модель!");
}
//---------------------------------------------------------------------------

//
void __fastcall TTuningForm::FormShow(TObject *Sender)
{
    if (mainForm->trainsParams != NULL)
    {
          PhTrackBar1->Position = int(100.*mainForm->trainsParams->EventSens);
    }
}
//---------------------------------------------------------------------------

void __fastcall TTuningForm::PhTrackBar1Change(TObject *Sender)
{
	Edit1->Text = IntToStr(PhTrackBar1->Position);
}
//---------------------------------------------------------------------------

void __fastcall TTuningForm::PhTrackBar1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    double value = (double)PhTrackBar1->Position / 100.;

    if (mainForm->trainsParams != NULL)
    {
	    mainForm->trainsParams->EventSens = value;
        m_ParamsEdited = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TTuningForm::PhTrackBar2Change(TObject *Sender)
{
	Edit2->Text = IntToStr(PhTrackBar2->Position);
}
//---------------------------------------------------------------------------

void __fastcall TTuningForm::PhTrackBar2MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    double value = (double)PhTrackBar2->Position / 100.;
    if (mainForm->targetParams != NULL)
    {
	    mainForm->targetParams->EventSens = value;
        m_ParamsEdited = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TTuningForm::Button3Click(TObject *Sender)
{
    mainForm->UpdateParams();
    m_ParamsEdited = false;
}
//---------------------------------------------------------------------------
bool __fastcall TTuningForm::GetIsModelAvailable()
{
    if (mainForm->engine == NULL)
        return false;

    return mainForm->engine->IsModelAvailable();
}

