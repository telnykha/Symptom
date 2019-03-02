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
//		File: FullnessTool.cpp
//		Purpose: contains TPhFullnessTool class implementation
//
//      CopyRight 2015-2018 (c) NN-Videolab.net
//M*/
//---------------------------------------------------------------------------
#pragma hdrstop
#include "FullnessTool.h"
#include "Dialogs.hpp"
#include "..\..\include\va_common.h"
#include "..\..\include\trains.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Photon41.lib"
__fastcall TPhFullnessTool::TPhFullnessTool(TComponent* Owner) : TPhSelRectTool(Owner)
{
    m_pos = 0;
}
__fastcall TPhFullnessTool::~TPhFullnessTool()
{

}

void TPhFullnessTool::Draw(TCanvas* Canvas)
{
    inherited::Draw(Canvas);
    TRect rect0 = m_pImage->GetSelRect();
    TRect rect  = m_pImage->GetScreenRect(rect0);
    TPoint p = m_pImage->GetScreenPoint(rect0.Left, rect0.Top + m_pos);

    TColor oc = Canvas->Pen->Color;
    Canvas->Pen->Color = clLime;
    int pw = Canvas->Pen->Width;
    Canvas->Pen->Width = 3;

    Canvas->MoveTo(rect.Left, p.y);
    Canvas->LineTo(rect.Right, p.Y);

    Canvas->Pen->Color = oc;
    Canvas->Pen->Width = pw;
}

void TPhFullnessTool::MouseDown(int X, int Y, TMouseButton Button)
{
    inherited::MouseDown(X, Y, Button);
}

void TPhFullnessTool::MouseUp(int X, int Y, TMouseButton Button)
{
    inherited::MouseUp(X, Y, Button);
    ProcessSelection();
    m_pImage->Paint();
}

void TPhFullnessTool::MouseMove(int X, int Y, TShiftState Shift)
{
    inherited::MouseMove(X, Y, Shift);
}
void TPhFullnessTool::Reset()
{
    inherited::Reset();
}

void __fastcall TPhFullnessTool::ProcessSelection()
{
    FULLNESSHANDLE handle = NULL;
    TVAInitParams params;

    params.EventSens = 0.5;
    params.NumZones = 1;

    awpImage* img = NULL;

    this->m_pImage->GetAwpImage(&img);
    if (img == NULL)
    {
        ShowMessage(L"Cannot image for analysis.");
        return;
    }

    TRect rect = m_pImage->GetSelRect();

    params.Zones = new TVAZone;
    params.Zones[0].IsRect = true;
    params.Zones[0].Rect.LeftTop.X = 100.*(double)rect.Left / (double)img->sSizeX;
    params.Zones[0].Rect.LeftTop.Y = 100.*(double)rect.Top / (double)img->sSizeY;
    params.Zones[0].Rect.RightBottom.X = 100.*(double)rect.Right / (double)img->sSizeX;
    params.Zones[0].Rect.RightBottom.Y = 100.*(double)rect.Bottom / (double)img->sSizeY;


    handle = trainsFullnessInit(params);
    if (handle != NULL)
    {

          double d = 0;
          if (trainsFullnessProcess(handle, (unsigned char*)img->pPixels,
          img->sSizeX, img->sSizeY, img->bChannels, &d) != VA_OK)
          {
	        ShowMessage(L"Error processing image.");
          }
          else
          {
	         // ShowMessage(FloatToStr(d));
              m_pos = d*rect.Height()/100.;
          }
	      trainsFullnessClose(handle);
    }
    else
        ShowMessage(L"Cannot init fullness engine.");

    _AWP_SAFE_RELEASE_(img)
    delete params.Zones;
}
