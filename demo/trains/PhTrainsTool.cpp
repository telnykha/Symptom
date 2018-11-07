//---------------------------------------------------------------------------

#pragma hdrstop

#include "PhTrainsTool.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


__fastcall TPhTrainsTool::TPhTrainsTool(TComponent* Owner): TPhImageTool(Owner)
{
    memset(m_vertex, 0, sizeof(m_vertex));
    m_edit_vertex = -1;
    m_OnChange = NULL;
}
__fastcall TPhTrainsTool::~TPhTrainsTool()
{

}

void TPhTrainsTool::Draw(TCanvas* Canvas)
{
    if (Canvas == NULL)
        return;

    TColor oldColor =  Canvas->Pen->Color;
    TBrushStyle oldStyle = Canvas->Brush->Style;

    int delta = 5;

    int w = this->m_pImage->Bitmap->Width;
    int h = this->m_pImage->Bitmap->Height;

    int x1 = w*this->m_numberRect.LeftTop.X / 100.;
    int x2 = w*this->m_numberRect.RightBottom.X / 100.;
    int y1 = h*this->m_numberRect.LeftTop.Y / 100.;
    int y2 = h*this->m_numberRect.RightBottom.Y / 100.;

    Canvas->Pen->Color = clRed;
    Canvas->Brush->Color = clRed;

    TRect rect(x1,y1, x2, y2);
    TRect rect1 = this->m_pImage->GetScreenRect(rect);

    TPoint p1 = rect1.TopLeft();
    TPoint p2 = rect1.BottomRight();

	Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
    Canvas->Ellipse(p2.x-delta, p2.y - delta, p2.x + delta, p2.y + delta);


    p1.x = p2.x;
	Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
	p1 = rect1.TopLeft();
    p1.y = p2.y;
	Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);

    Canvas->Brush->Style = bsClear;
    Canvas->Rectangle(rect1);

    x1 = w*this->m_modelRect.LeftTop.X / 100.;
    x2 = w*this->m_modelRect.RightBottom.X / 100.;
    y1 = h*this->m_modelRect.LeftTop.Y / 100.;
    y2 = h*this->m_modelRect.RightBottom.Y / 100.;

    Canvas->Pen->Color = clLime;
    Canvas->Brush->Color = clLime;

    TRect rect2(x1,y1, x2, y2);
    TRect rect3 = this->m_pImage->GetScreenRect(rect2);


    p1 = rect3.TopLeft();
    p2 = rect3.BottomRight();

	Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
    Canvas->Ellipse(p2.x-delta, p2.y - delta, p2.x + delta, p2.y + delta);


    p1.x = p2.x;
	Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);
	p1 = rect3.TopLeft();
    p1.y = p2.y;
	Canvas->Ellipse(p1.x-delta, p1.y - delta, p1.x + delta, p1.y + delta);

    Canvas->Brush->Style = bsClear;
    Canvas->Rectangle(rect3);

    Canvas->Pen->Color = oldColor;
    Canvas->Brush->Color = oldColor;
    Canvas->Brush->Style = oldStyle;
}
void TPhTrainsTool::MouseDown(int X, int Y, TMouseButton Button)
{
     TPoint p(X,Y);
     IsPointNearVertex(p, true);

}
void TPhTrainsTool::MouseUp(int X, int Y, TMouseButton Button)
{
    if (this->m_edit_vertex >= 0)
    {
        // event here
        if (m_OnChange != NULL)
            this->m_OnChange(this);
        m_edit_vertex = -1;
    }

}
void TPhTrainsTool::MouseMove(int X, int Y, TShiftState Shift)
{
    TPoint p(X,Y);

    if (m_edit_vertex >= 0)
    {
        this->SetVertex(GetVAPoint(p), m_edit_vertex);
    }


    m_pImage->Cursor = crArrow;
    if (IsPointNearVertex(p))
	    m_pImage->Cursor = crHandPoint;
    else
    	m_pImage->Cursor = crArrow;
}
void TPhTrainsTool::Reset()
{

}

bool __fastcall TPhTrainsTool::IsPointNearVertex(TPoint p, bool edit)
{
    double x = this->m_pImage->GetImageX(p.x);
    double y = this->m_pImage->GetImageY(p.y);

    if (x > 0 && y > 0)
    {
        int w = this->m_pImage->Bitmap->Width;
        int h = this->m_pImage->Bitmap->Height;

        x = x*100. / w;
        y = y*100. / h;

        for (int i = 0; i < 8; i++)
        {
	        double d = _2D_Dist(x,y, m_vertex[i].X, m_vertex[i].Y);
            if (d < 1)
            {
                if (edit)
                    m_edit_vertex = i;
                return true;
            }

        }
    }
    return false;
}

double TPhTrainsTool::_2D_Dist(double x1,double y1,double x2,double y2)
{
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

void __fastcall TPhTrainsTool::SetModelRect(TVARect rect)
{
    TVAPoint p = rect.LeftTop;
    m_vertex[0] = p;
    p.X = rect.RightBottom.X;
    m_vertex[1] = p;
    p = rect.RightBottom;
    m_vertex[2] = p;
    p.X = rect.LeftTop.X;
    m_vertex[3] = p;

    m_modelRect = rect;
}

void __fastcall TPhTrainsTool::SetNumberRect(TVARect rect)
{
    TVAPoint p = rect.LeftTop;
    m_vertex[4] = p;
    p.X = rect.RightBottom.X;
    m_vertex[5] = p;
    p = rect.RightBottom;
    m_vertex[6] = p;
    p.X = rect.LeftTop.X;
    m_vertex[7] = p;

    m_numberRect = rect;
}

void __fastcall TPhTrainsTool::SetVertex(TVAPoint p, int idx)
{
    if (idx < 0 || idx > 7)
        return;
    m_vertex[idx] = p;
    switch(idx)
    {
        case 0:
           m_modelRect.LeftTop = p;
        break;
        case 1:
           m_modelRect.LeftTop.Y = p.Y;
           m_modelRect.RightBottom.X = p.X;
        break;
        case 2:
           m_modelRect.RightBottom = p;
        break;
        case 3:
           m_modelRect.LeftTop.X = p.X;
           m_modelRect.RightBottom.Y = p.Y;
        break;
        case 4:
            m_numberRect.LeftTop = p;
        break;
        case 5:
           m_numberRect.LeftTop.Y = p.Y;
           m_numberRect.RightBottom.X = p.X;
        break;
        case 6:
           m_numberRect.RightBottom = p;
        break;
        case 7:
           m_numberRect.LeftTop.X = p.X;
           m_numberRect.RightBottom.Y = p.Y;
        break;
    }
    SetModelRect(m_modelRect);
    SetNumberRect(m_numberRect);
    this->m_pImage->Paint();
}

TVAPoint __fastcall TPhTrainsTool::GetVAPoint(TPoint p)
{
    double x = this->m_pImage->GetImageX(p.x);
    double y = this->m_pImage->GetImageY(p.y);
    int w = this->m_pImage->Bitmap->Width;
    int h = this->m_pImage->Bitmap->Height;
    TVAPoint res;
    res.X = x*100. / w;
    res.Y = y*100. / h;
    return res;
}

