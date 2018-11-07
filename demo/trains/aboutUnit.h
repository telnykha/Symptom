//----------------------------------------------------------------------------
#ifndef aboutUnitH
#define aboutUnitH
//----------------------------------------------------------------------------
#include <System.hpp>
#include <Windows.hpp>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Graphics.hpp>
#include <Forms.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>
//----------------------------------------------------------------------------
class TAboutBox : public TForm
{
__published:
	TPanel *Panel1;
	TLabel *ProductName;
	TButton *OKButton;
	TLabel *Copyright;
	TLabel *Version;
	TImage *Image1;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Comments;
private:
public:
	virtual __fastcall TAboutBox(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TAboutBox *AboutBox;
//----------------------------------------------------------------------------
#endif    
