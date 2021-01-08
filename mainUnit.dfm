object MainForm: TMainForm
  Left = 554
  Top = 166
  Width = 1024
  Height = 736
  Caption = 'PM'
  Color = clBtnFace
  Constraints.MinHeight = 480
  Constraints.MinWidth = 640
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 200
    Top = 0
    Width = 3
    Height = 671
    Cursor = crHSplit
    Visible = False
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 671
    Width = 1016
    Height = 19
    Panels = <
      item
        Alignment = taCenter
        BiDiMode = bdRightToLeft
        ParentBiDiMode = False
        Style = psOwnerDraw
        Width = 100
      end
      item
        Alignment = taCenter
        Width = 100
      end
      item
        Alignment = taCenter
        BiDiMode = bdLeftToRight
        ParentBiDiMode = False
        Width = 100
      end
      item
        Alignment = taCenter
        BiDiMode = bdLeftToRight
        ParentBiDiMode = False
        Width = 500
      end
      item
        Width = 50
      end>
    SimplePanel = False
    OnDrawPanel = StatusBarDrawPanel
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 200
    Height = 671
    Align = alLeft
    Anchors = [akLeft, akBottom]
    BevelOuter = bvNone
    TabOrder = 1
    DesignSize = (
      200
      671)
    object Label2: TLabel
      Left = 0
      Top = 0
      Width = 200
      Height = 13
      Align = alTop
      Caption = '  Mode:'
      Enabled = False
    end
    object lbClientsList: TLabel
      Left = 7
      Top = 147
      Width = 80
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      Caption = 'Ext. Connections'
    end
    object Label1: TLabel
      Left = 7
      Top = 201
      Width = 37
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      Caption = 'Masters'
    end
    object Label3: TLabel
      Left = 7
      Top = 455
      Width = 96
      Height = 13
      Anchors = [akLeft, akBottom]
      Caption = '> ch pwr sen'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier'
      Font.Style = []
      ParentFont = False
    end
    object ModeComboBox: TComboBox
      Left = 3
      Top = 16
      Width = 192
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      Enabled = False
      ItemHeight = 13
      TabOrder = 1
      Text = '---'
      Items.Strings = (
        'Default'
        'Relay')
    end
    object StartButton: TButton
      Left = 8
      Top = 637
      Width = 81
      Height = 25
      Anchors = [akLeft, akBottom]
      Caption = 'Start'
      TabOrder = 0
      OnClick = StartButtonClick
    end
    object OptionsGroupBox: TGroupBox
      Left = 3
      Top = 43
      Width = 192
      Height = 101
      Anchors = [akLeft, akTop, akRight]
      Caption = ' Options '
      Ctl3D = True
      ParentCtl3D = False
      TabOrder = 2
      object Label4: TLabel
        Left = 107
        Top = 59
        Width = 54
        Height = 13
        Alignment = taRightJustify
        Caption = 'Interval, ms'
      end
      object Label5: TLabel
        Left = 7
        Top = 59
        Width = 68
        Height = 13
        Caption = 'Verbose Level'
        FocusControl = Button1
      end
      object cbAutoStart: TCheckBox
        Left = 8
        Top = 13
        Width = 97
        Height = 17
        Caption = 'Auto Start'
        TabOrder = 0
        OnClick = cbAutoStartClick
      end
      object CheckBox1: TCheckBox
        Left = 8
        Top = 44
        Width = 65
        Height = 16
        Caption = 'Log to file'
        TabOrder = 1
        OnClick = CheckBox1Click
      end
      object cbDoSetBR: TCheckBox
        Left = 100
        Top = 13
        Width = 85
        Height = 17
        Alignment = taLeftJustify
        Caption = 'Do Set RSBR'
        TabOrder = 2
        OnClick = cbDoSetBRClick
      end
      object cbSync: TCheckBox
        Left = 8
        Top = 28
        Width = 97
        Height = 17
        Caption = 'Sync threads'
        TabOrder = 3
        OnClick = cbSyncClick
      end
      object eChInterval: TEdit
        Left = 114
        Top = 74
        Width = 41
        Height = 21
        TabOrder = 4
        Text = '3'
        OnChange = eChIntervalChange
      end
      object UpDown: TUpDown
        Left = 155
        Top = 74
        Width = 16
        Height = 21
        Min = 1
        Max = 60
        Position = 3
        TabOrder = 5
        Thousands = False
        Wrap = False
      end
      object cbVerbose: TComboBox
        Left = 8
        Top = 74
        Width = 85
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 6
        OnChange = cbVerboseClick
        Items.Strings = (
          'L0 (err.only)'
          'L1 (visual)'
          'L2 (debug 1)'
          'L3 (debug 2)'
          'L4 (slow work)')
      end
      object CheckBox2: TCheckBox
        Left = 108
        Top = 43
        Width = 77
        Height = 17
        Alignment = taLeftJustify
        BiDiMode = bdLeftToRight
        Caption = 'Tune delays'
        ParentBiDiMode = False
        TabOrder = 7
        OnClick = CheckBox2Click
      end
      object CheckBox3: TCheckBox
        Left = 99
        Top = 28
        Width = 86
        Height = 17
        Alignment = taLeftJustify
        Caption = 'Do Set Power'
        TabOrder = 8
        OnClick = CheckBox3Click
      end
    end
    object lbClients: TListBox
      Left = 3
      Top = 163
      Width = 192
      Height = 30
      TabStop = False
      Anchors = [akLeft, akTop, akRight]
      Enabled = False
      ExtendedSelect = False
      ItemHeight = 13
      TabOrder = 3
    end
    object lbServers: TListBox
      Left = 3
      Top = 216
      Width = 192
      Height = 234
      Anchors = [akLeft, akTop, akRight, akBottom]
      ExtendedSelect = False
      ItemHeight = 13
      PopupMenu = pmMaster
      TabOrder = 4
      OnClick = lbServersClick
      OnMouseDown = lbServersMouseUp
    end
    object lbChannels: TListBox
      Left = 3
      Top = 470
      Width = 191
      Height = 160
      Anchors = [akLeft, akRight, akBottom]
      Enabled = False
      ExtendedSelect = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier'
      Font.Style = []
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
        '12')
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 5
    end
    object Button1: TButton
      Left = 160
      Top = 534
      Width = 25
      Height = 24
      Anchors = [akLeft, akBottom]
      Caption = 'B1'
      Enabled = False
      TabOrder = 6
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 160
      Top = 565
      Width = 25
      Height = 25
      Anchors = [akLeft, akBottom]
      Caption = 'B2'
      TabOrder = 7
      OnClick = Button2Click
    end
    object Button3: TButton
      Left = 160
      Top = 597
      Width = 25
      Height = 25
      Anchors = [akLeft, akBottom]
      Caption = 'B3'
      Enabled = False
      TabOrder = 8
      OnClick = Button3Click
    end
    object mthreadCheckBox: TCheckBox
      Left = 123
      Top = 200
      Width = 72
      Height = 17
      Alignment = taLeftJustify
      Caption = 'mon.thread'
      Enabled = False
      TabOrder = 9
      OnClick = mthreadCheckBoxClick
    end
    object ScanButton: TButton
      Left = 144
      Top = 452
      Width = 51
      Height = 17
      Anchors = [akLeft, akBottom]
      Caption = 'Scan'
      TabOrder = 10
      OnClick = ScanButtonClick
    end
    object cbF3004: TCheckBox
      Left = 96
      Top = 638
      Width = 97
      Height = 17
      Anchors = [akLeft, akBottom]
      Caption = 'Use port 3004'
      TabOrder = 11
      OnClick = cbF3004Click
    end
  end
  object Panel2: TPanel
    Left = 203
    Top = 0
    Width = 813
    Height = 671
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 2
    object Splitter2: TSplitter
      Left = 0
      Top = 450
      Width = 813
      Height = 3
      Cursor = crVSplit
      Align = alTop
    end
    object LogMemo: TRichEdit
      Left = 0
      Top = 0
      Width = 813
      Height = 450
      Align = alTop
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier'
      Font.Style = []
      ParentFont = False
      PlainText = True
      ReadOnly = True
      ScrollBars = ssVertical
      TabOrder = 0
    end
    object TabControl1: TTabControl
      Left = 0
      Top = 453
      Width = 813
      Height = 218
      Align = alClient
      HotTrack = True
      Style = tsButtons
      TabOrder = 1
      Tabs.Strings = (
        'Cycles per second'
        'Indexes')
      TabIndex = 0
      OnChange = TabControl1Change
      object Chart1: TChart
        Left = 4
        Top = 27
        Width = 805
        Height = 187
        BackWall.Brush.Color = clWhite
        BackWall.Color = clBtnText
        BackWall.Pen.Visible = False
        Foot.Alignment = taLeftJustify
        MarginBottom = 0
        MarginLeft = 0
        MarginRight = 0
        MarginTop = 0
        Title.AdjustFrame = False
        Title.Alignment = taLeftJustify
        Title.Brush.Color = clWhite
        Title.Brush.Style = bsClear
        Title.Color = clWhite
        Title.Font.Charset = DEFAULT_CHARSET
        Title.Font.Color = clBlue
        Title.Font.Height = -11
        Title.Font.Name = 'Arial'
        Title.Font.Style = [fsBold]
        Title.Text.Strings = (
          'Cycles per second vs. Uptime, s')
        BackColor = clBtnText
        BottomAxis.Axis.Width = 1
        BottomAxis.AxisValuesFormat = '###0.###'
        BottomAxis.ExactDateTime = False
        BottomAxis.Grid.Color = cl3DDkShadow
        BottomAxis.Grid.Style = psSolid
        BottomAxis.Grid.SmallDots = True
        BottomAxis.LabelsSeparation = 15
        BottomAxis.MinorTickCount = 4
        BottomAxis.MinorTicks.Visible = False
        BottomAxis.TickOnLabelsOnly = False
        BottomAxis.Ticks.Color = clBlack
        ClipPoints = False
        Frame.Visible = False
        LeftAxis.Axis.Width = 1
        LeftAxis.AxisValuesFormat = '###0.###'
        LeftAxis.Grid.Color = cl3DDkShadow
        LeftAxis.Grid.Style = psSolid
        LeftAxis.Grid.SmallDots = True
        LeftAxis.MinorTicks.Color = clGradientActiveCaption
        LeftAxis.MinorTicks.Visible = False
        LeftAxis.Ticks.Color = clBlack
        Legend.Color = clBlack
        Legend.Font.Charset = DEFAULT_CHARSET
        Legend.Font.Color = clBtnFace
        Legend.Font.Height = -11
        Legend.Font.Name = 'Arial'
        Legend.Font.Style = []
        Legend.Frame.Visible = False
        Legend.LegendStyle = lsSeries
        Legend.ShadowSize = 0
        Legend.TopPos = 6
        View3D = False
        Align = alClient
        BevelOuter = bvNone
        BevelWidth = 4
        BorderWidth = 4
        BorderStyle = bsSingle
        TabOrder = 0
        OnDblClick = Chart1DblClick
        object Series: TFastLineSeries
          Cursor = crCross
          Marks.ArrowLength = 8
          Marks.Style = smsValue
          Marks.Visible = False
          SeriesColor = clRed
          LinePen.Color = clRed
          XValues.DateTime = True
          XValues.Name = 'X'
          XValues.Multiplier = 1
          XValues.Order = loAscending
          YValues.DateTime = False
          YValues.Name = 'Y'
          YValues.Multiplier = 1
          YValues.Order = loNone
        end
        object Series1: TFastLineSeries
          Marks.ArrowLength = 8
          Marks.Style = smsValue
          Marks.Visible = False
          SeriesColor = clBlue
          LinePen.Color = clBlue
          XValues.DateTime = False
          XValues.Name = 'X'
          XValues.Multiplier = 1
          XValues.Order = loAscending
          YValues.DateTime = False
          YValues.Name = 'Y'
          YValues.Multiplier = 1
          YValues.Order = loNone
        end
        object Series2: TPointSeries
          Marks.ArrowLength = 0
          Marks.Visible = False
          SeriesColor = clGreen
          Pointer.Brush.Color = clGreen
          Pointer.HorizSize = 1
          Pointer.InflateMargins = False
          Pointer.Pen.Visible = False
          Pointer.Style = psSmallDot
          Pointer.VertSize = 1
          Pointer.Visible = True
          XValues.DateTime = False
          XValues.Name = 'X'
          XValues.Multiplier = 1
          XValues.Order = loAscending
          YValues.DateTime = False
          YValues.Name = 'Y'
          YValues.Multiplier = 1
          YValues.Order = loNone
        end
      end
      object Chart2: TChart
        Left = 4
        Top = 27
        Width = 805
        Height = 187
        BackWall.Brush.Color = clWhite
        BackWall.Color = clBtnText
        BackWall.Pen.Visible = False
        Foot.Alignment = taLeftJustify
        MarginBottom = 0
        MarginLeft = 0
        MarginRight = 0
        MarginTop = 0
        Title.AdjustFrame = False
        Title.Alignment = taLeftJustify
        Title.Brush.Color = clWhite
        Title.Brush.Style = bsClear
        Title.Color = clWhite
        Title.Font.Charset = DEFAULT_CHARSET
        Title.Font.Color = clBlue
        Title.Font.Height = -11
        Title.Font.Name = 'Arial'
        Title.Font.Style = [fsBold]
        Title.Text.Strings = (
          'Indexes vs. Uptime, s')
        BackColor = clBtnText
        BottomAxis.Axis.Width = 1
        BottomAxis.AxisValuesFormat = '###0.###'
        BottomAxis.ExactDateTime = False
        BottomAxis.Grid.Color = cl3DDkShadow
        BottomAxis.Grid.Style = psSolid
        BottomAxis.Grid.SmallDots = True
        BottomAxis.LabelsSeparation = 15
        BottomAxis.MinorTickCount = 4
        BottomAxis.MinorTicks.Visible = False
        BottomAxis.TickOnLabelsOnly = False
        BottomAxis.Ticks.Color = clBlack
        ClipPoints = False
        Frame.Visible = False
        LeftAxis.Axis.Width = 1
        LeftAxis.AxisValuesFormat = '###0.###'
        LeftAxis.Grid.Color = cl3DDkShadow
        LeftAxis.Grid.Style = psSolid
        LeftAxis.Grid.SmallDots = True
        LeftAxis.MinorTicks.Color = clGradientActiveCaption
        LeftAxis.MinorTicks.Visible = False
        LeftAxis.Ticks.Color = clBlack
        Legend.Color = clBlack
        Legend.Font.Charset = DEFAULT_CHARSET
        Legend.Font.Color = clBtnFace
        Legend.Font.Height = -11
        Legend.Font.Name = 'Arial'
        Legend.Font.Style = []
        Legend.Frame.Visible = False
        Legend.LegendStyle = lsSeries
        Legend.ShadowSize = 0
        Legend.TopPos = 6
        View3D = False
        Align = alClient
        BevelOuter = bvNone
        BevelWidth = 4
        BorderWidth = 4
        BorderStyle = bsSingle
        TabOrder = 1
        Visible = False
        OnDblClick = Chart1DblClick
        object FastLineSeries1: TFastLineSeries
          Cursor = crCross
          Marks.ArrowLength = 8
          Marks.Style = smsValue
          Marks.Visible = False
          SeriesColor = clRed
          LinePen.Color = clRed
          XValues.DateTime = True
          XValues.Name = 'X'
          XValues.Multiplier = 1
          XValues.Order = loAscending
          YValues.DateTime = False
          YValues.Name = 'Y'
          YValues.Multiplier = 1
          YValues.Order = loNone
        end
        object FastLineSeries2: TFastLineSeries
          Marks.ArrowLength = 8
          Marks.Style = smsValue
          Marks.Visible = False
          SeriesColor = clBlue
          LinePen.Color = clBlue
          XValues.DateTime = False
          XValues.Name = 'X'
          XValues.Multiplier = 1
          XValues.Order = loAscending
          YValues.DateTime = False
          YValues.Name = 'Y'
          YValues.Multiplier = 1
          YValues.Order = loNone
        end
        object PointSeries1: TPointSeries
          Marks.ArrowLength = 0
          Marks.Visible = False
          SeriesColor = clGreen
          Pointer.Brush.Color = clGreen
          Pointer.HorizSize = 1
          Pointer.InflateMargins = False
          Pointer.Pen.Visible = False
          Pointer.Style = psSmallDot
          Pointer.VertSize = 1
          Pointer.Visible = True
          XValues.DateTime = False
          XValues.Name = 'X'
          XValues.Multiplier = 1
          XValues.Order = loAscending
          YValues.DateTime = False
          YValues.Name = 'Y'
          YValues.Multiplier = 1
          YValues.Order = loNone
        end
      end
    end
  end
  object ReFreshTimer: TTimer
    Enabled = False
    Interval = 200
    Left = 528
    Top = 160
  end
  object MainMenu1: TMainMenu
    BiDiMode = bdLeftToRight
    ParentBiDiMode = False
    Left = 496
    Top = 160
    object File3: TMenuItem
      Caption = '&File'
      object LoadConfig1: TMenuItem
        Caption = '&Load Config...'
        Enabled = False
      end
      object ReadDataFile: TMenuItem
        Caption = '&Read Data File...'
        OnClick = ReadDataFileClick
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object SaveLog1: TMenuItem
        Caption = 'S&ave Log...'
        Enabled = False
      end
      object SaveAs2: TMenuItem
        Caption = '&Save Data...'
        Enabled = False
        OnClick = SaveAs2Click
      end
      object N4: TMenuItem
        Caption = '-'
      end
      object Exit3: TMenuItem
        Caption = 'E&xit'
        Enabled = False
      end
    end
    object Tools1: TMenuItem
      Caption = 'Tools'
      object ExtractData: TMenuItem
        Caption = 'Data Extractor...'
        OnClick = ExtractDataClick
      end
      object PowerManager1: TMenuItem
        Caption = 'Power Manager...'
        OnClick = PowerManager1Click
      end
    end
    object Settings1: TMenuItem
      Caption = 'Set HV'
      OnClick = Settings1Click
    end
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'dat'
    Filter = 'Data files (*.dat)|*.dat'
    InitialDir = 'Data'
    Options = [ofOverwritePrompt, ofEnableSizing]
    Left = 464
    Top = 160
  end
  object ApplicationEvents1: TApplicationEvents
    OnException = ApplicationEvents1Exception
    OnMessage = ApplicationEvents1Message
    Left = 560
    Top = 160
  end
  object ConnectedThreads: TIdThreadMgrDefault
    Left = 400
    Top = 160
  end
  object TTS: TIdTCPServer
    Bindings = <>
    DefaultPort = 0
    OnConnect = TTSConnect
    OnExecute = TTSExecute
    OnDisconnect = TTSDisconnect
    ThreadMgr = ConnectedThreads
    Left = 368
    Top = 160
  end
  object PopupMenu1: TPopupMenu
    Left = 432
    Top = 160
    object Clearlog1: TMenuItem
      Caption = 'Clear log'
      OnClick = Clearlog1Click
    end
    object Savetologtxt1: TMenuItem
      Caption = 'Save to log.txt'
      OnClick = Savetologtxt1Click
    end
  end
  object OpenDialog1: TOpenDialog
    InitialDir = '.'
    Left = 464
    Top = 128
  end
  object pmMaster: TPopupMenu
    OnPopup = pmMasterPopup
    Left = 432
    Top = 192
    object Reconnect: TMenuItem
      Caption = 'Reconnect'
      OnClick = ReconnectClick
    end
  end
end
