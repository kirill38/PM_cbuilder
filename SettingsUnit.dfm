object SettingsForm: TSettingsForm
  Left = 179
  Top = 277
  Width = 164
  Height = 101
  Caption = 'Settings'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 11
    Top = 37
    Width = 21
    Height = 13
    Caption = 'HV='
  end
  object CheckBox1: TCheckBox
    Left = 8
    Top = 8
    Width = 137
    Height = 17
    Caption = 'Set Total HV (0...1100)'
    TabOrder = 0
    OnClick = CheckBox1Click
  end
  object Edit1: TEdit
    Left = 80
    Top = 32
    Width = 73
    Height = 21
    TabOrder = 1
    OnChange = Edit1Change
  end
end
