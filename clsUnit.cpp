#include <vcl.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <vector>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "clsUnit.h"
#include "mainUnit.h"
#include "masterUnit.h"
#include "srvUnit.h"
#include "func.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//----------------------------------------------------- TData constructor ---
__fastcall TData::TData(TIniFile *AIni){
    //f3004 = false; // Use 3004 port and flag system
    int i;
    line_counter = 0;
    error_count = 0;
    SDBCounter = 0;
    partial_read_count = 0;
    inadequate = 0;
    NSR = 0;
    cfc = 0;
    ofc = 0;
    
    busy = false;
    
    ff1 = false;

    Ini = AIni;

    df = Ini->ReadInteger("Options","DebugFlag",1);
    doSetBR = Ini->ReadBool("Options","doSetBR",false);
    doSetPower = Ini->ReadBool("Options","doSetPower",false);
    doSetHV = false;
    //doSetHV = Ini->ReadBool("Options","doSetHV",false);
    allHV = Ini->ReadInteger("Options","allHV",0);
    syncByTimer = Ini->ReadBool("Options","syncThreads",false);
    delay_tune = Ini->ReadBool("Options","DelayTune",true);

    SyncTimer = new TTimer(NULL);
    SyncTimer->Enabled = false;

    MaxFiles = Ini->ReadInteger("Options","MaxFiles",10);
    MaxFileSize = Ini->ReadInteger("Options","MaxFileSize",102400);
    Form = MainForm;

    file_index = 0;
    //AnsiString filename;
    filesize = 0;

    unsigned short y,m,d,h,min,s,ms;
    TDateTime T(Now());
    T.DecodeDate(&y,&m,&d);
    T.DecodeTime(&h,&min,&s,&ms);
    filename = AnsiString().sprintf("%02d",y-2000) +
        AnsiString().sprintf("%02d",m) +
        AnsiString().sprintf("%02d",d) +
        AnsiString().sprintf("%02d",h) +
        AnsiString().sprintf("%02d",min) +
        AnsiString().sprintf("%02d",s) + "." +
        AnsiString().sprintf("%03d",file_index);

    //std::ofstream temp_df(filename.c_str());
    //temp_df.close();
    //datafile.clear();
    //datafile.close();
    filename = Form->Path + "\\datacat\\" + filename;

    datafile.open(filename.c_str(), ios::out | ios::binary);

    dhdr = new TDataHeader();

    dfhdr.DataFileVersion = 1;
    dfhdr.CRC = 0xffff;
    //dfhdr.rawdata = NULL;
    Form->LogMemo->Lines->Add("Size of TDFhdr: " + AnsiString(sizeof(TDFhdr)));
    datafile.write((char*)&dfhdr, sizeof(TDFhdr));
    Form->LogMemo->Lines->Add("Create new data file: " + filename);
    //datafile.flush();
    //datafile.clear();
    //datafile.seekp(0,ios::end);
    //filesize = datafile.tellp();
    /*
    filesize = -1;
    int lastfile = 0;

    filename = "data." + AnsiString().sprintf("%03d",file_index);
    datafile.open(filename.c_str(), ios::in | ios::out | ios::binary | ios::ate);

    while(file_index<MaxFiles){
        if(datafile.is_open()){
            lastfile = file_index;
            filesize = datafile.tellp();
            Form->LogMemo->Lines->Add("File exist: " + filename);
            Form->LogMemo->Lines->Add("File size: " + AnsiString(filesize));
            datafile.close();
        }
        file_index++;
        filename = "data." + AnsiString().sprintf("%03d",file_index);
        datafile.clear();
        datafile.open(filename.c_str(), ios::in | ios::out | ios::binary | ios::ate);
    }

    file_index = lastfile;

    if(file_index<MaxFiles && filesize<=MaxFileSize && filesize>0){
        filename = "data." + AnsiString().sprintf("%03d",file_index);
        datafile.clear();
        datafile.open(filename.c_str(), ios::in | ios::out | ios::binary | ios::ate);
        Form->LogMemo->Lines->Add("Open exist data file: " + filename);
        Form->LogMemo->Lines->Add("File size: " + AnsiString(filesize));
    }else{
        if(filesize > MaxFileSize)file_index++;
        if(file_index>=MaxFiles)file_index = 1;
        filename = "data." + AnsiString().sprintf("%03d",file_index);
        std::ofstream temp_df(filename.c_str());
        dfhdr.DataFileVersion = 1;
        dfhdr.CRC = 0xffff;
        temp_df.write((char*)&dfhdr, sizeof(TDFhdr));
        temp_df.close();
        datafile.clear();
        datafile.open(filename.c_str(), ios::in | ios::out | ios::binary | ios::ate);
        Form->LogMemo->Lines->Add("Create new data file: " + filename);
    }
    */

    



    NumOfServers = Ini->ReadInteger("Servers","NumOfServers",2);
    NumOfClients = Ini->ReadInteger("ClientsPriority","NumOfClients",2);
    AllListSize = Ini->ReadInteger("Monitoring","AllListSize",3);
    ShortListSize = Ini->ReadInteger("Monitoring","ShortListSize",1);
    AnsiString prms = Ini->ReadString("Monitoring","ReadParametersList","100;108;122;");

    MaxURA = Ini->ReadInteger("Options","MaxUnsuccessReadAttempts",3);
    MaxESRA = Ini->ReadInteger("Options","MaxEmptyStringReadAttempts",100);

    ClientsPriority = new AnsiString [NumOfClients];
    ClientLastChannel = new int [NumOfClients];

    ParametersToRead = new int [AllListSize];

    ExtSessionReceivedBytes = new int [NumOfServers];
    ExtSessionSentBytes = new int [NumOfServers];

    if(df)Form->LogMemo->Lines->Add(prms);
    for(i=0;i<AllListSize;i++){
        ParametersToRead[i] = prms.SubString(1,prms.Pos(";")-1).ToIntDef(999);
        prms.Delete(1,prms.Pos(";"));
    }
    
    if(df)Form->LogMemo->Lines->Add(AnsiString(AllListSize));
    if(df)Form->LogMemo->Lines->Add(AnsiString(ShortListSize));

    //Versions Load
    Form->LogMemo->Lines->Add("Loading versions list...");
    versions = new TStringList();
    NumOfVers = Ini->ReadInteger("Monitoring","NumOfVers",0);
    AnsiString VerStr;
    int plim;
    int shortl;
    PArrays = new int*[NumOfVers];
    PASizes = new int [NumOfVers];
    PAShortSizes = new int [NumOfVers];
    for(i=0;i<NumOfVers;i++){
        VerStr = Ini->ReadString("Monitoring","Ver" + AnsiString(i), "");
        versions->Add(VerStr.SubString(1,VerStr.Pos(";")-1));
        VerStr.Delete(1,VerStr.Pos(";"));
        
        plim = 0;

        plim = VerStr.SubString(1,VerStr.Pos(";")-1).ToIntDef(0);
        VerStr.Delete(1,VerStr.Pos(";"));

        if(plim){
            PASizes[i] = plim;
            PAShortSizes[i] = VerStr.SubString(1,VerStr.Pos(";")-1).ToIntDef(0);
            VerStr.Delete(1,VerStr.Pos(";"));

            PArrays[i] = new int [plim];
            for(int j=0;j<plim;j++){
                PArrays[i][j] = VerStr.SubString(1,VerStr.Pos(";")-1).ToIntDef(0);
                VerStr.Delete(1,VerStr.Pos(";"));
            }
            Form->LogMemo->Lines->Add("Ver " + AnsiString(i) + " = " + versions->Strings[i] + ", " + AnsiString(plim) + " parameters added");
        }else{
            PASizes[i] = AllListSize;
            PAShortSizes[i] = ShortListSize;
            PArrays[i] = new int [AllListSize];
            for(int j=0;j<AllListSize;j++){
                PArrays[i][j] = ParametersToRead[j];
            }
            Form->LogMemo->Lines->Add("Ver " + AnsiString(i) + " = " + versions->Strings[i]);
        }
    }

    //Servers configuration loading
    srvrs = new TServer*[NumOfServers];
    AnsiString str;
    for(i=0;i<NumOfServers;i++){
        str = Ini->ReadString("Servers","Server_"+AnsiString(i),"0.0.0.0;active;");
        if(df)Form->LogMemo->Lines->Add(str);
        srvrs[i] = new TServer(str);

        for(int j=0;j<srvrs[i]->NumOfChannels;j++){
            srvrs[i]->Channels[j]->PASize = PASizes[srvrs[i]->Channels[j]->Version];
            srvrs[i]->Channels[j]->PAShortSize = PAShortSizes[srvrs[i]->Channels[j]->Version];
            srvrs[i]->Channels[j]->PArray = PArrays[srvrs[i]->Channels[j]->Version];
            if(df)Form->LogMemo->Lines->Add("Channel " + AnsiString(srvrs[i]->Channels[j]->RelayChannel)  + " version: " + AnsiString(srvrs[i]->Channels[j]->Version));
            if(df)Form->LogMemo->Lines->Add("Channel " + AnsiString(srvrs[i]->Channels[j]->RelayChannel)  + " params: " + srvrs[i]->Channels[j]->GetParamsList());
        }

        if(df)Form->LogMemo->Lines->Add(srvrs[i]->GetConfigString());
        //if(df)Form->LogMemo->Lines->Add(srvrs[i]->Address);
        //if(df)Form->LogMemo->Lines->Add(srvrs[i]->Port);
        //if(df)Form->LogMemo->Lines->Add(AnsiString(srvrs[i]->Channels[0]->Version));
    }
    for(i=0;i<NumOfClients;i++){
        ClientsPriority[i] = Ini->ReadString("ClientsPriority","Client_"+AnsiString(i),"0.0.0.0");
    }

    //Timings Initialization
    WaitForConnectionInterval = Ini->ReadInteger("Timings","WaitForConnectionInterval",6000);
    ParameterInterval = Ini->ReadInteger("Timings","ParameterInterval",100);
    WaitIfReadErrorInterval = Ini->ReadInteger("Timings","WaitIfReadErrorInterval",2000);
    WaitForResponseInterval = Ini->ReadInteger("Timings","WaitForResponseInterval",2000);
    XLInterval = Ini->ReadInteger("Timings","XLInterval",10000);
    SleepModeInterval = Ini->ReadInteger("Timings","SleepModeInterval",10000);
    FCI = Ini->ReadInteger("Timings","ChannelsInterval",2000);
    SyncTimer->Interval = FCI;
    PermissionRequestInterval = Ini->ReadInteger("Timings","PermissionRequestInterval",1000);

    AnsiString m_num;
    hosts_string = "(";
    ports_string = "(";
    for(int i=0;i<NumOfServers-1;i++){
        m_num = srvrs[i]->Address;
        while(m_num.Pos('.'))m_num.Delete(1,m_num.Pos('.'));
        hosts_string = hosts_string + m_num + " ";
        ports_string = ports_string + AnsiString(srvrs[i]->LocalPort) + " ";
    }
    m_num = srvrs[NumOfServers-1]->Address;
    while(m_num.Pos('.'))m_num.Delete(1,m_num.Pos('.'));
    hosts_string = hosts_string + m_num + ")\xD\xA" ;
    ports_string = ports_string + AnsiString(srvrs[NumOfServers-1]->LocalPort) + ")\xD\xA";

    f3004 = Ini->ReadBool("Options","f3004",false);

    AutoStart = Ini->ReadBool("Options","AutoStart",false);

    LogListA = new TStringList();
    LogListB = new TStringList();
    //pgBuffer = new TStringList();
    p2A = true;
    pLogList = LogListA;
    gLogList = LogListB;

    LogBuf = new TStringList();
}
//------------------------------------------------------ TData destructor ---
__fastcall TData::~TData(void){
    int i;

    //Timings Saving
    Ini->WriteInteger("Timings","WaitForConnectionInterval",WaitForConnectionInterval);
    Ini->WriteInteger("Timings","ParameterInterval",ParameterInterval);
    Ini->WriteInteger("Timings","WaitIfWriteErrorInterval",WaitIfReadErrorInterval);
    Ini->WriteInteger("Timings","WaitForResponseInterval",WaitForResponseInterval);
    Ini->WriteInteger("Timings","XLInterval",XLInterval);
    Ini->WriteInteger("Timings","SleepModeInterval",SleepModeInterval);
    Ini->WriteInteger("Timings","ChannelsInterval",FCI);
    Ini->WriteInteger("Timings","PermissionRequestInterval",PermissionRequestInterval);

    //Versions Saving
    for(i=0;i<NumOfVers;i++) delete [] PArrays[i];
    delete [] PArrays;
    delete [] PASizes;
    delete [] PAShortSizes;
    
    if(NumOfVers < versions->Count){ //write if new versions added
        for(i=NumOfVers;i<versions->Count;i++){
            Ini->WriteString("Monitoring","Ver" + AnsiString(i), versions->Strings[i] + ";");
        }
        Ini->WriteInteger("Monitoring","NumOfVers",versions->Count);
    }

    delete versions;

    Ini->WriteInteger("Options","DebugFlag",df);

    Ini->WriteBool("Options","doSetBR",doSetBR);

    Ini->WriteBool("Options","doSetPower",doSetPower);

    //Ini->WriteBool("Options","doSetHV",doSetHV);
    Ini->WriteInteger("Options","allHV",allHV);

    Ini->WriteBool("Options","syncThreads",syncByTimer);

    Ini->WriteBool("Options","DelayTune",delay_tune);

    Ini->WriteBool("Options","f3004",f3004);

    Ini->WriteBool("Options","AutoStart",AutoStart);

    delete SyncTimer;

    Ini->WriteInteger("Misc","ErrorCount",error_count);
    
    Ini->WriteInteger("Servers", "NumOfServers", NumOfServers);


    for(i=0;i<NumOfServers;i++){
        Ini->WriteString("Servers","Server_"+AnsiString(i),srvrs[i]->GetConfigString());
    }

    for(i=0;i<NumOfClients;i++){
        Ini->WriteString("ClientsPriority","Client_"+AnsiString(i),ClientsPriority[i]);
    }

    delete [] ClientsPriority;
    delete [] ClientLastChannel;
    delete [] ParametersToRead;
    delete [] ExtSessionReceivedBytes;
    delete [] ExtSessionSentBytes;
    
    for(i=0;i<NumOfServers;i++)delete srvrs[i];
    delete [] srvrs;

    delete dhdr;

    datafile.close();

    delete LogListA;
    delete LogListB;
    delete LogBuf;
}
//---------------------------------------------------------------------------
void __fastcall TData::NextFile(void){
    file_index++;
    unsigned int ex_file_index;
    AnsiString tmp_fn;

    if(file_index>=MaxFiles)file_index=0;

    unsigned short y,m,d,h,min,s,ms;
        TDateTime T(Now());
        T.DecodeDate(&y,&m,&d);
        T.DecodeTime(&h,&min,&s,&ms);
        filename = AnsiString().sprintf("%02d",y-2000) +
                   AnsiString().sprintf("%02d",m) +
                   AnsiString().sprintf("%02d",d) +
                   AnsiString().sprintf("%02d",h) +
                   AnsiString().sprintf("%02d",min) +
                   AnsiString().sprintf("%02d",s) + "." +
                   AnsiString().sprintf("%03d",file_index);

    filename = filepath + "\\datacat\\" + filename;

    dfhdr.DataFileVersion = 1;
    dfhdr.CRC = 0xffff;
    dfhdr.rawdata[0] = 0x20;
    datafile.seekp(0);
    datafile.write((char*)&dfhdr, sizeof(TDFhdr));

    do {
        //cfc++;
        if(!FileExists(filename))datafile.close();
    }while(datafile.is_open());

    while(!datafile.is_open()){
        //ofc++;
        ex_file_index = 0;
        while(FileExists(filename)){
            filename = AnsiString().sprintf("%02d",y-2000) +
                    AnsiString().sprintf("%02d",m) +
                    AnsiString().sprintf("%02d",d) +
                    AnsiString().sprintf("%02d",h) +
                    AnsiString().sprintf("%02d",min) +
                    AnsiString().sprintf("%02d",s) +
                    "_" +
                    AnsiString().sprintf("%d",ex_file_index) +
                    "." +
                    AnsiString().sprintf("%03d",file_index);
            ex_file_index++;
            }
        datafile.open(filename.c_str(), ios::out | ios::binary);
        ::Sleep(50);
    }

    dfhdr.DataFileVersion = 1;
    dfhdr.CRC = 0xffff;
    datafile.write((char*)&dfhdr, sizeof(TDFhdr));

    //do {
    //    datafile.close();
    //}while(datafile.is_open());
    //datafile.operator =() = tempfile;
    //datafile = tempfile;
    //ufffile.flush();
    //while(bu)bufffile.close();
}
//---------------------------------------------------------------------------
void __fastcall TData::StoreData(TDateTime APCTime, AnsiString AMasterAddress, TChannel *AChannel, int AParameter, int AValueSize, AnsiString s){

    if(df>3)Form->LogMemo->Lines->Add("Write to data file...");
    cfc++;
    dhdr->AddData(APCTime, AMasterAddress, AChannel, AParameter, AValueSize);
    datafile.write((char *)&dhdr->hdr, sizeof(dhdr->hdr));
    if(s.Length())datafile.write(s.c_str(), s.Length());
    do {
        filesize = datafile.tellp();
    } while(filesize < 0);
    if(filesize>=MaxFileSize)NextFile();
    ofc++;
}
//---------------------------------------------------------------------------
void TData::SetCI(unsigned int ACI){
    FCI = ACI;
    SyncTimer->Interval = FCI;
}
//---------------------------------------------------------------------------
int TData::PMCommand(AnsiString s, int Amdi, TIdPeerThread *AThread){
    //Form->AddLogString("PMCommand(" + StrToHexStr(s) + ") called.");
    switch(s.c_str()[0]){
    case 'S' :
            //Form->AddLogString("Start signal seceived from " + AnsiString(Amdi));
            {
                AThread->Connection->Write(Data->hosts_string);
                AThread->Connection->Write(Data->ports_string);
                //AThread->Connection->Disconnect();
                if(!Form->runned && !Form->ss_busy){
                    Form->ss_busy = true;
                    Form->startAll_signal = true;
                    Form->ReFreshTimer->Enabled = true;
                    return 1;
                    //Form->StartButtonClick(NULL);
                    //Form->AddLogString("System STARTED by command.");
                }else{
                    //Form->AddLogString("System always runned or busy. Do nothing.");
                }
                return 2;
            }// do start of system
    case 'K' :
            //Form->AddLogString("Stop signal seceived from " + AnsiString(Amdi));
            {
                if(Form->runned && !Form->ss_busy){
                    Form->ss_busy = true;
                    Form->stopAll_signal = true;
                    return 3;
                    //Form->AddLogString("Stop flag setted.");
                }else{
                    //Form->AddLogString("System not runned or busy. Do nothing.");
                }
                return 4;
            }// do stop of system
    }
    return 0;
}
//---------------------------------------------------------------------------
//void TData::SwapLog(void){
//    pLogList = gLogList;
//    if(p2A)gLogList = LogListA;
//    else gLogList = LogListB;
//    p2A = !p2A;
//}
//---------------------------------------------------------------------------
void __fastcall TData::AddLogString(AnsiString str){
    line_counter++;
    //TDateTime T(Now());
    //pLogList->Add("L" + AnsiString().sprintf("%06d",Data->line_counter) + " " + T.TimeString() + ":" + str);
    //pgBuffer->Add("L" + AnsiString().sprintf("%06d",Data->line_counter) + " " + T.TimeString() + ":" + str);
}
//---------------------------------------------------------------------------
