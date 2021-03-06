//---------------------------------------------------------------------------
#pragma hdrstop
#include "srvUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//-------------------------------------------------- TChannel constructor ---
__fastcall TChannel::TChannel(AnsiString str){
    Relay = str.SubString(1,str.Pos(".")-1).ToIntDef(0);str.Delete(1,str.Pos("."));
    RelayChannel = str.SubString(1,str.Pos(".")-1).ToIntDef(0);str.Delete(1,str.Pos("."));
    Pair = str.SubString(1,str.Pos(".")-1).ToIntDef(0);str.Delete(1,str.Pos("."));
    RSBR = str.SubString(1,str.Pos(".")-1).ToIntDef(0);str.Delete(1,str.Pos("."));
    Address = str.SubString(1,str.Pos(".")-1).ToIntDef(0);str.Delete(1,str.Pos("."));
    Version = str.SubString(1,str.Pos(";")-1).ToIntDef(0);str.Delete(1,str.Pos(";"));
}
//-------------------------------------------------- TChannel constructor ---
__fastcall TChannel::TChannel(int ARelay, int ARelayChannel, int APair, int ARSBR, int AAddress, int AVersion){
    Relay = ARelay;
    RelayChannel = ARelayChannel;
    Pair = APair;
    RSBR = ARSBR;
    Address = AAddress;
    Version = AVersion;
}
//-------------------------------------------------- TChannel destructor ---
__fastcall TChannel::~TChannel(void){
}
//-------------------------------------------------- TSensor constructor ---
__fastcall TSensor::TSensor(int AI2CAddress, int ASensorType){
    I2CAddress = AI2CAddress;
    SensorType = ASensorType;
}
//-------------------------------------------------- TSensor destructor ---
__fastcall TSensor::~TSensor(void){
}
//---------------------------------------------------- TServer constructor ---
__fastcall TServer::TServer(AnsiString str){

    Address = str.SubString(1,str.Pos(";")-1);str.Delete(1,str.Pos(";"));
    Active = (str.SubString(1,str.Pos(";")-1) == "active" ? true:false);str.Delete(1,str.Pos(";"));
    Port = str.SubString(1,str.Pos(";")-1).ToIntDef(0);str.Delete(1,str.Pos(";"));
    //if(Port==3004)lp3004 = true;
    //else lp3004 = false;
    LocalPort = str.SubString(1,str.Pos(";")-1).ToIntDef(0);str.Delete(1,str.Pos(";"));
    NumOfChannels = str.SubString(1,str.Pos(";")-1).ToIntDef(0);str.Delete(1,str.Pos(";"));
    ActiveChannel = str.SubString(1,str.Pos(";")-1).ToIntDef(0);str.Delete(1,str.Pos(";"));

    Channels = new TChannel*[NumOfChannels];
    Sensors = new TList();

    for(int i=0;i<NumOfChannels;i++){
        Channels[i] = new TChannel(str);
        str.Delete(1,str.Pos(";"));
    }
}
//---------------------------------------------------- TServer destructor ---
__fastcall TServer::~TServer(void){
    for(int i=0;i<NumOfChannels;i++){
        delete Channels[i];
    }
    delete [] Channels;
    if(Sensors->Count)ClearSensorsList();
    delete Sensors;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TServer::GetConfigString(void){
    AnsiString str;
    str =
    Address + ";" +
    (Active ? "active":"suspend") + ";" +
    AnsiString(Port) + ";" +
    AnsiString(LocalPort) + ";" +
    AnsiString(NumOfChannels) + ";" +
    AnsiString(ActiveChannel) + ";";

    for(int i=0;i<NumOfChannels;i++){
        str = str +
        AnsiString(Channels[i]->Relay) + "." +
        AnsiString(Channels[i]->RelayChannel) + "." +
        AnsiString(Channels[i]->Pair) + "." +
        AnsiString(Channels[i]->RSBR) + "." +
        AnsiString(Channels[i]->Address) + "." +
        AnsiString(Channels[i]->Version) + ";";
    }
    return str;
}
//---------------------------------------------------------------------------
__fastcall TServer::AddSensor(int AI2CAddress, int ASensorType){
    PSensor = new TSensor(AI2CAddress, ASensorType);
    Sensors->Add(PSensor);
    return 0;
}
//---------------------------------------------------------------------------
__fastcall TServer::ClearSensorsList(void){
    for(int i_list=0; i_list<Sensors->Count; i_list++){
        PSensor = (TSensor*) Sensors->Items[i_list];
        delete PSensor;
    }
    Sensors->Clear();
    return 0;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TChannel::GetParamsList(void){
    AnsiString str;
    str = AnsiString(PASize) + ";" + AnsiString(PAShortSize) + ";";
    for(int i=0; i<PASize; i++){
        str = str + AnsiString(PArray[i]) + ";";
    }
    return str;
}
//---------------------------------------------------------------------------
void __fastcall TDataHeader::AddData(TDateTime APCTime, AnsiString AMasterAddress, TChannel *AChannel, int AParameter, int AValueSize){
    hdr.PCTime = Now();
    hdr.DataType = 0;
    for(int i=0;i<3;i++){
        hdr.masterIP[i] = AMasterAddress.SubString(1,AMasterAddress.Pos(".")-1).ToIntDef(0);AMasterAddress.Delete(1,AMasterAddress.Pos("."));
    }
    hdr.masterIP[3] = AMasterAddress.ToIntDef(0);
    FChannel = AChannel;
    hdr.Relay = FChannel->Relay;
    hdr.RelayChannel = FChannel->RelayChannel;
    hdr.Pair = FChannel->Pair;
    hdr.Address = FChannel->Address;
    hdr.Parameter = AParameter;
    hdr.ValueSize = AValueSize;
    hdr.GetDTms = (unsigned __int16)MilliSecondsBetween(hdr.PCTime, APCTime);
}
//---------------------------------------------------------------------------
void __fastcall TDataHeader::AddMasterData(TDateTime APCTime, AnsiString AMasterAddress, int AParameter, int AValueSize){
    hdr.PCTime = Now();
    hdr.DataType = 1;
    for(int i=0;i<3;i++){
        hdr.masterIP[i] = AMasterAddress.SubString(1,AMasterAddress.Pos(".")-1).ToIntDef(0);AMasterAddress.Delete(1,AMasterAddress.Pos("."));
    }
    hdr.masterIP[3] = AMasterAddress.ToIntDef(0);

    hdr.Relay = 255;
    hdr.RelayChannel = 255;
    hdr.Pair = 255;
    hdr.Address = 255;
    hdr.Parameter = AParameter;
    hdr.ValueSize = AValueSize;
    hdr.GetDTms = (unsigned __int16)MilliSecondsBetween(hdr.PCTime, APCTime);
}
//---------------------------------------------------------------------------
__fastcall TDataHeader::TDataHeader(void){
}
//---------------------------------------------------------------------------
__fastcall TDataHeader::~TDataHeader(void){
}
//---------------------------------------------------------------------------
__fastcall TCInfo::TCInfo(AnsiString addr){
    IP = addr;
    LastChannelNumber = -1;
    //TIdPeerThread* PeerThread;
}
//---------------------------------------------------------------------------
__fastcall TCInfo::~TCInfo(void){

}
//---------------------------------------------------------------------------
