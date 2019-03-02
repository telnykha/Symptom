//---------------------------------------------------------------------------

#ifndef mainUnitH
#define mainUnitH
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
//---------------------------------------------------------------------------
class TForm5 : public TForm
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
	void __fastcall Exit1Click(TObject *Sender);
	void __fastcall Openimage1Click(TObject *Sender);
	void __fastcall PhImage1AfterOpen(TObject *Sender);
	void __fastcall Closeimage1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm5(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm5 *Form5;
//---------------------------------------------------------------------------
#endif
