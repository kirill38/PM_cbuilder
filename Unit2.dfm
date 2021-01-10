object YRange: TYRange
  Left = 886
  Top = 289
  Width = 178
  Height = 289
  Caption = 'Y Range'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 17
    Height = 13
    Caption = 'Min'
  end
  object Label2: TLabel
    Left = 88
    Top = 8
    Width = 20
    Height = 13
    Caption = 'Max'
  end
  object Label3: TLabel
    Left = 8
    Top = 48
    Width = 36
    Height = 13
    Caption = 'Legend'
  end
  object Edit1: TEdit
    Left = 8
    Top = 24
    Width = 73
    Height = 21
    TabOrder = 0
    Text = '0'
    OnChange = Edit1Change
  end
  object Edit2: TEdit
    Left = 88
    Top = 24
    Width = 73
    Height = 21
    TabOrder = 1
    Text = '0'
    OnChange = Edit2Change
  end
  object LegendLB: TListBox
    Left = 8
    Top = 64
    Width = 153
    Height = 161
    Style = lbOwnerDrawFixed
    Color = clBlack
    ItemHeight = 15
    Items.Strings = (
      'M_____CH__ADDR__P__X_Y'
      '21.239 12 200 106 0 3'
      '1'
      '2'
      '3'
      '')
    TabOrder = 2
    OnClick = LegendLBClick
    OnDrawItem = LegendLBDrawItem
  end
  object Button1: TButton
    Left = 8
    Top = 232
    Width = 75
    Height = 25
    Caption = 'Button1'
    TabOrder = 3
    OnClick = Button1Click
  end
end
