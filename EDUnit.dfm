object EDForm: TEDForm
  Left = 688
  Top = 278
  Width = 659
  Height = 265
  Caption = 'Extract data'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  DesignSize = (
    651
    238)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 2
    Top = 11
    Width = 23
    Height = 13
    Alignment = taRightJustify
    BiDiMode = bdLeftToRight
    Caption = 'From'
    ParentBiDiMode = False
  end
  object Label2: TLabel
    Left = 12
    Top = 35
    Width = 13
    Height = 13
    Alignment = taRightJustify
    BiDiMode = bdLeftToRight
    Caption = 'To'
    ParentBiDiMode = False
  end
  object Label3: TLabel
    Left = 37
    Top = 107
    Width = 32
    Height = 13
    Alignment = taRightJustify
    Caption = 'Master'
  end
  object Label4: TLabel
    Left = 17
    Top = 131
    Width = 52
    Height = 13
    Alignment = taRightJustify
    Caption = 'Chan/Addr'
  end
  object Label5: TLabel
    Left = 21
    Top = 155
    Width = 48
    Height = 13
    Alignment = taRightJustify
    Caption = 'Parameter'
  end
  object NDimLabel: TLabel
    Left = 22
    Top = 179
    Width = 47
    Height = 13
    Alignment = taRightJustify
    Caption = 'X/Y index'
  end
  object Label6: TLabel
    Left = 23
    Top = 84
    Width = 46
    Height = 13
    Alignment = taRightJustify
    Caption = 'Data type'
  end
  object FromPicker: TDateTimePicker
    Left = 29
    Top = 8
    Width = 79
    Height = 21
    CalAlignment = dtaLeft
    Date = 43536.3125
    Time = 43536.3125
    DateFormat = dfShort
    DateMode = dmComboBox
    Kind = dtkDate
    ParseInput = False
    TabOrder = 0
    OnChange = FromPickerChange
  end
  object ToPicker: TDateTimePicker
    Left = 29
    Top = 32
    Width = 79
    Height = 21
    CalAlignment = dtaLeft
    Date = 43536.4166666667
    Time = 43536.4166666667
    DateFormat = dfShort
    DateMode = dmComboBox
    Kind = dtkDate
    ParseInput = False
    TabOrder = 1
    OnChange = ToPickerChange
  end
  object MasterCB: TComboBox
    Left = 73
    Top = 104
    Width = 100
    Height = 21
    ItemHeight = 13
    TabOrder = 2
    Text = '192.168.21.198'
    OnChange = MasterCBChange
  end
  object ChannelCB: TComboBox
    Left = 73
    Top = 128
    Width = 48
    Height = 21
    ItemHeight = 13
    TabOrder = 3
    Text = '7'
    OnChange = ChannelCBChange
  end
  object ParameterCB: TComboBox
    Left = 73
    Top = 152
    Width = 100
    Height = 21
    ItemHeight = 13
    Sorted = True
    TabOrder = 4
    Text = '199'
  end
  object ParametersLB: TListBox
    Left = 224
    Top = 8
    Width = 335
    Height = 202
    Anchors = [akLeft, akTop, akRight, akBottom]
    ItemHeight = 13
    TabOrder = 5
    OnDblClick = ParametersLBDblClick
  end
  object AddButton: TButton
    Left = 128
    Top = 200
    Width = 75
    Height = 17
    Caption = 'Add to list'
    TabOrder = 6
    OnClick = AddButtonClick
  end
  object CheckBox1: TCheckBox
    Left = 20
    Top = 58
    Width = 97
    Height = 17
    Alignment = taLeftJustify
    Caption = 'From config'
    TabOrder = 7
  end
  object DrawButton: TButton
    Left = 566
    Top = 115
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'Draw'
    TabOrder = 8
    OnClick = DrawButtonClick
  end
  object SaveButton: TButton
    Left = 566
    Top = 168
    Width = 75
    Height = 17
    Anchors = [akTop, akRight]
    Caption = 'Save to file'
    Enabled = False
    TabOrder = 9
  end
  object CloseButton: TButton
    Left = 566
    Top = 188
    Width = 75
    Height = 22
    Anchors = [akRight, akBottom]
    Caption = 'Close'
    TabOrder = 10
    OnClick = CloseButtonClick
  end
  object ReadDataButton: TButton
    Left = 566
    Top = 40
    Width = 75
    Height = 17
    Anchors = [akTop, akRight]
    Caption = 'ReadData'
    Enabled = False
    TabOrder = 11
  end
  object Button1: TButton
    Left = 566
    Top = 8
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'Get File list'
    TabOrder = 12
    OnClick = Button1Click
  end
  object DateTimePicker1: TDateTimePicker
    Left = 110
    Top = 8
    Width = 70
    Height = 21
    CalAlignment = dtaLeft
    Date = 43536.3125
    Time = 43536.3125
    DateFormat = dfShort
    DateMode = dmComboBox
    Kind = dtkTime
    ParseInput = False
    TabOrder = 13
    OnChange = DateTimePicker1Change
  end
  object DateTimePicker2: TDateTimePicker
    Left = 110
    Top = 32
    Width = 70
    Height = 21
    CalAlignment = dtaLeft
    Date = 43536.4166666667
    Time = 43536.4166666667
    DateFormat = dfShort
    DateMode = dmComboBox
    Kind = dtkTime
    ParseInput = False
    TabOrder = 14
    OnChange = DateTimePicker2Change
  end
  object ProgressBar1: TProgressBar
    Left = 0
    Top = 222
    Width = 651
    Height = 16
    Align = alBottom
    Min = 0
    Max = 100
    TabOrder = 15
  end
  object Button2: TButton
    Left = 566
    Top = 144
    Width = 75
    Height = 17
    Anchors = [akTop, akRight]
    Caption = 'All in thread'
    Enabled = False
    TabOrder = 16
    OnClick = Button2Click
  end
  object NDimCB: TComboBox
    Left = 128
    Top = 176
    Width = 45
    Height = 21
    ItemHeight = 13
    Sorted = True
    TabOrder = 17
    Text = '5'
    Items.Strings = (
      '1'
      '2'
      '3'
      '4'
      '5'
      '6')
  end
  object Button3: TButton
    Left = 176
    Top = 80
    Width = 41
    Height = 17
    Caption = 'Scan'
    Enabled = False
    TabOrder = 18
  end
  object Button4: TButton
    Left = 176
    Top = 104
    Width = 41
    Height = 17
    Caption = 'Fill Cl.'
    Enabled = False
    TabOrder = 19
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 176
    Top = 128
    Width = 41
    Height = 17
    Caption = 'Fill St.'
    TabOrder = 20
    OnClick = Button5Click
  end
  object AddressCB: TComboBox
    Left = 128
    Top = 128
    Width = 45
    Height = 21
    ItemHeight = 13
    TabOrder = 21
    Text = '7'
    OnChange = ChannelCBChange
  end
  object XidxCB: TComboBox
    Left = 72
    Top = 176
    Width = 48
    Height = 21
    ItemHeight = 13
    Sorted = True
    TabOrder = 22
    Text = '4'
    Items.Strings = (
      '1'
      '2'
      '3'
      '4'
      '5'
      '6')
  end
  object DataTypeCB: TComboBox
    Left = 73
    Top = 80
    Width = 100
    Height = 21
    ItemHeight = 13
    TabOrder = 23
    Text = '0'
    Items.Strings = (
      '0'
      '1'
      '2'
      '3'
      '4')
  end
  object CheckBox2: TCheckBox
    Left = 176
    Top = 176
    Width = 46
    Height = 17
    BiDiMode = bdLeftToRight
    Caption = 'count'
    ParentBiDiMode = False
    TabOrder = 24
    OnClick = CheckBox2Click
  end
end
