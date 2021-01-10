object PowerForm: TPowerForm
  Left = 765
  Top = 112
  Width = 506
  Height = 767
  Caption = 'Power Manager'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 360
    Width = 96
    Height = 20
    Caption = 'Power source'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 8
    Top = 16
    Width = 88
    Height = 20
    Caption = 'Commutator'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 85
    Top = 504
    Width = 78
    Height = 13
    Caption = 'Power state: ON'
  end
  object PowerCLB: TCheckListBox
    Left = 8
    Top = 80
    Width = 161
    Height = 217
    OnClickCheck = PowerCLBClickCheck
    AutoComplete = False
    ItemHeight = 13
    Items.Strings = (
      '1'
      '2'
      '3'
      '4'
      '5'
      '6'
      '7'
      '8'
      '9'
      '10'
      '11'
      '12'
      '13'
      '14'
      '15'
      '16')
    TabOrder = 0
    OnClick = PowerCLBClick
  end
  object LabeledEdit1: TLabeledEdit
    Left = 8
    Top = 56
    Width = 89
    Height = 21
    EditLabel.Width = 68
    EditLabel.Height = 13
    EditLabel.Caption = 'NPort Address'
    LabelPosition = lpAbove
    LabelSpacing = 3
    TabOrder = 1
    Text = '192.168.1.14'
    OnChange = LabeledEdit1Change
  end
  object LabeledEdit2: TLabeledEdit
    Left = 104
    Top = 56
    Width = 65
    Height = 21
    EditLabel.Width = 19
    EditLabel.Height = 13
    EditLabel.Caption = 'Port'
    LabelPosition = lpAbove
    LabelSpacing = 3
    TabOrder = 2
    Text = '4001'
    OnChange = LabeledEdit2Change
  end
  object LabeledEdit3: TLabeledEdit
    Left = 8
    Top = 320
    Width = 73
    Height = 21
    EditLabel.Width = 70
    EditLabel.Height = 13
    EditLabel.Caption = 'Ask Period, ms'
    Enabled = False
    LabelPosition = lpAbove
    LabelSpacing = 3
    TabOrder = 3
  end
  object ListBox1: TListBox
    Left = 176
    Top = 8
    Width = 297
    Height = 713
    ItemHeight = 13
    TabOrder = 4
  end
  object Button1: TButton
    Left = 16
    Top = 600
    Width = 75
    Height = 25
    Caption = 'Strart'
    Enabled = False
    TabOrder = 5
  end
  object CheckBox1: TCheckBox
    Left = 16
    Top = 672
    Width = 97
    Height = 17
    Caption = 'CheckBox1'
    Enabled = False
    TabOrder = 6
  end
  object CheckBox2: TCheckBox
    Left = 16
    Top = 696
    Width = 97
    Height = 17
    Caption = 'CheckBox2'
    Enabled = False
    TabOrder = 7
  end
  object Button2: TButton
    Left = 96
    Top = 600
    Width = 75
    Height = 25
    Caption = 'On'
    Enabled = False
    TabOrder = 8
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 120
    Top = 632
    Width = 49
    Height = 25
    Caption = 'Close'
    Enabled = False
    TabOrder = 9
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 16
    Top = 632
    Width = 75
    Height = 25
    Caption = 'init'
    Enabled = False
    TabOrder = 10
    OnClick = Button4Click
  end
  object LabeledEdit4: TLabeledEdit
    Left = 104
    Top = 320
    Width = 65
    Height = 21
    Color = clBtnFace
    EditLabel.Width = 25
    EditLabel.Height = 13
    EditLabel.Caption = 'State'
    LabelPosition = lpAbove
    LabelSpacing = 3
    ReadOnly = True
    TabOrder = 11
  end
  object LabeledEdit5: TLabeledEdit
    Left = 8
    Top = 400
    Width = 89
    Height = 21
    EditLabel.Width = 53
    EditLabel.Height = 13
    EditLabel.Hint = 'NPort Address, connected to power source'
    EditLabel.Caption = 'Address (&?)'
    EditLabel.ParentShowHint = False
    EditLabel.ShowHint = True
    LabelPosition = lpAbove
    LabelSpacing = 3
    TabOrder = 12
    Text = '192.168.1.24'
    OnChange = LabeledEdit5Change
  end
  object LabeledEdit6: TLabeledEdit
    Left = 104
    Top = 400
    Width = 65
    Height = 21
    EditLabel.Width = 19
    EditLabel.Height = 13
    EditLabel.Caption = 'Port'
    LabelPosition = lpAbove
    LabelSpacing = 3
    TabOrder = 13
    Text = '4001'
    OnChange = LabeledEdit6Change
  end
  object LabeledEdit7: TLabeledEdit
    Left = 8
    Top = 480
    Width = 65
    Height = 21
    Color = clBtnFace
    EditLabel.Width = 24
    EditLabel.Height = 13
    EditLabel.Caption = 'I, mA'
    LabelPosition = lpAbove
    LabelSpacing = 3
    ReadOnly = True
    TabOrder = 14
  end
  object LabeledEdit8: TLabeledEdit
    Left = 8
    Top = 440
    Width = 65
    Height = 21
    Color = clBtnFace
    EditLabel.Width = 21
    EditLabel.Height = 13
    EditLabel.Caption = 'U, V'
    LabelPosition = lpAbove
    LabelSpacing = 3
    ReadOnly = True
    TabOrder = 15
  end
  object LabeledEdit9: TLabeledEdit
    Left = 88
    Top = 480
    Width = 65
    Height = 21
    EditLabel.Width = 54
    EditLabel.Height = 13
    EditLabel.Caption = 'I cutoff, mA'
    EditLabel.Color = clBtnFace
    EditLabel.ParentColor = False
    LabelPosition = lpAbove
    LabelSpacing = 3
    ReadOnly = True
    TabOrder = 16
    Text = '0'
  end
  object LabeledEdit10: TLabeledEdit
    Left = 88
    Top = 440
    Width = 65
    Height = 21
    EditLabel.Width = 51
    EditLabel.Height = 13
    EditLabel.Caption = 'U cutoff, V'
    LabelPosition = lpAbove
    LabelSpacing = 3
    ReadOnly = True
    TabOrder = 17
    Text = '0'
  end
  object UpDown1: TUpDown
    Left = 153
    Top = 440
    Width = 15
    Height = 21
    Associate = LabeledEdit10
    Min = 0
    Position = 0
    TabOrder = 18
    Wrap = False
  end
  object UpDown2: TUpDown
    Left = 153
    Top = 480
    Width = 15
    Height = 21
    Associate = LabeledEdit9
    Min = 0
    Position = 0
    TabOrder = 19
    Wrap = False
  end
  object LabeledEdit11: TLabeledEdit
    Left = 8
    Top = 520
    Width = 65
    Height = 21
    Color = clBtnFace
    EditLabel.Width = 24
    EditLabel.Height = 13
    EditLabel.Caption = 'P, W'
    LabelPosition = lpAbove
    LabelSpacing = 3
    ReadOnly = True
    TabOrder = 20
  end
  object Button5: TButton
    Left = 128
    Top = 520
    Width = 41
    Height = 21
    Caption = 'OFF'
    TabOrder = 21
  end
  object CheckBox3: TCheckBox
    Left = 80
    Top = 524
    Width = 45
    Height = 21
    Caption = 'Lock'
    TabOrder = 22
  end
  object Edit1: TEdit
    Left = 8
    Top = 552
    Width = 97
    Height = 21
    TabOrder = 23
    Text = '192.168.21.16'
    OnChange = Edit1Change
  end
  object CheckBox4: TCheckBox
    Left = 8
    Top = 576
    Width = 121
    Height = 17
    Caption = 'UDP Server Enable'
    TabOrder = 24
    OnClick = CheckBox4Click
  end
  object UdpSocket1: TUdpSocket
    BlockMode = bmNonBlocking
    RemoteHost = '192.168.1.14'
    RemotePort = '4001'
    OnCreateHandle = UdpSocket1CreateHandle
    OnReceive = UdpSocket1Receive
    OnSend = UdpSocket1Send
    OnError = UdpSocket1Error
    Left = 104
    Top = 88
  end
  object IdUDPServer1: TIdUDPServer
    Bindings = <
      item
        IP = '192.168.21.16'
        Port = 4001
      end
      item
        IP = '192.168.21.16'
        Port = 4002
      end>
    DefaultPort = 0
    OnUDPRead = IdUDPServer1UDPRead
    Left = 136
    Top = 120
  end
  object UdpSocket2: TUdpSocket
    BlockMode = bmNonBlocking
    RemoteHost = '192.168.1.24'
    RemotePort = '4001'
    Left = 136
    Top = 88
  end
end
