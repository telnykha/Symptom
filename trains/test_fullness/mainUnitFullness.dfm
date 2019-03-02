object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = 'testFullness'
  ClientHeight = 412
  ClientWidth = 556
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object PhImage1: TPhImage
    Left = 0
    Top = 0
    Width = 556
    Height = 412
    ThumbWidht = 128
    ThumbHeight = 128
    SlideShowInterval = 500
    Align = alClient
    ParentColor = False
    PopupMenu = PopupMenu1
    AfterOpen = PhImage1AfterOpen
  end
  object PopupMenu1: TPopupMenu
    Left = 288
    Top = 208
    object Openimage1: TMenuItem
      Caption = 'Open image...'
      OnClick = Openimage1Click
    end
    object Closeimage1: TMenuItem
      Caption = 'Close image'
      OnClick = Closeimage1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Zoompanemode1: TMenuItem
      Action = zoompaneAction
    end
    object Fullnesstestmode1: TMenuItem
      Action = toolFullnessAction
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object Exit1: TMenuItem
      Caption = 'Exit'
      OnClick = Exit1Click
    end
  end
  object OpenDialog1: TOpenDialog
    Filter = 'jpeg images|*.jpg'
    Left = 344
    Top = 208
  end
  object PhPaneTool1: TPhPaneTool
    PhImage = PhImage1
    Left = 400
    Top = 208
  end
  object ActionList1: TActionList
    Left = 240
    Top = 208
    object zoompaneAction: TAction
      Caption = 'Zoom/pane mode'
      OnExecute = zoompaneActionExecute
      OnUpdate = zoompaneActionUpdate
    end
    object toolFullnessAction: TAction
      Caption = 'Fullness test mode'
      OnExecute = toolFullnessActionExecute
      OnUpdate = toolFullnessActionUpdate
    end
  end
end
