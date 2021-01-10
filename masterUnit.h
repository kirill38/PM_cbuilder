//---------------------------------------------------------------------------
#ifndef masterUnitH
#define masterUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <IdTCPServer.hpp>
#include <IdTCPClient.hpp>
#include <DateUtils.hpp>
#include "clsUnit.h"
#include "mainUnit.h"
#include "srvUnit.h"
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TOMChannel{
private:
protected:
public:
    __fastcall TOMChannel(void);
    __fastcall ~TOMChannel(void);

    int state;  //-1 - undefined, 0 - off, 1 - on
    int cc_uptime;
    int RelayChannel;
    int RSBR;
    int RS_Address; //-1 - undefined, 0 - 255 (0-11) controller address
    int SensorsState; // -1 - undefined, sensors state of controller

    int paramIndex, paramNum, paramNumShort;
    int *ParamsToRead;

//Temporary declarations
};
//---------------------------------------------------------------------------
extern PACKAGE TOMChannel *OMChan;
//---------------------------------------------------------------------------
class TMThread : public TThread{
private:
    bool FNC;
    bool FES;
    bool ND; //no delay reading flag. For reading A and M

    TData *D;
    TObject *FOwner;

    TSensor *PS;
    int Pparameter;

    void __fastcall Next(void);
    void __fastcall Next2(void);

    void __fastcall VALS(void);
    AnsiString VALS_BUF;
    TColor MessageColor;

    //int paramIndex, paramNum, paramNumShort, chanIndex, chanNum, XLIndex;
    //int *ParamsToRead;
    //int *ChansToRead;
    //int Fmdi;

    TDataHeader *dh;

protected:
    void __fastcall Execute(void);
public:
    __property bool ES = { read=GetES, write=SetES };        //External Session flag
    void __fastcall SetES(bool AES);
    bool __fastcall GetES(void);
    __property int RelChan = { read=GetRelChan };
    int __fastcall GetRelChan(void);
    __property int Param = { read=GetParam };
    int __fastcall GetParam(void);

    __property bool NC = { read=GetNC, write=SetNC };       //No connection flag
    void __fastcall SetNC(bool ANC);
    bool __fastcall GetNC(void);

                    
    bool XL;        //Extended List flag setted by XLTimer
    //bool PF;        //Pulse flag
    bool getstart;
    bool stopForSync;
    bool pow_set;

    TTimer *SuspendTimer;
    TTimer *ResponseTimer;
    TTimer *XLTimer;
    float rate;
    unsigned long count,last;

    TDateTime Fdt,Adt,T,T3;
    unsigned int Tdiff;

    __fastcall TMThread(TObject *Owner);
    __fastcall ~TMThread(void);

    int __fastcall SCP(void);
    
    int __fastcall SuspendForES(int AWaitTime);
    void __fastcall SuspendTimerTimer(TObject *Sender);
    void __fastcall WaitForResponse(int AWaitTime);
    void __fastcall ResponseTimerTimer(TObject *Sender);
    void __fastcall XLTimerTimer(TObject *Sender);
    void __fastcall MThreadTerminate(TObject *Sender);

    void __fastcall GetParameterCycle(void);

    void __fastcall ALS(AnsiString astr);
    void __fastcall ALS(AnsiString astr, TColor color);

    void __fastcall ESB(void);

    void __fastcall WaitSDB(void);
    void __fastcall StoreData_Sync(void);
    void __fastcall StoreMasterData_Sync(void);
    AnsiString StoreData_s;

    int paramIndex, paramNum, paramNumShort, chanIndex, chanNum, XLIndex;
    int *ParamsToRead;
    int *ChansToRead;
    int Fmdi;
    unsigned char *delays;
};
//---------------------------------------------------------------------------
extern PACKAGE TMThread *MThread;
//---------------------------------------------------------------------------
class TMaster : TObject{
private:
    AnsiString* state_string;
    AnsiString* Sstate_string;


    TIdTCPServer *FServer;
    int FPort;
    bool Fmthread;

    // Form functions
    void __fastcall leCurrChanChange(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall CheckBox1Click(TObject *Sender);
    void __fastcall lbChannelsClick(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);

    void __fastcall ExitES(TIdPeerThread *AThread);
    
protected:
public:
    __fastcall TMaster(TData* AData, int Amdi);
    __fastcall ~TMaster(void);

    TIdTCPClient* Client;
    TMainForm *FInfoOb;
    TMThread *MThrd;

    TStringList* C_List;
    TStringList* Ch_ListA;
    TStringList* Ch_ListB;
    bool clA;

    // Properties
    __property TMainForm *InfoOb = { read=GetInfoOb, write=SetInfoOb };
    TMainForm* __fastcall GetInfoOb(void);
    void __fastcall SetInfoOb(TMainForm *AInfoOb);
    __property TIdTCPServer *Server = { read=GetServer, write=SetServer };
    TIdTCPServer* __fastcall GetServer(void);
    void __fastcall SetServer(TIdTCPServer *AServer);
    __property int Port = { read=GetPort, write=SetPort };
    int __fastcall GetPort(void);
    void __fastcall SetPort(int APort);
    __property bool mthread = { read=Fmthread, write=Setmthread };
    void __fastcall Setmthread(bool Amthread);

    void __fastcall RefreshInfo(TObject *Sender);

    int __fastcall ClientWrite(AnsiString str);
    AnsiString __fastcall ClientRead(int timeout);

    int __fastcall SendToRS(AnsiString sendStr);
    AnsiString __fastcall ReadFromRS(void);

    AnsiString __fastcall GetCCParam(int ccAddr, int param);
    int __fastcall SetOneCCParam(int ccAddr, int param, double value);
    AnsiString __fastcall CheckAdequate(int param, AnsiString str);
    int __fastcall GetPowerState(void);
    bool __fastcall Permitted(void);
    bool __fastcall Permitted2(void);
    
    bool __fastcall FreeRS(void);
    bool __fastcall FreeRS2(void);

    bool __fastcall I2CDevReadByte(unsigned char dev, unsigned char reg, unsigned char &val);
    bool __fastcall I2CDevWriteByte(unsigned char dev, unsigned char reg, unsigned char val);


    bool __fastcall I2CGetBytes(unsigned char dev, unsigned char byte_count, unsigned char *data);
    bool __fastcall I2CSHT20GetRH(double &RH);
    bool __fastcall I2CSHT20GetTemp(double &Temp);
    bool __fastcall I2CSHT20GetUserReg(unsigned char &UserReg);
    bool __fastcall I2CSHT20SetUserReg(unsigned char UserReg);

    bool __fastcall I2CLPS331APGetWhoAmIReg(unsigned char &WhoAmIReg);

    int __fastcall SelectChannel(int channel);
    int __fastcall SetBaudrate(int brate);
    int __fastcall SetPower(int state);

    AnsiString __fastcall C_CRB(void);
    AnsiString __fastcall S_CRB(TIdPeerThread *AThread);

    void __fastcall ExtSession(TIdPeerThread *AThread);

    void __fastcall Scan(bool APower, bool ShortList);

    TOMChannel *Channels;
    TOMChannel *CurrentChannelRef;

    int CurrChan;
    int EIBytesR, EIBytesT;
    TData *D;
    int MDataIndex;
    bool MI;
    bool EI;
    AnsiString temp_buf_c, temp_buf_s;
    bool RW;
    int URA;        // Unsuccess read attempts
    int ESRA;       // Empty string read attempts
    unsigned int error_count;
};
//---------------------------------------------------------------------------
extern TMaster *Master;
//---------------------------------------------------------------------------
#endif
