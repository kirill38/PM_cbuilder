object Form1: TForm1
  Left = 181
  Top = 183
  Width = 285
  Height = 302
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object leCurrChan: TLabeledEdit
    Left = 8
    Top = 24
    Width = 81
    Height = 21
    EditLabel.Width = 70
    EditLabel.Height = 13
    EditLabel.Caption = 'Current Chanel'
    LabelPosition = lpAbove
    LabelSpacing = 3
    TabOrder = 0
  end
  object Button1: TButton
    Left = 8
    Top = 56
    Width = 49
    Height = 25
    Caption = 'Button1'
    TabOrder = 1
  end
  object CheckBox1: TCheckBox
    Left = 104
    Top = 24
    Width = 97
    Height = 17
    Caption = 'CheckBox1'
    TabOrder = 2
  end
  object lbChannels: TListBox
    Left = 8
    Top = 88
    Width = 121
    Height = 153
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Console'
    Font.Style = []
    ItemHeight = 11
    ParentFont = False
    TabOrder = 3
  end
  object Button2: TButton
    Left = 64
    Top = 56
    Width = 65
    Height = 25
    Caption = 'SetCurrent'
    TabOrder = 4
  end
  object BitBtn1: TBitBtn
    Left = 160
    Top = 80
    Width = 75
    Height = 25
    Caption = '&Close'
    TabOrder = 5
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00388888888877
      F7F787F8888888888333333F00004444400888FFF444448888888888F333FF8F
      000033334D5007FFF4333388888888883338888F0000333345D50FFFF4333333
      338F888F3338F33F000033334D5D0FFFF43333333388788F3338F33F00003333
      45D50FEFE4333333338F878F3338F33F000033334D5D0FFFF43333333388788F
      3338F33F0000333345D50FEFE4333333338F878F3338F33F000033334D5D0FFF
      F43333333388788F3338F33F0000333345D50FEFE4333333338F878F3338F33F
      000033334D5D0EFEF43333333388788F3338F33F0000333345D50FEFE4333333
      338F878F3338F33F000033334D5D0EFEF43333333388788F3338F33F00003333
      4444444444333333338F8F8FFFF8F33F00003333333333333333333333888888
      8888333F00003333330000003333333333333FFFFFF3333F00003333330AAAA0
      333333333333888888F3333F00003333330000003333333333338FFFF8F3333F
      0000}
    Layout = blGlyphRight
    NumGlyphs = 2
    Style = bsWin31
  end
end
