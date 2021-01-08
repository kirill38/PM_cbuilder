//---------------------------------------------------------------------------
#ifndef srvUnitH
#define srvUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <DateUtils.hpp>
//---------------------------------------------------------------------------
class TChannel{
private:
protected:
public:
    int Relay;
    int RelayChannel;
    int Pair;
    int RSBR;
    int Address;
    int Version;

    int PASize;
    int PAShortSize;
    int *PArray;

    __fastcall TChannel(AnsiString AChan);
    __fastcall ~TChannel(void);

    AnsiString __fastcall GetParamsList(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TChannel *ch;
//---------------------------------------------------------------------------
class TServer{
private:
protected:
public:
    AnsiString Address;
    bool Active;
    //bool lp3004;
    int NumOfChannels;
    int ActiveChannel;
    int Port;
    int LocalPort;
    TChannel **Channels;
    __fastcall TServer(AnsiString str);
    __fastcall ~TServer(void);

    __property AnsiString ConfigString = { read=GetConfigString };
    AnsiString __fastcall GetConfigString(void);

};
//---------------------------------------------------------------------------
extern PACKAGE TServer *srvrs;
//---------------------------------------------------------------------------
struct Thdr {
    unsigned __int16 DataType;
    TDateTime PCTime;
    unsigned __int16 GetDTms;
    unsigned char masterIP[4];  //master address
    unsigned char Relay;
    unsigned char RelayChannel;
    unsigned char Pair;
    unsigned char Address;
    unsigned char Parameter;
    unsigned __int16 ValueSize; //array of strings
};
//---------------------------------------------------------------------------
struct ThdrPlus {
    unsigned __int16 DataType;
    TDateTime PCTime;
    unsigned __int16 GetDTms;
    unsigned char masterIP[4];  //master address
    unsigned char Relay;
    unsigned char RelayChannel;
    unsigned char Pair;
    unsigned char Address;
    unsigned char Parameter;
    unsigned __int16 ValueSize; //array of strings
    AnsiString Value;
};
//---------------------------------------------------------------------------
class TDataHeader{
private:
    TChannel *FChannel;
protected:
public:
    Thdr hdr;
    __fastcall TDataHeader(void);
    __fastcall ~TDataHeader(void);
    void __fastcall TDataHeader::AddData(TDateTime APCTime, AnsiString AMasterAddress, TChannel *AChannel, int AParameter, int AValueSize);
};
//---------------------------------------------------------------------------
extern PACKAGE TDataHeader *dr;
//---------------------------------------------------------------------------
class TCInfo : public TObject {
public:
    int LastChannelNumber;
    __fastcall TCInfo(AnsiString addr);
    __fastcall ~TCInfo(void);
    AnsiString IP;
};
//---------------------------------------------------------------------------
extern PACKAGE TCInfo *ci;
//---------------------------------------------------------------------------
#endif
 