//---------------------------------------------------------------------------

#ifndef mainUnitH
#define mainUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "FImage41.h"
#include "PhImageTool.h"
#include "PhPaneTool.h"
#include "PhTrackBar.h"
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.Menus.hpp>
#include "trackerUtils.h"
//---------------------------------------------------------------------------
class TPhMediaSource;
class TmainForm : public TForm
{
__published:	// IDE-managed Components
	TActionList *ActionList1;
	TAction *CloseAction;
	TAction *ModePaneAction;
	TAction *ModeSelRectAction;
	TAction *ActualSizeAction;
	TAction *BestFitAction;
	TAction *AboutAction;
	TAction *OpenVideoAction;
	TAction *CloseVideoAction;
	TAction *PlayAction;
	TAction *PrevFrameAction;
	TAction *NextFrameAction;
	TAction *LastFrameAction;
	TAction *logOpenAction;
	TAction *logSaveAction;
	TAction *logClearAction;
	TAction *logInsertAction;
	TAction *logDeleteAction;
	TAction *GotoFrameAction;
	TAction *modeAnalysisAction;
	TAction *modeTuningAction;
	TAction *viewZonesAction;
	TAction *viewDetectRectAction;
	TStatusBar *StatusBar1;
	TPanel *Panel1;
	TSpeedButton *SpeedButton2;
	TSpeedButton *SpeedButton3;
	TSpeedButton *SpeedButton9;
	TSpeedButton *SpeedButton4;
	TSpeedButton *SpeedButton5;
	TPhImage *FImage1;
	TPhTrackBar *PhTrackBar1;
	TPhPaneTool *PhPaneTool1;
	TSaveDialog *SaveDialog1;
	TMainMenu *MainMenu1;
	TMenuItem *N11;
	TMenuItem *N14;
	TMenuItem *N17;
	TMenuItem *N18;
	TMenuItem *USB1;
	TMenuItem *IP1;
	TMenuItem *N22;
	TMenuItem *N21;
	TMenuItem *N19;
	TMenuItem *N20;
	TMenuItem *N13;
	TMenuItem *N12;
	TMenuItem *N36;
	TMenuItem *N37;
	TMenuItem *N38;
	TMenuItem *N25;
	TMenuItem *FirstFrameAction1;
	TMenuItem *N26;
	TMenuItem *N27;
	TMenuItem *N28;
	TMenuItem *N29;
	TMenuItem *N30;
	TMenuItem *N31;
	TMenuItem *N32;
	TMenuItem *N33;
	TMenuItem *N34;
	TMenuItem *N35;
	TMenuItem *N15;
	TMenuItem *N16;
	TOpenDialog *OpenDialog1;
	TAction *FirstFrameAction;
	void __fastcall OpenVideoActionExecute(TObject *Sender);
	void __fastcall PlayActionExecute(TObject *Sender);
	void __fastcall PhTrackBar1Change(TObject *Sender);
	void __fastcall PhTrackBar1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall PhTrackBar1KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FImage1FrameData(TObject *Sender, int w, int h, int c, BYTE *data);
	void __fastcall CloseActionExecute(TObject *Sender);
	void __fastcall CloseVideoActionExecute(TObject *Sender);
	void __fastcall modeAnalysisActionExecute(TObject *Sender);
	void __fastcall modeAnalysisActionUpdate(TObject *Sender);
	void __fastcall modeTuningActionExecute(TObject *Sender);
	void __fastcall modeTuningActionUpdate(TObject *Sender);
	void __fastcall PlayActionUpdate(TObject *Sender);
	void __fastcall FirstFrameActionExecute(TObject *Sender);
	void __fastcall FirstFrameActionUpdate(TObject *Sender);
	void __fastcall PrevFrameActionExecute(TObject *Sender);
	void __fastcall PrevFrameActionUpdate(TObject *Sender);
	void __fastcall NextFrameActionExecute(TObject *Sender);
	void __fastcall NextFrameActionUpdate(TObject *Sender);
	void __fastcall LastFrameActionExecute(TObject *Sender);
	void __fastcall LastFrameActionUpdate(TObject *Sender);
	void __fastcall GotoFrameActionExecute(TObject *Sender);
	void __fastcall GotoFrameActionUpdate(TObject *Sender);


private:	// User declarations

    TPhMediaSource* m_videoSource;
    TAction*        m_modeAction;
    TTrackAnalysisEngine m_engine;

    awpImage* m_fg;

    void __fastcall SetSource(TPhMediaSource* source);
	void __fastcall SetMode(TAction* action);

    awpImage* __fastcall GetFg();
    void __fastcall SetFg(awpImage* fg);

    TTrackAnalysisEngine* __fastcall GetEngine();

    void __fastcall RenderFG(awpImage* img);
public:		// User declarations
	__fastcall TmainForm(TComponent* Owner);

    __property TTrackAnalysisEngine* engine = {read = GetEngine};
    __property TAction* Mode = {read = m_modeAction};
    __property awpImage* Foregroung = {read = GetFg, write = SetFg};
};
//---------------------------------------------------------------------------
extern PACKAGE TmainForm *mainForm;
//---------------------------------------------------------------------------
#endif
