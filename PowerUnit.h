//---------------------------------------------------------------------------

#ifndef PowerUnitH
#define PowerUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <CheckLst.hpp>
#include <ExtCtrls.hpp>
#include <DateUtils.hpp>
#include "clsUnit.h"
#include <Sockets.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdUDPBase.hpp>
#include <IdUDPClient.hpp>
#include <IdUDPServer.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TPowerThread : public TThread{
private:
    TData *D;
    TObject *FOwner;
    void __fastcall VALS(void);
    AnsiString VALS_BUF;
    TColor MessageColor;

protected:
    void __fastcall Execute(void);
public:

    TTimer *SuspendTimer;
    TTimer *ResponseTimer;

    __fastcall TPowerThread(TObject *Owner);
    __fastcall ~TPowerThread(void);

    void __fastcall SuspendTimerTimer(TObject *Sender);
    void __fastcall ResponseTimerTimer(TObject *Sender);
    void __fastcall WaitForResponse(int AWaitTime);

    void __fastcall PowerThreadTerminate(TObject *Sender);

    void __fastcall ALS(AnsiString astr);
    void __fastcall ALS(AnsiString astr, TColor color);
};
//---------------------------------------------------------------------------
extern PACKAGE TPowerThread *PowerThread;
//---------------------------------------------------------------------------
class TPowerForm : public TForm
{
__published:	// IDE-managed Components
    TCheckListBox *PowerCLB;
    TLabeledEdit *LabeledEdit1;
    TLabeledEdit *LabeledEdit2;
    TLabeledEdit *LabeledEdit3;
    TListBox *ListBox1;
    TButton *Button1;
    TCheckBox *CheckBox1;
    TCheckBox *CheckBox2;
    TUdpSocket *UdpSocket1;
    TButton *Button2;
    TButton *Button3;
    TIdUDPServer *IdUDPServer1;
    TButton *Button4;
    TLabeledEdit *LabeledEdit4;
    TUdpSocket *UdpSocket2;
    TLabeledEdit *LabeledEdit5;
    TLabeledEdit *LabeledEdit6;
    TLabeledEdit *LabeledEdit7;
    TLabeledEdit *LabeledEdit8;
    TLabeledEdit *LabeledEdit9;
    TLabeledEdit *LabeledEdit10;
    TUpDown *UpDown1;
    TUpDown *UpDown2;
    TLabel *Label1;
    TLabel *Label2;
    TLabeledEdit *LabeledEdit11;
    TButton *Button5;
    TCheckBox *CheckBox3;
    TLabel *Label3;
        TEdit *Edit1;
        TCheckBox *CheckBox4;
    //TPowerThread *PowerThread;

    void __fastcall PowerCLBClickCheck(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall UdpSocket1Receive(TObject *Sender, PChar Buf,
          int &DataLen);
    void __fastcall UdpSocket1Send(TObject *Sender, PChar Buf,
          int &DataLen);
    void __fastcall UdpSocket1CreateHandle(TObject *Sender);
    void __fastcall UdpSocket1Error(TObject *Sender, int SocketError);
    void __fastcall Button3Click(TObject *Sender);
    void __fastcall IdUDPServer1UDPRead(TObject *Sender, TStream *AData,
          TIdSocketHandle *ABinding);
    void __fastcall Button4Click(TObject *Sender);
    void __fastcall PowerCLBClick(TObject *Sender);
    void __fastcall LabeledEdit1Change(TObject *Sender);
    void __fastcall LabeledEdit2Change(TObject *Sender);
    void __fastcall LabeledEdit5Change(TObject *Sender);
    void __fastcall LabeledEdit6Change(TObject *Sender);
        void __fastcall Edit1Change(TObject *Sender);
        void __fastcall CheckBox4Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    TData *D;
    TPowerThread *PThrd;

    __fastcall TPowerForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TPowerForm *PowerForm;
//---------------------------------------------------------------------------
#endif
