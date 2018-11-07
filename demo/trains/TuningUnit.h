//---------------------------------------------------------------------------

#ifndef TuningUnitH
#define TuningUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "cspin.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.AppEvnts.hpp>
#include "PhTrackBar.h"
//---------------------------------------------------------------------------
class TTuningForm : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TCheckBox *CheckBox1;
	TGroupBox *GroupBox2;
	TCheckBox *CheckBox2;
	TLabel *Label1;
	TEdit *Edit1;
	TLabel *Label2;
	TCSpinEdit *CSpinEdit1;
	TEdit *Edit2;
	TLabel *Label3;
	TButton *Button1;
	TLabel *Label4;
	TApplicationEvents *ApplicationEvents1;
	TPhTrackBar *PhTrackBar1;
	TPhTrackBar *PhTrackBar2;
	TButton *Button3;
	void __fastcall ApplicationEvents1Idle(TObject *Sender, bool &Done);
	void __fastcall CheckBox1Click(TObject *Sender);
	void __fastcall CheckBox2Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall PhTrackBar1Change(TObject *Sender);
	void __fastcall PhTrackBar1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall PhTrackBar2Change(TObject *Sender);
	void __fastcall PhTrackBar2MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall Button3Click(TObject *Sender);
private:	// User declarations
    bool m_ParamsEdited;
    bool __fastcall GetIsModelAvailable();
public:		// User declarations
	__fastcall TTuningForm(TComponent* Owner);
    __property bool IsModelAvailable = {read = GetIsModelAvailable};
    __property bool IsParamsEdited = {read = m_ParamsEdited, write = m_ParamsEdited};
};
//---------------------------------------------------------------------------
extern PACKAGE TTuningForm *TuningForm;
//---------------------------------------------------------------------------
#endif
