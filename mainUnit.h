//---------------------------------------------------------------------------

#ifndef mainUnitH
#define mainUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Chart.hpp>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <ComCtrls.hpp>
#include <TeeFunci.hpp>
#include <ToolWin.hpp>
#include <Grids.hpp>
#include <Menus.hpp>
#include <ValEdit.hpp>
#include <Dialogs.hpp>
#include <AppEvnts.hpp>
#include <ImgList.hpp>
#include "CGAUGES.h"
#include <CustomizeDlg.hpp>
#include <Chartfx3.hpp>
#include <OleCtrls.hpp>
#include <VCFI.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdTCPServer.hpp>
#include <IdThreadMgr.hpp>
#include <IdThreadMgrDefault.hpp>
#include <IniFiles.hpp>
#include <Sockets.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <IdThreadMgrPool.hpp>
#include <IdIPWatch.hpp>
#include <fstream.h>

//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
    TTimer *ReFreshTimer;
    TStatusBar *StatusBar;
    TPanel *Panel1;
    TSplitter *Splitter1;
    TMainMenu *MainMenu1;
    TPanel *Panel2;
    TComboBox *ModeComboBox;
    TButton *StartButton;
    TGroupBox *OptionsGroupBox;
    TMenuItem *File3;
    TMenuItem *N4;
    TMenuItem *SaveAs2;
    TMenuItem *Exit3;
    TMenuItem *N1;
    TMenuItem *LoadConfig1;
    TSaveDialog *SaveDialog;
    TMenuItem *SaveLog1;
    TApplicationEvents *ApplicationEvents1;
    TCheckBox *cbAutoStart;
    TIdThreadMgrDefault *ConnectedThreads;
    TListBox *lbClients;
    TLabel *Label2;
    TLabel *lbClientsList;
    TListBox *lbServers;
    TLabel *Label1;
    TIdTCPServer *TTS;
    TListBox *lbChannels;
    TLabel *Label3;
    TButton *Button1;
    TButton *Button2;
    TButton *Button3;
    TCheckBox *mthreadCheckBox;
    TPopupMenu *PopupMenu1;
    TMenuItem *Clearlog1;
    TButton *ScanButton;
    TMenuItem *ReadDataFile;
    TOpenDialog *OpenDialog1;
    TCheckBox *CheckBox1;
    TCheckBox *cbDoSetBR;
    TCheckBox *cbSync;
    TEdit *eChInterval;
    TUpDown *UpDown;
    TLabel *Label4;
    TComboBox *cbVerbose;
    TLabel *Label5;
    TCheckBox *CheckBox2;
    TPopupMenu *pmMaster;
    TMenuItem *Reconnect;
    TCheckBox *CheckBox3;
    TMenuItem *Savetologtxt1;
    TMenuItem *ExtractData;
    TRichEdit *LogMemo;
    TSplitter *Splitter2;
    TCheckBox *cbF3004;
    TMenuItem *Tools1;
    TMenuItem *PowerManager1;
        TMenuItem *Settings1;
        TTabControl *TabControl1;
        TChart *Chart1;
        TFastLineSeries *Series;
        TFastLineSeries *Series1;
        TPointSeries *Series2;
        TChart *Chart2;
        TFastLineSeries *FastLineSeries1;
        TFastLineSeries *FastLineSeries2;
        TPointSeries *PointSeries1;
    void __fastcall StartButtonClick(TObject *Sender);
    void __fastcall ReFreshTimerTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall SaveAs2Click(TObject *Sender);
    void __fastcall ApplicationEvents1Exception(TObject *Sender, Exception *E);
    void __fastcall ApplicationEvents1Message(tagMSG &Msg, bool &Handled);
    void __fastcall StatusBarDrawPanel(TStatusBar *StatusBar,
          TStatusPanel *Panel, const TRect &Rect);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall StartAll(TObject *Sender);
    void __fastcall StopAll(TObject *Sender);
    void __fastcall TTSExecute(TIdPeerThread *AThread);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall lbServersClick(TObject *Sender);
    void __fastcall mthreadCheckBoxClick(TObject *Sender);
    void __fastcall Clearlog1Click(TObject *Sender);
    void __fastcall ScanButtonClick(TObject *Sender);
    void __fastcall cbVerboseClick(TObject *Sender);
    void __fastcall ParseData(AnsiString filename);
    void __fastcall ReadDataFileClick(TObject *Sender);
    void __fastcall CheckBox1Click(TObject *Sender);
    void __fastcall Button3Click(TObject *Sender);
    void __fastcall cbDoSetBRClick(TObject *Sender);
    void __fastcall cbSyncClick(TObject *Sender);
    void __fastcall SyncTimerTimer(TObject *Sender);
    void __fastcall eChIntervalChange(TObject *Sender);
    void __fastcall TTSConnect(TIdPeerThread *AThread);
    void __fastcall TTSDisconnect(TIdPeerThread *AThread);
    void __fastcall CheckBox2Click(TObject *Sender);
    void __fastcall ReconnectClick(TObject *Sender);
    void __fastcall lbServersMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall CheckBox3Click(TObject *Sender);
    void __fastcall pmMasterPopup(TObject *Sender);
    void __fastcall Savetologtxt1Click(TObject *Sender);
    void __fastcall ExtractDataClick(TObject *Sender);
    void __fastcall cbF3004Click(TObject *Sender);
    void __fastcall cbAutoStartClick(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall Chart1DblClick(TObject *Sender);
    void __fastcall PowerManager1Click(TObject *Sender);
        void __fastcall Settings1Click(TObject *Sender);
        void __fastcall TabControl1Change(TObject *Sender);

private:	// User declarations
    bool MI;  //Flash for Monitoring Indicator
    TStrings *err_list;
    AnsiString LogString;
    void __fastcall ThreadDone(TObject *Sender);
    AnsiString __fastcall S_CRB(TIdPeerThread *AThread);
public:		// User declarations
    bool runned;
    virtual __fastcall TMainForm(TComponent* Owner);
    AnsiString Path, ExeFileName;
    TIniFile *Ini;
    TDateTime cdt, startTime, upTime;
    TList *Masters;
    bool ltf;
    std::ofstream logfile;
    bool RichText_busy;
    unsigned int RTBCounter;
    unsigned int ticks;
    bool ss_busy;
    bool stopAll_signal,startAll_signal;
    AnsiString temp_buf_s;
    int __fastcall DoOfflineCommand(TIdPeerThread *AThread);
    void __fastcall ClientListChange(TObject *Sender);
    int popupitm;
    //TStringList *LogBuf;
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
