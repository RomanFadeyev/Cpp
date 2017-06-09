object fmMain: TfmMain
  Left = 313
  Top = 196
  BorderStyle = bsDialog
  Caption = 'Example of using MultiTranslator via OLE Automation'
  ClientHeight = 279
  ClientWidth = 517
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel1: TBevel
    Left = 0
    Top = 0
    Width = 517
    Height = 41
    Align = alTop
    Shape = bsBottomLine
  end
  object Bevel2: TBevel
    Left = 0
    Top = 240
    Width = 517
    Height = 39
    Align = alBottom
    Shape = bsTopLine
  end
  object buStart: TButton
    Left = 8
    Top = 8
    Width = 89
    Height = 25
    Caption = 'Start Example '
    TabOrder = 0
    OnClick = buStartClick
  end
  object lbReport: TListBox
    Left = 8
    Top = 48
    Width = 505
    Height = 185
    ItemHeight = 13
    TabOrder = 1
  end
  object buClose: TButton
    Left = 424
    Top = 250
    Width = 89
    Height = 23
    Caption = 'Close '
    TabOrder = 2
    OnClick = buCloseClick
  end
  object buNext: TButton
    Left = 104
    Top = 8
    Width = 89
    Height = 25
    Caption = 'Perform'
    Enabled = False
    TabOrder = 3
    OnClick = buNextClick
  end
  object dlgOpen: TOpenDialog
    Filter = 'MultiTranslator Project Files (*.pjt)|*.pjt'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 248
    Top = 8
  end
end
