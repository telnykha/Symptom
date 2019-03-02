//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "mainUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FImage41"
#pragma link "PhImageTool"
#pragma link "PhPaneTool"
#pragma resource "*.dfm"
TForm5 *Form5;
//---------------------------------------------------------------------------
__fastcall TForm5::TForm5(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm5::Exit1Click(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm5::Openimage1Click(TObject *Sender)
{
    if (OpenDialog1->Execute())
    {
        PhImage1->Init(OpenDialog1->Files);
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm5::PhImage1AfterOpen(TObject *Sender)
{
    PhImage1->BestFit();
}
//---------------------------------------------------------------------------
void __fastcall TForm5::Closeimage1Click(TObject *Sender)
{
   PhImage1->Close();
   PhImage1->Paint();
}
//---------------------------------------------------------------------------
