//---------------------------------------------------------------------------
#include <vcl.h>
#include <time.h>
#pragma hdrstop
#include "masterUnit.h"
#include "mainUnit.h"
#include "func.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//----------------------------------------------------- TMaster functions ---
__fastcall TMaster::TMaster(TData* AData, int Amdi){
    D = AData;
    RW = false;
    MI = false;
    EI = false;
    URA = 0;

    temp_buf_s = "";
    temp_buf_c = "";

    error_count = 0;

    FServer = NULL;
    FPort = D->srvrs[Amdi]->LocalPort;

    C_List = new TStringList();
    Ch_ListA = new TStringList();
    Ch_ListB = new TStringList();
    clA = true;

    Channels = new TOMChannel [12]; // не должно быть 12 должно быть в соответствии с конфигурацией
    Client = new TIdTCPClient(NULL);
    Client->Host = D->srvrs[Amdi]->Address;
    if(D->f3004) Client->Port = 3004;
    else Client->Port = D->srvrs[Amdi]->Port;

    //InfoOb = new TForm1(NULL);
    //FInfoOb->Show();
    FInfoOb = D->Form;
    state_string = new AnsiString [3];
    state_string[0] = "???";
    state_string[1] = "off";
    state_string[2] = "on ";
    Sstate_string = new AnsiString [9];
    Sstate_string[0] = "   ";
    Sstate_string[1] = " ok";
    Sstate_string[2] = " -h";
    Sstate_string[3] = " -m";
    Sstate_string[4] = "-hm";
    Sstate_string[5] = " -p";
    Sstate_string[6] = "-tp";
    Sstate_string[7] = "-mp";
    Sstate_string[8] = "---";
    for(int i=0;i<D->srvrs[Amdi]->NumOfChannels;i++){
        Channels[D->srvrs[Amdi]->Channels[i]->RelayChannel].RS_Address = D->srvrs[Amdi]->Channels[i]->Address;
        //Channels[D->srvrs[Amdi]->Channels[i]->RelayChannel].RSBR = D->srvrs[Amdi]->Channels[i]->RSBR;
        Channels[D->srvrs[Amdi]->Channels[i]->RelayChannel].ParamsToRead = D->srvrs[Amdi]->Channels[i]->PArray;
        Channels[D->srvrs[Amdi]->Channels[i]->RelayChannel].paramNum = D->srvrs[Amdi]->Channels[i]->PASize;
        Channels[D->srvrs[Amdi]->Channels[i]->RelayChannel].paramNumShort = D->srvrs[Amdi]->Channels[i]->PAShortSize;

        Channels[D->srvrs[Amdi]->Channels[i]->RelayChannel].RelayChannel = D->srvrs[Amdi]->Channels[i]->RelayChannel;

        D->Form->LogMemo->Lines->Add(AnsiString(D->srvrs[Amdi]->Channels[i]->RelayChannel) + ": " + D->srvrs[Amdi]->Channels[i]->GetParamsList());
    }
    CurrChan = -1;

    for(int i=0;i<12;i++){
        Ch_ListA->Add(AnsiString( i == CurrChan ? ">":" ") +
                AnsiString(Channels[i].RS_Address>0 ? AnsiString().sprintf("%3d",Channels[i].RS_Address).c_str():"   ") + " " +
                state_string[Channels[i].state+1] + " " +
                AnsiString(Sstate_string[Channels[i].SensorsState+1]));
        Ch_ListB->Add(Ch_ListA->Strings[i]);
    }

    Fmthread = D->srvrs[Amdi]->Active;
    MDataIndex = Amdi;
    //C_List->OnChange = Data->Form->ClientListChange;

    MThrd = new TMThread((TObject*)this);
}
//----------------------------------------------------- TMaster functions ---
__fastcall TMaster::~TMaster(void){
    MThrd->Terminate();
    D->Form->LogMemo->Lines->Add("Terminate()...ok");
    MThrd->Resume();
    D->Form->LogMemo->Lines->Add("Resume()...ok");
    MThrd->WaitFor();
    D->Form->LogMemo->Lines->Add("WaitFor()...ok");
    delete MThrd;
    delete Client;
    delete [] Channels;
    delete [] state_string;
    delete [] Sstate_string;
    D->srvrs[MDataIndex]->Active = mthread;

    for(int i=0;i<C_List->Count;i++){
        delete C_List->Objects[i];      // clearing clientr list
    }
    C_List->Clear();

    delete C_List;
    delete Ch_ListA;
    delete Ch_ListB;
    D->Form->LogMemo->Lines->Add("All deleted");
}
//----------------------------------------------------- TMaster functions ---
void __fastcall TMaster::ExtSession(TIdPeerThread *AThread){

    //AThread->Priority = tpHighest;

    int strLength;
    int CNT = 10;
    AnsiString temp_s;
    int idxOfC = C_List->IndexOf(AThread->Connection->Binding->PeerIP);

    if(idxOfC>=0){}
    else{
        TCInfo* CInfo = new TCInfo(AThread->Connection->Binding->PeerIP);
        C_List->AddObject(AThread->Connection->Binding->PeerIP,CInfo);
        idxOfC = C_List->IndexOf(AThread->Connection->Binding->PeerIP);
        MThrd->ALS("New client added to c_list:" + AThread->Connection->Binding->PeerIP);
    }

    do{

    temp_s = S_CRB(AThread);
    strLength = CheckInputStr(temp_s);

    if(strLength){
        if(D->df>3)MThrd->ALS("OK string: " + StrToHexStr(temp_s));
    }else{
        if(D->df>0)MThrd->ALS("Wrong string: " + StrToHexStr(temp_s));
        //ExitES(AThread);
        return;
    }

    if(strLength>=10 && temp_s.Pos("\xF\xFA\xF\xFA")){
    //ID ID ID ID LL LL 00 00 LL MM XX XX XX XX 0D
    //                           GG
        if(temp_s.SubString(10,1) == "G"){
            int r = D->PMCommand(temp_s.SubString(11,temp_s.Pos("\xD")-11),MDataIndex,AThread);
            if(r>0){
                if(D->df>0)MThrd->ALS("PMCommand passed. PMCommand(" + temp_s.SubString(11,temp_s.Pos("\xD")-11) + "," + AnsiString(MDataIndex) + ", ...) = " + AnsiString(r));
                return;
            }else{
                MThrd->ALS("Unknown G-command for system.");
            }
        }
        if(temp_s.SubString(10,1) == "M"){/* it's command for this master */}
        return;
    }


    if(strLength>=18 && temp_s.SubString(10,8) == "#0000100"){
        ((TCInfo*)C_List->Objects[idxOfC])->LastChannelNumber = 2*strtoul(temp_s.SubString(18,1).c_str(),NULL,16);
        CurrChan = ((TCInfo*)C_List->Objects[idxOfC])->LastChannelNumber;
        MThrd->ALS("Channel selected in external session: " + AnsiString(((TCInfo*)C_List->Objects[idxOfC])->LastChannelNumber));
    }

    if(strLength>=18 && temp_s.SubString(10,6) == "#00000"){
        unsigned int ch = strtoul(temp_s.SubString(16,1).c_str(),NULL,16);
        unsigned int st = strtoul(temp_s.SubString(18,1).c_str(),NULL,16);
        MThrd->pow_set = true;
        Channels[ch].state = st;
        MThrd->ALS("Set power in external session for channel " + AnsiString(ch) + " ( " + AnsiString(st) + " ) ");
    }

    EI = !EI;

    if(strLength>0&&!MThrd->NC&&MThrd->SuspendForES(10000)){
            try{
                Client->Write(temp_s);
                //ClientWrite(temp_s);
                RW = true;
                //temp_s = Client->CurrentReadBuffer();
                temp_s = C_CRB();
                //temp_s = ClientRead(2000);
            }catch(Exception &e){
                D->error_count++;
                if(RW)MThrd->ALS(AnsiString(AThread->Connection->Binding->Port) + ": ExtSession TCPClient (from master) CRB() error. " + e.Message, clRed);
                else MThrd->ALS(AnsiString(AThread->Connection->Binding->Port) + ": ExtSession TCPClient (to master) Write() error. " + e.Message, clRed);
                ExitES(AThread);
                return;
            }
            RW = false;
            if(D->df>2)MThrd->ALS("Received from master string in ExtSession: " + StrToHexStr(temp_s));
            try{AThread->Connection->Write(temp_s);}
            catch(Exception &e){
                D->error_count++;
                MThrd->ALS(AnsiString(AThread->Connection->Binding->Port) + ": ExtSession TCPServer write error. " + e.Message, clRed);ExitES(AThread);
                return;
            }
    }else{
        if(D->df>3)MThrd->ALS("temp_s<=0||MThrd->NC");
    }

    }while(CNT--);  //end do{}while()

    if(CNT)MThrd->ALS("ExitES. no return");
    else{
        MThrd->ALS("CNT = 0 ! Too many S_CRB() calls!");
    }
    if(D->df>3)MThrd->ALS("ExtSession() temp_buf_s = [" + StrToHexStr(temp_buf_s) + "] temp_s(!) = " + "[" + StrToHexStr(temp_s) + "]");
    ExitES(AThread);
}
//----------------------------------------------------- TMaster functions ---
void __fastcall TMaster::ExitES(TIdPeerThread *AThread){
    if(D->df>3)MThrd->ALS("ExitES called");
    if(!AThread->Connection->Connected()){
        MThrd->ES = false;
        EI = true;
        if(MThrd->Suspended&&mthread)MThrd->Resume();
    }
}
//----------------------------------------------------- TMaster functions ---
void __fastcall TMaster::SetInfoOb(TMainForm *AInfoOb){
    FInfoOb = AInfoOb;
    //FInfoOb->OnShow = FormShow;
    //FInfoOb->lbChannels->OnClick = lbChannelsClick;
}
//----------------------------------------------------- TMaster functions ---
TMainForm* __fastcall TMaster::GetInfoOb(void){
    return FInfoOb;
}
//----------------------------------------------------- TMaster functions ---
void __fastcall TMaster::RefreshInfo(TObject *Sender){
    if(InfoOb&&InfoOb->lbChannels->Items->Count==12){
        for(int i=0;i<12;i++){
            InfoOb->lbChannels->Items->Strings[i] =
            AnsiString(i==CurrChan?">":" ") +
            AnsiString(Channels[i].RS_Address>0 ? AnsiString().sprintf("%3d",Channels[i].RS_Address).c_str():"   ") + " " +
            state_string[Channels[i].state+1] + " " +
            AnsiString(Channels[i].SensorsState);
        }
    }
}
//-------------------------------------------------- TOMChannel functions ---
__fastcall TOMChannel::TOMChannel(void){
    state = -1;
    RS_Address =-1;
    SensorsState = -1;
    RSBR = -1;
    paramIndex = 0;
}
//-------------------------------------------------- TOMChannel functions ---
__fastcall TOMChannel::~TOMChannel(void){
}
//---------------------------------------------------------------------------
__fastcall TMThread::TMThread(TObject *Owner):TThread(false){
    Priority = tpHighest;
    OnTerminate = MThreadTerminate;
    FOwner = Owner;
    SuspendTimer = new TTimer(NULL);
    SuspendTimer->Interval = 999;
    SuspendTimer->Enabled = false;
    SuspendTimer->OnTimer = SuspendTimerTimer;
    ResponseTimer = new TTimer(NULL);
    ResponseTimer->Enabled = false;
    ResponseTimer->OnTimer = ResponseTimerTimer;
    XLTimer = new TTimer(NULL);
    XLTimer->Enabled = false;
    XLTimer->OnTimer = XLTimerTimer;

    D = ((TMaster*)Owner)->D;

    XLTimer->Interval = D->XLInterval;
    Fdt = Now();

    MessageColor = D->Form->LogMemo->SelAttributes->Color;

    FNC = true;
    FES = false;
    XL = true;
    ND = false;
    getstart = true;
    pow_set = false;

    stopForSync = false;

    XLIndex = -1;

    Fmdi = ((TMaster*)Owner)->MDataIndex;
    paramIndex = 0;
    paramNum = D->AllListSize;

    chanIndex = 0;
    chanNum = D->srvrs[Fmdi]->NumOfChannels;
    ParamsToRead = D->ParametersToRead;
    paramNumShort = D->ShortListSize;
    ChansToRead = new int[chanNum];

    dh = new TDataHeader();

    for(int i=0;i<paramNum;i++){
        if(D->df)ALS(AnsiString(ParamsToRead[i]));
    }

    if(D->df)ALS(AnsiString(paramNum));
    if(D->df)ALS(AnsiString(paramNumShort));

    for(int i=0;i<chanNum;i++){
        ChansToRead[i] = D->srvrs[Fmdi]->Channels[i]->Address;
        if(D->df)ALS("Chans to read: " + AnsiString(ChansToRead[i]));
    }

    delays = new unsigned char[100];
    AnsiString s = Data->Ini->ReadString("Options","Delays" + AnsiString(Fmdi),"");
    for(int i=0;i<100;i++){
        delays[i] = s.SubString(1,s.Pos(";")-1).ToIntDef(0);
        s.Delete(1,s.Pos(";"));
    }

}
//---------------------------------------------------------------------------
__fastcall TMThread::~TMThread(void){
    ALS("TMThread destructor called()...");
    AnsiString dlys = "";
    for(int i=0;i<100;i++){dlys = dlys + AnsiString(delays[i]) + ";";}
    Data->Ini->WriteString("Options","Delays" + AnsiString(Fmdi),dlys);
    ALS(dlys);
    delete [] delays;
    delete dh;
    delete XLTimer;
    delete SuspendTimer;
    delete ResponseTimer;
    delete [] ChansToRead;
}
//-------------------------------------------------- Main Thread Function ---
void __fastcall TMThread::Execute(){
    count = 0;
    last = 0;
    XLTimer->Enabled = true;
    while(1){
        switch(SCP()) {
        case 0:
            if(D->df>2)ALS("SCP()=0 (disconnecting...)");
            //XLTimer->Enabled = false;
            try{
                ((TMaster*)FOwner)->Client->Disconnect();
                ALS("Master disconnected.", clBlue);
            }catch(Exception &exception){
                D->error_count++;
                ALS("Disconnect error " + exception.Message, clRed);
            }
            if(Terminated)return; //end function
        case 1:
            if(D->df>2)ALS("SCP()=1 (connecting...)");
            try{
                WaitForResponse(D->WaitForConnectionInterval);
                ((TMaster*)FOwner)->Client->Connect();
                ResponseTimer->Enabled = false;
                NC = false;
                if(((TMaster*)FOwner)->Client->Port == 3004){
                    if(((TMaster*)FOwner)->Permitted()){
                        ((TMaster*)FOwner)->GetPowerState();
                    }else{
                        pow_set = true;
                        ALS("RS on 3004 connection restricted on master " + ((TMaster*)FOwner)->Client->Host, clYellow);
                    }
                }else{
                    //if(((TMaster*)FOwner)->Permitted2()){
                        ((TMaster*)FOwner)->GetPowerState();
                    //}else{
                    //    pow_set = true;
                    //    ALS("RS on 3000 connection restricted on master " + ((TMaster*)FOwner)->Client->Host, clYellow);
                    //}
                }

                unsigned char val;


                if( ((TMaster*)FOwner)->I2CDevReadByte(0x5C, 0x8, val) ) {
                    ALS("I2C Read = " + AnsiString(val), clGreen);
                    if( ((TMaster*)FOwner)->I2CDevWriteByte(0x5C, 0x8, 0) ) {
                        ALS("I2C Write is OK", clGreen);
                        if( ((TMaster*)FOwner)->I2CDevReadByte(0x5C, 0x8, val) ) ALS("I2C Read = " + AnsiString(val), clGreen);
                        else ALS("I2C Read error (3)", clRed);
                    } else {
                        ALS("I2C Write error (2)", clRed);
                    }
                } else {
                    ALS("I2C Read error (1)", clRed);
                }


                D->srvrs[Fmdi]->ClearSensorsList();
                for(unsigned char i_addr_scan=0x8; i_addr_scan<=0x77; i_addr_scan++){
                    unsigned char d[1];
                    if( ((TMaster*)FOwner)->I2CGetBytes(i_addr_scan, 1, d) ) {
                        ALS("I2C device with address " + AnsiString().sprintf("0x%X",i_addr_scan) + " found @ host [" + ((TMaster*)FOwner)->Client->Host + "]. Return data = " + AnsiString().sprintf("0x%X",d[0]), clGreen);
                        D->srvrs[Fmdi]->AddSensor(i_addr_scan,1);



                        /*
                        for(unsigned int i_read=0; i_read<10; i_read++){
                            if( ((TMaster*)FOwner)->I2CGetBytes(i_addr_scan, 1, d) ) {
                                ALS("I2C device with address " + AnsiString().sprintf("%X",i_addr_scan) + " @ host [" + ((TMaster*)FOwner)->Client->Host + "]. Return data #" + AnsiString(i_read+1) + " = " + AnsiString().sprintf("%X",d[0]), clGreen);
                            }else{
                                ALS("I2C device with address " + AnsiString().sprintf("%X",i_addr_scan) + " read error [" + ((TMaster*)FOwner)->Client->Host + "]", clGray);
                                break;
                            }
                        }
                        */
                    }
                }
//--------------------------------------------- Pint Sensors List of Master ----
                ALS("Print sensors list:", clGray);
                for(unsigned char i_sens=0; i_sens<D->srvrs[Fmdi]->Sensors->Count; i_sens++){
                    PS = (TSensor*)D->srvrs[Fmdi]->Sensors->Items[i_sens];
                    ALS("Address and type of sensor item #" + AnsiString(i_sens) + " = " + AnsiString().sprintf("0x%X",PS->I2CAddress) + " ; " + AnsiString(PS->SensorType), clGreen);

                }
//-------------------------------------------- Read Sensors Data from Master ---
                unsigned char UserReg=0;
                int raw_p=0;
                short raw_t=0;
                double RH=0,Temp=0,P=0;

                for(unsigned char i_sens=0; i_sens<D->srvrs[Fmdi]->Sensors->Count; i_sens++){
                    switch( ((TSensor*)D->srvrs[Fmdi]->Sensors->Items[i_sens])->I2CAddress ){
                        case(0x40): //  Address of SHT20 Humidity Sensor
                            if( ((TMaster*)FOwner)->I2CSHT20GetUserReg(UserReg) ) {
                                ALS("SHT20 found. I2C_SHT20_UserReg = " + AnsiString().sprintf("0x%X",UserReg) + " [" + ((TMaster*)FOwner)->Client->Host + "]", clGreen);
                                T = Now();
                                if( ((TMaster*)FOwner)->I2CSHT20GetRH(RH) ) {
                                    ALS("SHT20 RH(%) = " + AnsiString().sprintf("%.1f",RH) + " [" + ((TMaster*)FOwner)->Client->Host + "]", clGreen);
                                    Pparameter = 128;
                                    StoreData_s = AnsiString().sprintf("%.2f",RH);
                                    Synchronize(StoreMasterData_Sync);
                                }
                                ::Sleep(100);
                                T = Now();
                                if( ((TMaster*)FOwner)->I2CSHT20GetTemp(Temp) ) {
                                    ALS("SHT20 T(C) = " + AnsiString().sprintf("%.1f",Temp) + " [" + ((TMaster*)FOwner)->Client->Host + "]", clGreen);
                                    Pparameter = 127;
                                    StoreData_s = AnsiString().sprintf("%.2f",Temp);
                                    Synchronize(StoreMasterData_Sync);
                                }
                            }else{
                                ALS("I2C SHT20 User Register read error. Device not found. ", clGray);
                            }
                        break;
                        case(0x5C):
                            if( ((TMaster*)FOwner)->I2CDevReadByte(0x5C,0xF,UserReg) ) {
                                if(UserReg==0xBB){
                                    ALS("LPS331AP found. I2C_LPS331AP_WhoAmIReg = " + AnsiString().sprintf("0x%X",UserReg) + " [" + ((TMaster*)FOwner)->Client->Host + "]", clGreen);
                                    if( ((TMaster*)FOwner)->I2CDevReadByte(0x5C,0x20,UserReg) ) {
                                        ALS("LPS331AP CTRL1 Register read. I2C_LPS331AP_CTRL1_REG = " + AnsiString().sprintf("0x%X",UserReg), clGreen);
                                    }else{ALS("LPS331AP Read 20h Register error.",clRed);}

                                    if( ((TMaster*)FOwner)->I2CDevReadByte(0x5C,0x8,UserReg) ) {
                                        ALS("LPS331AP REF_P_XL Register read. I2C_LPS331AP_REF_P_REG_XL = " + AnsiString().sprintf("0x%X",UserReg), clGreen);
                                    }else{ALS("LPS331AP Read 08h Register error.",clRed);}
                                    if( ((TMaster*)FOwner)->I2CDevReadByte(0x5C,0x9,UserReg) ) {
                                        ALS("LPS331AP REF_P_L  Register read. I2C_LPS331AP_REF_P_REG_L  = " + AnsiString().sprintf("0x%X",UserReg), clGreen);
                                    }else{ALS("LPS331AP Read 09h Register error.",clRed);}
                                    if( ((TMaster*)FOwner)->I2CDevReadByte(0x5C,0xA,UserReg) ) {
                                        ALS("LPS331AP REF_P_H  Register read. I2C_LPS331AP_REF_P_REG_H  = " + AnsiString().sprintf("0x%X",UserReg), clGreen);
                                    }else{ALS("LPS331AP Read 0Ah Register error.",clRed);}

                                    if( ((TMaster*)FOwner)->I2CDevWriteByte(0x5C,0x20,0x84) ) {
                                        ALS("LPS331AP CTRL1(20h) Register write. Power up device. I2C_LPS331AP_CTRL1_REG = 84h (PD[7] BDU[2] bits is set)", clGreen);
                                    }else{ALS("LPS331AP Write 20h Register error.",clRed);}

                                    //for(int i_count=0;i_count<10;i_count++){

                                    if( ((TMaster*)FOwner)->I2CDevWriteByte(0x5C,0x21,0x1) ) {
                                        ALS("LPS331AP CTRL2(21h) Register write. Start Measure. I2C_LPS331AP_CTRL2_REG = 01h (ONE_SHOT[0] bits is set)", clGreen);
                                    }else{ALS("LPS331AP Write 21h Register error.",clRed);}

                                    ::Sleep(50);
                                    T = Now();
                                    if( ((TMaster*)FOwner)->I2CDevReadByte(0x5C,0x28,UserReg) ) {
                                        ALS("LPS331AP Read 28h Register = " + AnsiString(UserReg),clGray);
                                        raw_p = UserReg;
                                        ::Sleep(50);
                                        if( ((TMaster*)FOwner)->I2CDevReadByte(0x5C,0x29,UserReg) ) {
                                            ALS("LPS331AP Read 29h Register = " + AnsiString(UserReg),clGray);
                                            raw_p = raw_p|(short)UserReg<<8;
                                            ::Sleep(50);
                                            if( ((TMaster*)FOwner)->I2CDevReadByte(0x5C,0x2A,UserReg) ) {
                                                ALS("LPS331AP Read 2Ah Register = " + AnsiString(UserReg),clGray);
                                                raw_p = raw_p|(int)UserReg<<16;
                                                P = (double)raw_p/40960;
                                                ALS("LPS331AP Read P(kPa) = " + AnsiString().sprintf("%.3f",P) + " [" + ((TMaster*)FOwner)->Client->Host + "]",clGreen);
                                                Pparameter = 131;
                                                StoreData_s = AnsiString().sprintf("%.3f",P);
                                                Synchronize(StoreMasterData_Sync);
                                            }else{ALS("LPS331AP Read 2Ah Register error.",clRed);}
                                        }else{ALS("LPS331AP Read 29h Register error.",clRed);}
                                    }else{ALS("LPS331AP Read 28h Register error.",clRed);}

                                    ::Sleep(50);
                                    T = Now();
                                    if( ((TMaster*)FOwner)->I2CDevReadByte(0x5C,0x2B,UserReg) ) {
                                        ALS("LPS331AP Read 2Bh Register = " + AnsiString(UserReg),clGray);
                                        raw_t = UserReg;
                                        ::Sleep(50);
                                        if( ((TMaster*)FOwner)->I2CDevReadByte(0x5C,0x2C,UserReg) ) {
                                            ALS("LPS331AP Read 2Ch Register = " + AnsiString(UserReg),clGray);
                                            raw_t = raw_t|(short)UserReg<<8;
                                            Temp = 42.5 + (double)raw_t/480;

                                            ALS("LPS331AP Read T(C) = " + AnsiString().sprintf("%.2f",Temp) + " [" + ((TMaster*)FOwner)->Client->Host + "]",clGreen);
                                            Pparameter = 134;
                                            StoreData_s = AnsiString().sprintf("%.2f",Temp);
                                            Synchronize(StoreMasterData_Sync);

                                        }else{ALS("LPS331AP Read 29h Register error.",clRed);}
                                    }else{ALS("LPS331AP Read 28h Register error.",clRed);}

                                    //} //end for i_count

                                    if( ((TMaster*)FOwner)->I2CDevWriteByte(0x5C,0x20,0x0) ) {
                                        ALS("LPS331AP CTRL1 Register write. Power down device. I2C_LPS331AP_CTRL1_REG = 00h (PD[7] BDU[2] bits is unset)", clGreen);
                                    }else{ALS("LPS331AP Write 20h Register error.",clRed);}

                                }else{ALS("LPS331AP WhoAmI Register is not BBh",clRed);}
                            }else{ALS("LPS331AP Read WhoAmI Register error.",clRed);}
                        break;
                        default:
                        break;
                    }
                }
//----------------------------------------- block for check internal heater ----
                /*
                if( ((TMaster*)FOwner)->I2CSHT20SetUserReg(58) ) {
                    ALS("I2C SHT20 SetUserReg = true", clGreen);
                }
                if( ((TMaster*)FOwner)->I2CSHT20GetUserReg(UserReg) ) {
                    ALS("I2C SHT20 GetUserReg = " + AnsiString().sprintf("%d",UserReg), clGreen);
                }
                */

                ALS("Connected to master " + ((TMaster*)FOwner)->Client->Host, clGreen);
                ((TMaster*)FOwner)->temp_buf_s = "";
                ((TMaster*)FOwner)->temp_buf_c = "";
            }catch(Exception &exception){
                ResponseTimer->Enabled = false;
                if(exception.Message == "Already connected."){
                    ALS("Disonnect now...!");
                    ((TMaster*)FOwner)->Client->Disconnect();
                    //AddLogString("Disonnect Socket now...!");
                    //((TMaster*)FOwner)->Client->DisconnectSocket();
                }
                if(!NC){
                    NC = true;
                    //D->error_count++;
                    ALS("Connect error (not timeout): " + exception.Message, clRed);
                }else{
                    ALS("Exception by timeout error: " + exception.Message, clRed);
                }
                ::Sleep(1000);
            }
            break;
        case 2:
            if(getstart){
                getstart=false;
                T3 = Now();
            }
          //if(D->df>2)ALS("SCP()=2 (getting parameter " + AnsiString(chanIndex) + "(from " + AnsiString(chanNum) + "), " + AnsiString(ChansToRead[chanIndex]) + ", " + AnsiString(((TMaster*)FOwner)->Channels[D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel].ParamsToRead[paramIndex]) + ") paramNumShort = " + ((TMaster*)FOwner)->Channels[D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel].paramNumShort);
            if(D->df>2)ALS("SCP()=2 (getting parameter " + AnsiString(chanIndex) + "(from " + AnsiString(chanNum) + "), " + AnsiString(ChansToRead[chanIndex]) + ", " + AnsiString(D->srvrs[Fmdi]->Channels[chanIndex]->PArray[paramIndex]) + ") paramNumShort = " + AnsiString(D->srvrs[Fmdi]->Channels[chanIndex]->PAShortSize));

            if(!NC){
              //if(((TMaster*)FOwner)->Channels[D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel].paramNumShort==0 && XLIndex==-1 && ((TMaster*)FOwner)->mthread){
                if(D->srvrs[Fmdi]->Channels[chanIndex]->PAShortSize==0 && XLIndex==-1 && ((TMaster*)FOwner)->mthread){

                    if(D->df>2)ALS("paramNumShort==0, XLIndex==-1, mthread==true.  Skipping GetParameterCycle()", clPurple);
                    ((TMaster*)FOwner)->ESRA = 0;
                }else{
                    GetParameterCycle();
                    //count++;
                }
                Next2();
                //::Sleep(50);
                //::Sleep(10);
                if(ND){   // No Delay Need for Acc and Mag reading with separate commands
                    SuspendTimer->Interval = 50;
                    ND = false;
                }
                ((TMaster*)FOwner)->MI = !((TMaster*)FOwner)->MI;
            }else{
                D->error_count++;
                ALS("GetCCParameter error. Disconnect...", clRed);
                ((TMaster*)FOwner)->Client->Disconnect();
                SuspendTimer->Interval = D->WaitIfReadErrorInterval;
            }
        case 3:
            SuspendTimer->Enabled = false;
            if(!Terminated && SuspendTimer->Interval){
                if(D->df>3)ALS("SCP()=3 Suspending...");

                if(!stopForSync){
                    if(D->df>2)ALS("Suspend for " + AnsiString(SuspendTimer->Interval) + " ms...");
                    SuspendTimer->Enabled = true;
                    if(SuspendTimer->Enabled)Suspend();
                }else{
                    if(D->df>2)ALS("Suspend for sync timer");
                    if(D->SyncTimer->Enabled)Suspend();
                    stopForSync = false;
                }

                if(SuspendTimer->Enabled)SuspendTimer->Enabled = false;
                if(D->df>3)ALS("End suspension");
            }else{
                if(D->df>3)ALS("No suspend. Terminated signal.");
            }
        } //end switch
    } //end while
    //if(D->df)AddLogString("End thread...");
}
//---------------------------------------------------------------------------
void __fastcall TMThread::GetParameterCycle(void){
    ((TMaster*)FOwner)->SelectChannel(D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel);
    if(pow_set){
        ((TMaster*)FOwner)->GetPowerState();
        pow_set = false;
    }

    //AddLogString("Current channel index is " + AnsiString(chanIndex) + " RS_address = " + AnsiString(((TMaster*)FOwner)->CurrentChannelRef->RS_Address) + " Relay number = " + AnsiString(((TMaster*)FOwner)->CurrentChannelRef->RelayChannel),clPurple);
    //AddLogString("paramIndex is " + AnsiString(paramIndex) + " paramNum = " + AnsiString(((TMaster*)FOwner)->CurrentChannelRef->paramNum),clPurple);

    if(((TMaster*)FOwner)->Channels[D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel].state!=0){
        T = Now();
        if(D->doSetBR)((TMaster*)FOwner)->SetBaudrate(D->srvrs[Fmdi]->Channels[chanIndex]->RSBR);

        //StoreData_s = ((TMaster*)FOwner)->GetCCParam(ChansToRead[chanIndex],
        //    ((TMaster*)FOwner)->CurrentChannelRef->ParamsToRead[paramIndex]);
        StoreData_s = ((TMaster*)FOwner)->GetCCParam(ChansToRead[chanIndex],
            D->srvrs[Fmdi]->Channels[chanIndex]->PArray[paramIndex]);
        if(D->df>3)ALS("CHECKPOINT_2");
        

            if(((TMaster*)FOwner)->Client->Port == 3004){
                ((TMaster*)FOwner)->FreeRS();
            }//else{
                //((TMaster*)FOwner)->FreeRS2();
            //}
        if(D->df>3)ALS("CHECKPOINT_3");
    //----Postprocessing
        //if( ((TMaster*)FOwner)->Channels[D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel].ParamsToRead[paramIndex]==126 )
        if( D->srvrs[Fmdi]->Channels[chanIndex]->PArray[paramIndex]==126 )
            ((TMaster*)FOwner)->Channels[D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel].SensorsState = StoreData_s.ToIntDef(-1);
        //if( ((TMaster*)FOwner)->Channels[D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel].ParamsToRead[paramIndex]==122 && D->versions->IndexOf(StoreData_s)<0 && StoreData_s.Length() )
        if( D->srvrs[Fmdi]->Channels[chanIndex]->PArray[paramIndex]==122 && D->versions->IndexOf(StoreData_s)<0 && StoreData_s.Length() )
            D->versions->Add(StoreData_s);
        //if( ((TMaster*)FOwner)->Channels[D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel].ParamsToRead[paramIndex]==129 && ((TMaster*)FOwner)->Channels[D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel].ParamsToRead[paramIndex+1]==130 )
        if( D->srvrs[Fmdi]->Channels[chanIndex]->PArray[paramIndex]==129 && D->srvrs[Fmdi]->Channels[chanIndex]->PArray[paramIndex+1]==130 )
            ND = true;
        //if( ((TMaster*)FOwner)->CheckAdequate(((TMaster*)FOwner)->CurrentChannelRef->ParamsToRead[paramIndex],StoreData_s) == "" ){
        //if( ((TMaster*)FOwner)->CheckAdequate(D->srvrs[Fmdi]->Channels[chanIndex]->PArray[paramIndex],StoreData_s) == "" ){
        if( ((TMaster*)FOwner)->CheckAdequate(D->srvrs[Fmdi]->Channels[chanIndex]->PArray[paramIndex],StoreData_s) == "" ){
            //ALS("Inadequate answer [" + StrToHexStr(StoreData_s) + "] for parameter " + ((TMaster*)FOwner)->CurrentChannelRef->ParamsToRead[paramIndex] + " on channel " + ChansToRead[chanIndex],clYellow);
            ALS("Inadequate answer [" + StrToHexStr(StoreData_s) + "] for parameter " + D->srvrs[Fmdi]->Channels[chanIndex]->PArray[paramIndex] + " on channel " + ChansToRead[chanIndex],clYellow);
            D->inadequate++;
        }
        if(D->df>3)ALS("CHECKPOINT_4");
    //----end of Postprocessing block
        count++;
        
        /*//WaitSDB();
        ALS("Calling Store_Data" +
        AnsiString(D->srvrs[Fmdi]->Address) + " " +
        AnsiString(D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel) + " " +
        AnsiString(D->srvrs[Fmdi]->Channels[chanIndex]->Pair) + " " +
        AnsiString(D->srvrs[Fmdi]->Channels[chanIndex]->Address) + " " +
        AnsiString(D->srvrs[Fmdi]->Channels[chanIndex]->PArray[paramIndex]) + " " +
        AnsiString(StoreData_s.Length()),
        clPurple);
        */

        Synchronize(StoreData_Sync);
        //StoreData_Sync();
        if(D->df>3)ALS("CHECKPOINT_5");
        //D->StoreData(T,D->srvrs[Fmdi]->Address,D->srvrs[Fmdi]->Channels[chanIndex],((TMaster*)FOwner)->Channels[D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel].ParamsToRead[paramIndex],s.Length(),s);
        //D->busy = false;
    }else{
        //((TMaster*)FOwner)->Channels[D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel].state = -1;
        ((TMaster*)FOwner)->Channels[D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel].SensorsState = -1;
        if(D->doSetPower){
            ((TMaster*)FOwner)->SetPower(1);
            pow_set = true;
            ALS("Channel " + AnsiString(D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel) + " is OFF. Set it ON.", clBlue);
        }
        else ALS("Channel " + AnsiString(D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel) + " is OFF. Read is disabled.", clPurple);
    }
    if(D->df>3)ALS("CHECKPOINT_6");
}
//---------------------------------------------------------------------------
void __fastcall TMThread::WaitSDB(void){
    while(D->busy){
        D->SDBCounter++;
        ::Sleep(5);
    }
    D->busy = true;
}
//---------------------------------------------------------------------------
void __fastcall TMThread::StoreData_Sync(void){
    //D->busy = true;
    if(D->df>3)ALS("CHECKPOINT_7");
    D->StoreData(T,D->srvrs[Fmdi]->Address,D->srvrs[Fmdi]->Channels[chanIndex],D->srvrs[Fmdi]->Channels[chanIndex]->PArray[paramIndex],StoreData_s.Length(),StoreData_s);
    if(D->df>3)ALS("CHECKPOINT_8");
    //D->busy = false;
}
//---------------------------------------------------------------------------
void __fastcall TMThread::StoreMasterData_Sync(void){
    //D->busy = true;
    if(D->df>3)ALS("CHECKPOINT_7.1");
    D->StoreMasterData(T,D->srvrs[Fmdi]->Address,Pparameter,StoreData_s.Length(),StoreData_s);
    if(D->df>3)ALS("CHECKPOINT_8.1");
    //D->busy = false;
}
//---------------------------------------------------------------------------
void __fastcall TMThread::WaitForResponse(int AWaitTime){
    ResponseTimer->Enabled = false;
    ResponseTimer->Interval = AWaitTime;
    //ResponseTimer->OnTimer = ResponseTimerTimer;
    ResponseTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMThread::ResponseTimerTimer(TObject *Sender){
    ResponseTimer->Enabled = false;
    //D->error_count++;
    //if(D->df>0)ALS("Wait for response timeout (" + AnsiString(ResponseTimer->Interval) + " ms). Set NC = true. " +
    //"GetParamCC(" + AnsiString(ChansToRead[chanIndex]) + "," + AnsiString(((TMaster*)FOwner)->CurrentChannelRef->ParamsToRead[paramIndex]) + ") for master " +
    //((TMaster*)FOwner)->Client->Host, clRed);
    NC = true;
    ((TMaster*)FOwner)->Client->Disconnect();
}
//---------------------------------------------------------------------------
int __fastcall TMThread::SuspendForES(int AWaitTime){
    if(D->df>3)ALS("Suspend for ES " + AnsiString(AWaitTime) + " ms...");
    SuspendTimer->Interval = AWaitTime;
    if(Suspended&&NC){
        //SuspendTimer->Enabled = false;
        Resume();
        return 1;
    }
    if(ES){
        SuspendTimer->Enabled = false;
        SuspendTimer->Enabled = true;
        return 1;
    }
    
    ES = true;

    int k = 20;
    while( (!Suspended) && (!Terminated) && k ){
        ALS("Wait " + AnsiString(100-5*k--) + " ms for suspension...");
        ::Sleep(100-5*k);
    }
    
    if( k && !Terminated )return 1;
    ALS("Suspension for ES error. Can't suspend.");
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMThread::SuspendTimerTimer(TObject *Sender){
    SuspendTimer->Enabled = false;
    if(((TMaster*)FOwner)->RW){
        ((TMaster*)FOwner)->RW = false;
        NC=true;
        ALS("(ESRW) Wait for read timeout!!!");
        ((TMaster*)FOwner)->Client->Disconnect();  //drop client if read waiting more than Suspend for ES time
    }
    if(ES)ES = false;
    ((TMaster*)FOwner)->EI = true;
    if(D->df>2)ALS("ES = false. Resume by timer after " + AnsiString(SuspendTimer->Interval) + " ms waiting.");
    Resume();
}
//---------------------------------------------------------------------------
int __fastcall TMThread::SCP(void){
    MSG msg;
    if(Terminated){
        ALS("Stop request received...");
        return 0;
    }
	/*while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
        if(msg.message==WM_QUIT){
            ALS("Stopped by Windows.");
            Terminate();
            return 0;
        }else{
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
	} */
    if(NC&&((TMaster*)FOwner)->Client->Connected())return 0; //if connected, but NC=true
    if(NC&&!((TMaster*)FOwner)->Client->Connected())return 1; //if no connection at start
    if(((TMaster*)FOwner)->Client->Port == 3004){
        if(!((TMaster*)FOwner)->Permitted()){
            SuspendTimer->Interval = D->PermissionRequestInterval;
            return 3;
        }//else{
            //SuspendTimer->Interval = D->ParameterInterval;
            //return 2;
        //}
    }//else{
        //if(!((TMaster*)FOwner)->Permitted2()){
            //SuspendTimer->Interval = D->PermissionRequestInterval;
            //return 3;
        //}else{
        //    SuspendTimer->Interval = D->ParameterInterval;
        //    //return 2;
        //}
    //}
    if(ES)return 3; //if external session flag
    if(!((TMaster*)FOwner)->mthread){
        SuspendTimer->Interval = D->SleepModeInterval;
        return 2;
    }
    else SuspendTimer->Interval = D->ParameterInterval;
    return 2; //if all ok for monitoring cycle
}
//---------------------------------------------------------------------------
void __fastcall TMThread::ESB(void){
    if(!((TMaster*)FOwner)->FInfoOb->ScanButton->Enabled)((TMaster*)FOwner)->FInfoOb->ScanButton->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMaster::leCurrChanChange(TObject *Sender){
    /*int ch = FInfoOb->leCurrChan->Text.ToIntDef(-1);
    if(ch>=0&&ch<=11){
        FInfoOb->leCurrChan->Color = clWindow;
        CurrChan = ch;
        if(Channels[CurrChan].state==1)FInfoOb->CheckBox1->State = cbChecked;
        else if(Channels[CurrChan].state==-1)FInfoOb->CheckBox1->State = cbGrayed;
        else FInfoOb->CheckBox1->State = cbUnchecked;
        FInfoOb->CheckBox1->Enabled = true;
        RefreshInfo(Sender);
    }else{
        FInfoOb->leCurrChan->Color = clRed;
        FInfoOb->CheckBox1->Enabled = false;
    }
        FInfoOb->Caption = Sender->MethodName(Sender);
    */
}
//---------------------------------------------------------------------------
void __fastcall TMaster::Button1Click(TObject *Sender){
    RefreshInfo(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMaster::FormClose(TObject *Sender, TCloseAction &Action){
    //FInfoOb = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMaster::FormShow(TObject *Sender){
    //RefreshInfo(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMaster::CheckBox1Click(TObject *Sender){
    if(FInfoOb->mthreadCheckBox->State == cbChecked)Channels[CurrChan].state = 1;
    else if(FInfoOb->mthreadCheckBox->State==cbGrayed)Channels[CurrChan].state = -1;
         else Channels[CurrChan].state=0;
    RefreshInfo(Sender);
    //not Current Channel!!! Need to fix it!!!
}
//---------------------------------------------------------------------------
void __fastcall TMaster::lbChannelsClick(TObject *Sender){
    //FInfoOb->leCurrChan->Text = AnsiString(FInfoOb->lbChannels->ItemIndex);
}
//---------------------------------------------------------------------------
void __fastcall TMaster::Button2Click(TObject *Sender){
    //if(FInfoOb->lbChannels->ItemIndex>=0&&FInfoOb->lbChannels->ItemIndex<=11)
    //    FInfoOb->leCurrChan->Text = AnsiString(FInfoOb->lbChannels->ItemIndex);
    //RefreshInfo(Sender);
}
//---------------------------------------------------------------------------
TIdTCPServer* __fastcall TMaster::GetServer(void){
    return FServer;
}
//---------------------------------------------------------------------------
void __fastcall TMaster::SetServer(TIdTCPServer *AServer){
    FServer = AServer;
    //FServer->Active = false;
    FServer->Bindings->Add();
    FServer->Bindings->Items[MDataIndex]->Port = FPort;
    //FServer->Bindings->Items[FServer->Bindings->Count-1]->Port = FPort;
    //MThrd->AddLogString("SetServer() Add Port " + AnsiString(FServer->Bindings->Items[MDataIndex]->Port));
    //FServer->Active = true;
    //MThrd->AddLogString("SetServer() Add Port " + AnsiString(FServer->Bindings->Items[MDataIndex]->Port));
    for(int i=0;i<FServer->Bindings->Count;i++){
        MThrd->ALS("FServer Port = " + AnsiString(FServer->Bindings->Items[i]->Port));
    }
}
//---------------------------------------------------------------------------
int __fastcall TMaster::GetPort(void){
    return FPort;
}
//---------------------------------------------------------------------------
void __fastcall TMaster::SetPort(int APort){
    MThrd->ALS("SetPort(" + AnsiString(APort) + ") called...");
    if(FServer){
        FServer->Bindings->Items[MDataIndex]->Port = APort;
        //if(!FServer->Active)FServer->Active = true;

    /*
    for(int i=0;i<FServer->Bindings->Count;i++){
        if(FPort==FServer->Bindings->Items[i]->Port){
            FServer->Bindings->Items[i]->Port = APort;
            if(!FServer->Active)FServer->Active = true;
            FPort = APort;
            return;
        }
    }
    */
    }
    FPort = APort;
}
//---------------------------------------------------------------------------
int __fastcall TMaster::SelectChannel(int channel){
    if(MThrd->NC)return 0;
    if(D->df>3)MThrd->ALS("SelectChannel(" + AnsiString(channel) + ") called..." + " CurrChan = " + AnsiString(CurrChan));
    if(channel==CurrChan)return 1;
    if(channel/2!=CurrChan/2 || CurrChan<0){
        AnsiString str;
        char chr[] = "\xB\xBC\xB\xBC\x0\x13\x0\x0\xA#00000000\xD";
                //    1  2   3  4   5  6   7  8  9  0123456789
        chr[14] = '1';
        chr[17] = 0x30 + channel/2;
        str = AnsiString(chr,sizeof(chr)-1);
        if(D->df>3)MThrd->ALS("SelectChannel TX: " + str.SubString(10,9));
        try{
            ClientWrite(str);
            str = ClientRead(D->WaitForResponseInterval);
            //::Sleep(100);
        }catch(Exception &e){
            MThrd->ALS("SelectChannel() error: " + e.Message);
            return 0;
        }
    }
    CurrentChannelRef = &Channels[channel];
    CurrChan = channel;
    return 1;
}
//---------------------------------------------------------------------------
int __fastcall TMaster::SetBaudrate(int brate){
    if(MThrd->NC)return 0;
    if(CurrentChannelRef->RSBR==brate)return 1;

    AnsiString str;
    char chr[] = "\xB\xBC\xB\xBC\x0\x13\x0\x0\xA#00000000\xD";
    chr[14] = '1';
    chr[15] = '1';
    chr[17] = 0x30 + brate;
    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("SetBaudrate TX: " + str.SubString(10,9));

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("SetBaudrate() error: " + e.Message);
        return 0;
    }

    CurrentChannelRef->RSBR = brate;
    return 1;
}
//---------------------------------------------------------------------------
int __fastcall TMaster::SetPower(int state){
    if(MThrd->NC)return 0;
    if(state==Channels[CurrChan].state)return 1;
    AnsiString str;
    char chr[] = "\xB\xBC\xB\xBC\x0\x13\x0\x0\xA#00000000\xD";
    if(CurrChan>=0&&CurrChan<=9)chr[15] = 0x30 + CurrChan;
    if(CurrChan>=10&&CurrChan<=15)chr[15] = 0x37 + CurrChan;
    chr[17] = 0x30 + state;
    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("SetPower TX: " + str.SubString(10,9));

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("SetPower() error: " + e.Message);
        return 0;
    }
    
    Channels[CurrChan].state = state;
    return 1;
}
//---------------------------------------------------------------------------
int __fastcall TMaster::SendToRS(AnsiString sendStr){
    AnsiString str;
    char chr[] = "\xB\xBC\xB\xBC\x0\x0\x0\x0\x0";
    chr[4] = (unsigned char)(sendStr.Length()+10)/256;
    chr[5] = (unsigned char)(sendStr.Length()+10);
    chr[7] = (unsigned char)(sendStr.Length()+1)/256;
    chr[8] = (unsigned char)(sendStr.Length()+1);
    str = AnsiString(chr,sizeof(chr)-1);
    str = str + sendStr + "\xD";
    if(D->df>3)MThrd->ALS("SendToRS TX: " + str.SubString(10,sendStr.Length()) + "(" + AnsiString(str.Length()) + "bytes)");

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("SendToRS() error: " + e.Message);
        return 0;
    }

    return 1;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMaster::ReadFromRS(void){
    AnsiString str;
    char chr[] = "\xB\xBD\xB\xBD\x0\x9\x0\x0\x80";
    str = AnsiString(chr,sizeof(chr)-1);

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("ReadFromRS() error: " + e.Message);
        return "";
    }

    if(D->df>3)MThrd->ALS("ReadFromRS() ClientRead() = " + StrToHexStr(str));
    if(D->df>3)MThrd->ALS("ReadFromRS() ClientRead() Length = " + AnsiString(str.Length()));

    if(str.Length()<6){
        if(D->df>2)MThrd->ALS("ReadFromRS() Packet length < 6 . Exit function." + AnsiString(str.Length()),clRed);
        D->error_count++;
        return "";
    }

    unsigned short L = ((unsigned short)str[6]) + ((unsigned short)str[5])*256;

    if(str.Length() != L){
        if(D->df>2)MThrd->ALS("ReadFromRS str.Length != L " + AnsiString(str.Length()) + "!=" + AnsiString(L) ,clRed);
        D->error_count++;
        return "";
    }

    unsigned short l = ((unsigned short)str[9]) + ((unsigned short)str[8])*256;

    if(D->df>2)MThrd->ALS("L = " + AnsiString(L) + ", l = " + AnsiString(l));

    str = str.SubString(L-l+1,l);

    int pos = str.Pos('\x00');
    while(pos && pos!=str.Length()){
        str.Delete(pos,1);
        MThrd->ALS("ReadFromRS Found and removed one 00 character from received string. str.Length = " + AnsiString(str.Length()));
        pos = str.Pos('\x00');
    }

    if(l){
        if(D->df>3)MThrd->ALS("ReadFromRS RX: " + StrToHexStr(str));
        return str;
    }else{
        if(D->df>3)MThrd->ALS("ReadFromRS RX: [empty]", clYellow);
        return "";
    }
}
//---------------------------------------------------------------------------
int __fastcall TMaster::SetOneCCParam(int ccAddr, int param, double value){
    if(D->df>2)MThrd->ALS("SetCCParam(" + AnsiString(ccAddr) + "," + AnsiString(param) + "," + AnsiString().sprintf("%f",value) + ") called...");
    SendToRS("*" + AnsiString(ccAddr) + "," + AnsiString(param) + "," + AnsiString().sprintf("%f",value));
    return 0;
}
//---------------------------------------------------------------------------

AnsiString __fastcall TMaster::GetCCParam(int ccAddr, int param){
    if(D->df>2)MThrd->ALS("GetCCParam(" + AnsiString(ccAddr) + "," + AnsiString(param) + ") called...");
    SendToRS("*" + AnsiString(ccAddr) + "," + AnsiString(param));
    AnsiString s="";
    int pos=0;
    //int beg_delay = 50;

    //if(param==127)beg_delay = 80;
    //if(param==199)beg_delay = 40;
        
    for(ESRA=0;ESRA<D->MaxESRA&&!MThrd->NC;ESRA++){
        //::Sleep(beg_delay+ESRA*20);
        if(ESRA&&D->delay_tune)MThrd->delays[param-100] = MThrd->delays[param-100]+ESRA*2;
        ::Sleep(MThrd->delays[param-100]);
        
        s = s + ReadFromRS();
        if(s.Length()){
            //if(s[s.Length()]=='\x1A') break;
            pos = s.Pos("\x1A");
            if(pos){
                if(D->df>1)MThrd->ALS("GetCCParam(" + AnsiString(ccAddr) + "," + AnsiString(param) + ") = [" + StrToHexStr(s.SubString(1,pos-1)) + "] " + AnsiString(pos-1) + " bytes ( " + AnsiString(ESRA+1) + " attempts )");
                if(D->df==1&&MThrd->XL)MThrd->ALS("(" + AnsiString(ccAddr) + "," + AnsiString(param) + ") " + CheckAdequate(param,s.SubString(1,pos-1)), clGreen);
                return s.SubString(1,pos-1);
            }else{
                if(D->df>3)MThrd->ALS("Partial reading: " + StrToHexStr(s));
                D->partial_read_count++;
            }
            if(D->df>3)MThrd->ALS("CHECKPOINT_1");
        }
    }

    D->NSR++;

    if(ESRA>=D->MaxESRA){
        MThrd->ALS("Unsuccess reading with " + AnsiString(ESRA+1) + " attempts. GetCCParam(" + AnsiString(ccAddr) + "," + AnsiString(param) + ") = [] " + AnsiString(s.Length()) + " bytes", clPurple);
    }else{
        MThrd->ALS("GetCCParam() Connection lost. Interrupt reading.", clRed);
    }
    return "";
}
//---------------------------------------------------------------------------
int __fastcall TMaster::GetPowerState(void){
    if(MThrd->NC)return 0;
    SendToRS("#000200");
    AnsiString s,bin_state;
    int pos=0;
    unsigned int state,n;
    for(ESRA=0;ESRA<D->MaxESRA;ESRA++){
        s = s + ReadFromRS();
        if(s.Length()){
            //if(s[s.Length()]=='\x0D') break;
            pos = s.Pos("\x0D");
            if(pos>=6){
                state = strtoul((s.SubString(4,2) + s.SubString(2,2)).c_str(), NULL, 16);
                for(int i=0;i<12;i++){
                    if(D->df>0&&Channels[i].state>=0&&(unsigned int)Channels[i].state!=((state>>i)&1))
                        MThrd->ALS("Power state changing missed. Or UDP packet lost. Power on channel " + AnsiString(i) +" was " + AnsiString(Channels[i].state) + ", now " + AnsiString((state>>i)&1));
                    Channels[i].state = (state>>i)&1;
                    bin_state = bin_state + AnsiString(Channels[i].state) + " ";
                }
                MThrd->ALS("PowerState = " + AnsiString(bin_state) + "( " + AnsiString(ESRA) + " attempts )");
                ESRA = 0;
                return 1;
            }
            else{
                if(D->df>2)MThrd->ALS("Partial reading in GetPowerState: " + StrToHexStr(s));
                D->partial_read_count++;
            }
        }
        ::Sleep(10+ESRA*10);
    }

    MThrd->ALS("Unsuccess reading of Power State with " + AnsiString(ESRA) + " attempts.", clPurple);
    ESRA = 0;
    D->NSR++;
    return 0;
}
//---------------------------------------------------------------------------
bool __fastcall TMaster::Permitted(void){
    AnsiString str;
    char chr[] = "\xB\xBB\xB\xBB\x0\xC\x0\x0\x1\x2\x0\x0";
    //                            L  _  _  _  _
    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("Request Permission TX: (" + AnsiString(str.Length()) + "bytes)");

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("Request Permission error: " + e.Message);
        return false;
    }
    if(str.c_str()[10]) return true;
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TMaster::Permitted2(void){
    AnsiString str;
    char chr[] = "\xB\xBB\xB\xBB\x0\xC\x0\x0\x0\x2\x0\x0";
    //                             L  _  _  _  _
    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("Request Permission TX: (" + AnsiString(str.Length()) + "bytes)");

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("Request Permission error: " + e.Message);
        return false;
    }
    if(str.c_str()[10]) return true;
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TMaster::FreeRS(void){
    AnsiString str;
    char chr[] = "\xB\xBA\xB\xBA\x0\xE\x0\x0\x1\x2\x0\x0\x0\x1";
    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("FreeRS TX: (" + AnsiString(str.Length()) + "bytes)");

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("FreeRS error: " + e.Message);
        return false;
    }
    if(str.c_str()[9]) return false;
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMaster::FreeRS2(void){
    AnsiString str;
    char chr[] = "\xB\xBA\xB\xBA\x0\xE\x0\x0\x0\x2\x0\x0\x0\x1";
    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("FreeRS TX: (" + AnsiString(str.Length()) + "bytes)");

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("FreeRS error: " + e.Message);
        return false;
    }
    if(str.c_str()[9]) return false;
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMaster::I2CGetBytes(unsigned char dev, unsigned char byte_count, unsigned char *data){
    AnsiString str;
    char chr[] = "\xB\xC3\xB\xC3\x0\xA\x0\x0\x0\x0";

    if(!byte_count)return false;

    chr[8] = dev; //device to read
    chr[9] = byte_count; //num of bytes to read

    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("I2C GetBytes TX: (" + AnsiString(str.Length()) + " bytes)");

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("I2C GetBytes error: " + e.Message);
        return false;
    }
    if(D->df>3)MThrd->ALS("I2C GetBytes return string: " + StrToHexStr(str));
    if(str.SubString(1,4)!="\xB\xC3\xB\xC3" ||  str.c_str()[8]) {
        if(D->df>3)MThrd->ALS("I2C GetBytes error in return string.");
        return false;
    }

    for(unsigned char i_chr=0; i_chr<byte_count; i_chr++){
        data[i_chr] = str.c_str()[10+i_chr];
    }

    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMaster::I2CSHT20GetRH(double &RH){
    AnsiString str;
    char chr[] = "\xB\xC2\xB\xC2\x0\xA\x0\x0\x0\x0";

    chr[8] = 0x40;
    chr[9] = 0xF5;  //get hum, no hold master

    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("I2C Put TX: (" + AnsiString(str.Length()) + " bytes)");

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("I2C Put Byte error: " + e.Message);
        return false;
    }
    if(D->df>3)MThrd->ALS("I2C Put return string: " + StrToHexStr(str));
    if(str.c_str()[8]) {
        MThrd->ALS("I2CGetHum error write to device.");
        return false;
    }

    ::Sleep(100);

    chr[1] = 0xc3;
    chr[3] = 0xc3;
    chr[9] = 0x3;
    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("I2C Get TX: (" + AnsiString(str.Length()) + " bytes)");
    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("I2CRead error: " + e.Message);
        return false;
    }
    if(str.c_str()[8]) {
        MThrd->ALS("I2CGetHum error read from device.");
        return false;
    }
    if(str.c_str()[5]!=0xD){
        MThrd->ALS("I2CGetHum error. Not 3 bytes received.");
        return false;
    }

    str.c_str()[11] = str.c_str()[11] & 0xFC; //unset status bits (two last bits)

    //MThrd->ALS("GetRH() (1) RAW_RH = " + AnsiString((256*(double)(unsigned char)str.c_str()[10])+(unsigned char)str.c_str()[11]));
    //RH = (256*(double)(unsigned char)str.c_str()[10]+(double)(unsigned char)str.c_str()[11])/(double)65536;
    //RH = -6 + 125*RH;
    //MThrd->ALS("GetRH() (1) RH(%) = " + AnsiString().sprintf("%.3f",RH));

    //MThrd->ALS("GetRH() (2.1) RAW_RH = " + AnsiString((unsigned char)str.c_str()[11]));
    //MThrd->ALS("GetRH() (2.2) RAW_RH = " + AnsiString((unsigned short)(str.c_str()[10]<<8)));
    //MThrd->ALS("GetRH() (2.0) RAW_RH = " + AnsiString((unsigned short)str.c_str()[10]<<8|(unsigned char)str.c_str()[11]));
    //RH = ((double)((unsigned char)str.c_str()[11]|(unsigned short)str.c_str()[10]<<8))/65536;
    RH = -6+125*((double)((unsigned short)(str.c_str()[10]<<8)|(unsigned char)str.c_str()[11]))/65536;
    //MThrd->ALS("GetRH() (2) RH(%) = " + AnsiString().sprintf("%.3f",RH));

    //RH = -6 + 125*RH;
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMaster::I2CSHT20GetTemp(double &Temp){
    AnsiString str;
    char chr[] = "\xB\xC2\xB\xC2\x0\xA\x0\x0\x0\x0";

    chr[8] = 0x40;
    chr[9] = 0xF3; //get temp, no hold master

    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("I2C Put TX: (" + AnsiString(str.Length()) + " bytes)");

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("I2C Put Byte error: " + e.Message);
        return false;
    }
    if(D->df>3)MThrd->ALS("I2C Put return string: " + StrToHexStr(str));
    if(str.c_str()[8]) {
        MThrd->ALS("I2CGetTemp error write to device.");
        return false;
    }

    ::Sleep(100);

    chr[1] = 0xc3;
    chr[3] = 0xc3;
    chr[9] = 0x3;
    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("I2C Get TX: (" + AnsiString(str.Length()) + " bytes)");
    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("I2CRead error: " + e.Message);
        return false;
    }
    if(str.c_str()[8]) {
        MThrd->ALS("I2CGetTemp error read from device.");
        return false;
    }
    if(str.c_str()[5]!=0xD){
        MThrd->ALS("I2CGetTemp error. Not 3 bytes received.");
        return false;
    }

    str.c_str()[11] = str.c_str()[11] & 0xFC; //unset status bits (two LSB bits)

    Temp = -46.85 + 175.72*((double)((unsigned short)(str.c_str()[10]<<8)|(unsigned char)str.c_str()[11]))/65536;;
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMaster::I2CSHT20GetUserReg(unsigned char &UserReg){
    AnsiString str;
    char chr[] = "\xB\xC2\xB\xC2\x0\xA\x0\x0\x0\x0";

    chr[8] = 0x40;
    chr[9] = 0xE7; //get user register

    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("I2C Put TX: (" + AnsiString(str.Length()) + " bytes)");

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("I2C Put Byte error: " + e.Message);
        return false;
    }
    if(D->df>3)MThrd->ALS("I2C Put return string: " + StrToHexStr(str));
    if(str.SubString(1,4)!="\xB\xC2\xB\xC2" ||  str.c_str()[8]) {
        MThrd->ALS("I2CGetUR error write to device.");
        return false;
    }

    ::Sleep(100);

    chr[1] = 0xc3;
    chr[3] = 0xc3;
    chr[9] = 0x1;
    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("I2C Get TX: (" + AnsiString(str.Length()) + " bytes)");
    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("I2CRead error: " + e.Message);
        return false;
    }
    if(D->df>3)MThrd->ALS("I2C Get return string: " + StrToHexStr(str));
    if(str.SubString(1,4)!="\xB\xC3\xB\xC3" ||  str.c_str()[8]) {
        MThrd->ALS("I2CGetUR error read from device.");
        return false;
    }
    if(str.c_str()[5]!=0xB){
        MThrd->ALS("I2CGetUR error. Not 1 byte received.");
        return false;
    }

    UserReg = str.c_str()[10];
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMaster::I2CSHT20SetUserReg(unsigned char UserReg){
    AnsiString str;
    char chr[] = "\xB\xC2\xB\xC2\x0\xB\x0\x0\x0\x0\x0";

    chr[8] = 0x40;
    chr[9] = 0xE6; //set user register command
    chr[10] = UserReg;

    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("I2C Put TX: (" + AnsiString(str.Length()) + " bytes)");

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("I2C Put Byte error: " + e.Message);
        return false;
    }
    if(D->df>3)MThrd->ALS("I2C Put return string: " + StrToHexStr(str));
    if(str.c_str()[8]) {
        MThrd->ALS("I2CGetUR error write to device.");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMaster::I2CLPS331APGetWhoAmIReg(unsigned char &WhoAmIReg){
    AnsiString str;
    char chr[] = "\xB\xC2\xB\xC2\x0\xA\x0\x0\x0\x0";

    chr[8] = 0x5C;
    chr[9] = 0x0F; //get who_am_i register

    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("I2C Put TX: (" + AnsiString(str.Length()) + " bytes)");

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("I2C Put Byte error: " + e.Message);
        return false;
    }
    if(D->df>3)MThrd->ALS("I2C Put return string: " + StrToHexStr(str));
    if(str.SubString(1,4)!="\xB\xC2\xB\xC2" ||  str.c_str()[8]) {
        MThrd->ALS("I2CGetWAIR error write to device.");
        return false;
    }

    ::Sleep(10);

    chr[1] = 0xc3;
    chr[3] = 0xc3;
    chr[9] = 0x1;
    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("I2C Get TX: (" + AnsiString(str.Length()) + " bytes)");
    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("I2CRead error: " + e.Message);
        return false;
    }
    if(D->df>3)MThrd->ALS("I2C Get return string: " + StrToHexStr(str));
    if(str.SubString(1,4)!="\xB\xC3\xB\xC3" ||  str.c_str()[8]) {
        MThrd->ALS("I2CGetWAIR error read from device.");
        return false;
    }
    if(str.c_str()[5]!=0xB){
        MThrd->ALS("I2CGetWAIR error. Not 1 byte received.");
        return false;
    }

    WhoAmIReg = str.c_str()[10];
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMaster::I2CDevReadByte(unsigned char dev, unsigned char reg, unsigned char &val){
    AnsiString str;
    char chr[] = "\xB\xC2\xB\xC2\x0\xA\x0\x0\x0\x0";
    chr[8] = dev;
    chr[9] = reg;
    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("I2C TX: (" + AnsiString(str.Length()) + "bytes)");

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("I2CWrite error: " + e.Message);
        return false;
    }
    if(str.c_str()[8]) return false;

    chr[1] = 0xc3;
    chr[3] = 0xc3;
    chr[9] = 0x1;
    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("I2C TX: (" + AnsiString(str.Length()) + "bytes)");

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("I2CRead error: " + e.Message);
        return false;
    }
    if(str.c_str()[8]) return false;
    val = str.c_str()[10];

    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMaster::I2CDevWriteByte(unsigned char dev, unsigned char reg, unsigned char val){
    AnsiString str;
    char chr[] = "\xB\xC2\xB\xC2\x0\xB\x0\x0\x0\x0\x0";
    chr[8] = dev;
    chr[9] = reg;
    chr[10] = val;
    str = AnsiString(chr,sizeof(chr)-1);
    if(D->df>3)MThrd->ALS("I2C TX: (" + AnsiString(str.Length()) + "bytes)");

    try{
        ClientWrite(str);
        str = ClientRead(D->WaitForResponseInterval);
    }catch(Exception &e){
        MThrd->ALS("I2CWrite error: " + e.Message);
        return false;
    }
    if(str.c_str()[8]) return false;

    unsigned char d;
    if(I2CDevReadByte(dev, reg, d) && d == val) return true;

    MThrd->ALS("I2CDevWriteByte check: values not match!");

    return false;
}
//---------------------------------------------------------------------------
void __fastcall TMThread::Next(void){

    if(((TMaster*)FOwner)->ESRA==D->MaxESRA){
        ((TMaster*)FOwner)->URA++;
            if(((TMaster*)FOwner)->URA>=D->MaxURA){
                ((TMaster*)FOwner)->URA=0;
                chanIndex++;
                if(XL)XLIndex++;
                if(XLIndex==chanNum){
                    XL=false;
                    XLIndex=0;
                }
                paramIndex=0;
                if(chanIndex==chanNum){
                    chanIndex=0;
                }
                return;
            }
        return;
    }

    ((TMaster*)FOwner)->URA=0;

    if(!((TMaster*)FOwner)->mthread){
        paramIndex=0;
        chanIndex++;
        if(XL)XLIndex++;
        if(XLIndex==chanNum){
            XL=false;
            XLIndex=0;
        }
        if(chanIndex==chanNum)chanIndex=0;
        return;
    }

    paramIndex++;

    //if(!XL&&paramIndex>=((TMaster*)FOwner)->Channels[D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel].paramNumShort){    ////////////////paramNumShort
    if(!XL&&paramIndex>=D->srvrs[Fmdi]->Channels[chanIndex]->PAShortSize){
        paramIndex=0;
        chanIndex++;
        if(XL)XLIndex++;
        if(XLIndex==chanNum){
                XL=false;
                XLIndex=0;
        }
        if(chanIndex==chanNum)chanIndex=0;
        return;
    }
    //if(paramIndex>=((TMaster*)FOwner)->Channels[D->srvrs[Fmdi]->Channels[chanIndex]->RelayChannel].paramNum){         //////////////paramNum
    if(paramIndex>=D->srvrs[Fmdi]->Channels[chanIndex]->PASize){         //////////////paramNum
        paramIndex=0;
        chanIndex++;
        if(XL)XLIndex++;
        if(XLIndex==chanNum){
            XL=false;
            XLIndex=0;
        }
        if(chanIndex==chanNum){
            chanIndex=0;
            XL=false;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMThread::Next2(void){
    if(((TMaster*)FOwner)->ESRA>=D->MaxESRA){       // if 4 reading attempts
        ((TMaster*)FOwner)->ESRA = 0;
        ((TMaster*)FOwner)->URA++;
            if(((TMaster*)FOwner)->URA>=D->MaxURA){     // if 3x4 reading attempts
                ((TMaster*)FOwner)->URA = 0;
                if(paramIndex==0){
                    ((TMaster*)FOwner)->URA=0;
                    chanIndex++;
                    if(chanIndex==chanNum){
                        chanIndex=0;
                    }
                    if(D->df>2)ALS("R1 Chan" + AnsiString(chanIndex) + " Param" + AnsiString(paramIndex) + " XL" + AnsiString(XLIndex) + (XL?"X":""));
                    return; //переключает канал если не может прочитать первый параметр
                }
            }else{
                if(D->df>2)ALS("R2 Chan" + AnsiString(chanIndex) + " Param" + AnsiString(paramIndex) + " XL" + AnsiString(XLIndex) + (XL?"X":""));
                return; //если не достигла предела 3х10
            }
    }

    ((TMaster*)FOwner)->URA=0;

    if(!((TMaster*)FOwner)->mthread){
        paramIndex=paramNumShort;
        chanIndex++;
        if(chanIndex==chanNum)chanIndex=0;
        if(D->df>2)ALS("R3 Chan" + AnsiString(chanIndex) + " Param" + AnsiString(paramIndex) + " XL" + AnsiString(XLIndex) + (XL?"X":""));
        return;
    }

    paramIndex++;

    if(XLIndex<0&&paramIndex>=D->srvrs[Fmdi]->Channels[chanIndex]->PAShortSize){        ///////
        chanIndex++;
        if(chanIndex==chanNum){

            chanIndex=0;
            if(XL){
                paramIndex = D->srvrs[Fmdi]->Channels[chanIndex]->PAShortSize;              //////
                XLIndex++;
                if(D->df>2)ALS("R4 Chan" + AnsiString(chanIndex) + " Param" + AnsiString(paramIndex) + " XL" + AnsiString(XLIndex) + (XL?"X":""));
                return; //return point 4
            }

            getstart=true;
            Tdiff = MilliSecondsBetween(Now(),T3);
            if(D->df>1)ALS("R5 Cycle time = " + AnsiString(Tdiff));

            if(Tdiff>(D->ChannelsInterval-D->ParameterInterval)){
                SuspendTimer->Interval = D->ParameterInterval;
                if(D->df>2)ALS("R5 No delay set");
            }else{
                if(D->syncByTimer){
                    stopForSync = true;
                    if(D->df>2)ALS("R5 Stop For Sync Resume");
                    if(!D->SyncTimer->Enabled)D->SyncTimer->Enabled = true;
                }else{
                    SuspendTimer->Interval = D->ChannelsInterval - Tdiff;
                    if(D->df>2)ALS("R5 No Synchro start");
                }
            }//if

        }
        paramIndex=0;

        if(D->df>2)ALS("R5 Chan" + AnsiString(chanIndex) + " Param" + AnsiString(paramIndex) + " XL" + AnsiString(XLIndex) + (XL?"X":""));
        return; //return point 5
    }

    if(paramIndex==D->srvrs[Fmdi]->Channels[chanIndex]->PASize){
        chanIndex++;
        if(chanIndex==chanNum){

            getstart=true;
            Tdiff = MilliSecondsBetween(Now(),T3);
            if(D->df>1)ALS("R6 All channels cycle time = " + AnsiString(Tdiff) + " ms");

            if(Tdiff>(D->ChannelsInterval-D->ParameterInterval)){
                SuspendTimer->Interval = D->ParameterInterval;
            if(D->df>2)ALS("R6 No delay set");
            }else{
                if(D->syncByTimer){
                    stopForSync = true;
                    if(D->df>2)ALS("R6 Stop For Sync Resume");
                    if(!D->SyncTimer->Enabled)D->SyncTimer->Enabled = true;
                }else{
                    SuspendTimer->Interval = D->ChannelsInterval - Tdiff;
                    if(D->df>2)ALS("R6 No Synchro start");
                }
            }
            paramIndex=0;

            XLIndex=-1;
            chanIndex=0;

            XL=false;
            if(D->df>2)ALS("R6 Chan" + AnsiString(chanIndex) + " Param" + AnsiString(paramIndex) + " XL" + AnsiString(XLIndex) + (XL?"X":""));
            return;
        }
        paramIndex = D->srvrs[Fmdi]->Channels[chanIndex]->PAShortSize;              ////////
    }
    if(D->df>2)ALS("R7 Chan" + AnsiString(chanIndex) + " Param" + AnsiString(paramIndex) + " XL" + AnsiString(XLIndex) + (XL?"X":""));
}
//---------------------------------------------------------------------------
void __fastcall TMThread::XLTimerTimer(TObject *Sender){
    XL = true;
    if(D->df>2)ALS(
        "XL=" + AnsiString(XL?"1":"0") +
        " ES=" + AnsiString(ES?"1":"0") +
        " NC=" + AnsiString(NC?"1":"0") +
        " mthread=" + AnsiString(((TMaster*)FOwner)->mthread?"1":"0") +
        " MI=" + AnsiString(((TMaster*)FOwner)->MI?"1":"0") +
        " EI=" + AnsiString(((TMaster*)FOwner)->EI?"1":"0") +
        " Suspended=" + AnsiString(Suspended?"1":"0") +
        " RW=" + AnsiString(((TMaster*)FOwner)->RW?"1":"0")
    );
}
//---------------------------------------------------------------------------
void __fastcall TMThread::MThreadTerminate(TObject *Sender){
    ALS("Terminate() called. Sender = " + Sender->ClassName(), clBlue);
    if(D->df)ALS(
        "XL=" + AnsiString(XL?"1":"0") +
        " ES=" + AnsiString(ES?"1":"0") +
        " NC=" + AnsiString(NC?"1":"0") +
        " mthread=" + AnsiString(((TMaster*)FOwner)->mthread?"1":"0") +
        " MI=" + AnsiString(((TMaster*)FOwner)->MI?"1":"0") +
        " EI=" + AnsiString(((TMaster*)FOwner)->EI?"1":"0") +
        " Suspended=" + AnsiString(Suspended?"1":"0") +
        " Terminated=" + AnsiString(Terminated?"1":"0") +
        " RW=" + AnsiString(((TMaster*)FOwner)->RW?"1":"0")
    );
}
//---------------------------------------------------------------------------
void __fastcall TMaster::Setmthread(bool Amthread){
    if(Fmthread==Amthread)return;
    D->Form->LogMemo->Lines->Add("F!=A.");
    Fmthread = Amthread;
    if(Amthread){
        D->Form->LogMemo->Lines->Add("Amthread = true. Resume()");
        MThrd->Resume();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMaster::Scan(bool APower,bool ShortList){
    //D->versions->IndexOf();
    //bool scan = true;
    //bool poweron = APowerON;
    //Commands list = "*1,108";"*2,108";"*3,108";"*4,108";...
//for(int Channel=0;Cannel<12;Channel++){
    //if(poweron)SetPower(Channel,PowerState=1);
    //SelectPairByChannel(Channel){SelectPair(Channel/2);}
    //for(int chan=0;chan<256;chan++){
    //    AnsiString addr = MThrd->GetCCParam(chan,108);
    //    if(addr.Length()){
    //        int I2Cstate = GetCCParam(chan,126).ToInt();
    //        FInfoOb->lbChannels->Items[Channel] = AnsiString(Channel+1) + " " + addr + " on  " + Sstate_string[I2Cstate];
    //        Channels[Channel].state = 1;
    //        Channels[Channel].RS_Adress = addr.ToInt();
    //        Channels[Channel].SensorsState = I2Cstate;
    //    }else{
    //        lbChannels->Items[Channel] = AnsiString(Channel+1) + " " + "---" + " off " + "---";
    //    }
    //}
//}
}
//---------------------------------------------------------------------------
void __fastcall TMThread::SetES(bool AES){
    if(D->df>3)ALS("SetES(" + AnsiString(AES?"true)":"false)"));
    FES = AES;
}
//---------------------------------------------------------------------------
bool __fastcall TMThread::GetES(void){
    if(D->df>3)ALS("GetES()=" + AnsiString(FES?"true":"false"));
    return FES;
}
//---------------------------------------------------------------------------
int __fastcall TMThread::GetRelChan(void){
    //if(ParamIdx>MaxParamIdx)
    //    ChanIdx++;
    //if(ChanIdx>chanNum)ChanIdx = 1;
    //return ChansToRead[ChanIdx-1];
    return 0;
}
//---------------------------------------------------------------------------
int __fastcall TMThread::GetParam(void){
    return 0;
}
//---------------------------------------------------------------------------
int __fastcall TMaster::ClientWrite(AnsiString str){
    //if(D->df)MThrd->AddLogString("ClientWrite() called...");
    if(str.Length()!=str.c_str()[4]*256 + str.c_str()[5]){
        MThrd->ALS("ClientWrite() Error check length of string: " + AnsiString(str.Length()) + AnsiString("not equal") + AnsiString(str.c_str()[4]*256 + str.c_str()[5]));
        return 0;
    }
    if(MThrd->NC) return 0;
    try{
        if(D->df>3)MThrd->ALS("ClientWrite() Start writing: [" + StrToHexStr(str) + "] " + AnsiString(str.Length()) + " bytes");
        Client->Write(str);
        if(D->df>3)MThrd->ALS("ClientWrite() End writing...");
        return 1;
    }catch(Exception &e){
        //D->error_count++;
        MThrd->NC = true;
        MThrd->ALS("ClientWrite() Error writing data to master: " + e.Message + "\n    data = " + StrToHexStr(str), clRed);
    }
    return 0;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMaster::ClientRead(int timeout){
    if(D->df>3)MThrd->ALS("ClientRead() called...");
    if(MThrd->NC) return "";
    AnsiString result = "";
    MThrd->WaitForResponse(timeout);
    try{
        if(D->df>3)MThrd->ALS("ClientRead() Start reading...");
        //result = Client->CurrentReadBuffer();
        result = C_CRB();
        if(D->df>3)MThrd->ALS("ClientRead() Readed " + AnsiString(result.Length()) + " bytes");
    }catch(Exception &e){
        //D->error_count++;
        MThrd->NC = true;
        MThrd->ALS("ClientRead() Error reading data from master: " + e.Message, clRed);
    }
    if(D->df>3)MThrd->ALS("ClientRead() Stop response timer.");
    MThrd->ResponseTimer->Enabled = false;
    return result;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMaster::CheckAdequate(int param, AnsiString str){
    AnsiString AStr = str;
    AnsiString BStr;
    AnsiString temp_s;
    double temp_dbl;
    switch(param){
        case 100:
            if(AStr.ToIntDef(-1)>=0)return "Time(s) = " + str;
            break;
        case 102:
            if(AStr.Pos(".")){
                AStr = AStr.Insert(",",AStr.Pos("."));
                AStr = AStr.Delete(AStr.Pos("."),1);
            }
            try{temp_dbl = AStr.ToDouble();}catch(Exception &e){break;}
            if(temp_dbl>0 && temp_dbl<2200) return "UH = " + str;
            break;
        case 103:
            double u5v;
            if(AStr.Pos(".")){
                AStr = AStr.Insert(",",AStr.Pos("."));
                AStr = AStr.Delete(AStr.Pos("."),1);
            }
            try{u5v = AStr.ToDouble();}catch(Exception &e){break;}
            if(u5v>4 && u5v<6) return "U+5(V) = " + str;
            break;
        case 104:
            double u12v;
            if(AStr.Pos(".")){
                AStr = AStr.Insert(",",AStr.Pos("."));
                AStr = AStr.Delete(AStr.Pos("."),1);
            }
            try{u12v = AStr.ToDouble();}catch(Exception &e){break;}
            if(u12v>10 && u12v<14) return "U+12(V) = " + str;
            break;
        case 105:
            double u_5v;
            if(AStr.Pos(".")){
                AStr = AStr.Insert(",",AStr.Pos("."));
                AStr = AStr.Delete(AStr.Pos("."),1);
            }
            try{u_5v = AStr.ToDouble();}catch(Exception &e){break;}
            if(u_5v<-3 && u_5v>-8) return "U-5(V) = " + str;
            break;
        case 106:
            double T;
            if(AStr.Pos(".")){
                AStr = AStr.Insert(",",AStr.Pos("."));
                AStr = AStr.Delete(AStr.Pos("."),1);
            }
            try{T = AStr.ToDouble();}catch(Exception &e){break;}
            if(T>-10 && T<60) return "T(C) = " + str;
            break;
        case 108:
            if(0 < AStr.ToIntDef(-1) && AStr.ToIntDef(-1)<=255)return "Addr = " + str;
            break;
        case 109:
            if(AStr.ToIntDef(-1)==9600 || AStr.ToIntDef(-1)==115200) return "Baudrate = " + str;
            break;
        case 110:
            double HV;
            if(AStr.Pos(".")){
                AStr = AStr.Insert(",",AStr.Pos("."));
                AStr = AStr.Delete(AStr.Pos("."),1);
            }
            try{HV = AStr.ToDouble();}catch(Exception &e){break;}
            if(HV>0 && HV<2200) return "HV(V) = " + str;
            break;
        case 111:
            double THRSH;
            if(AStr.Pos(".")){
                AStr = AStr.Delete(AStr.Pos("."),1);
                AStr = AStr.Insert(",",AStr.Pos("."));
            }
            AStr = AStr.Delete(AStr.Pos("."),1);
            try{THRSH = AStr.ToDouble();}catch(Exception &e){break;}
            if(THRSH>0 && THRSH<2200) return "THRSH = " + str;
            break;
        case 116:
            if(AStr.ToIntDef(-1)>=0)return "Exposure(ms) = " + str;
            break;
        case 117:
            if(AStr.ToIntDef(-1)>=0)return "Interval(ms) = " + str;
            break;
        case 126:
            if(AStr.ToIntDef(-1)>=0)return "I2C state = " + str;
            break;
        case 127:
            if(AStr.Pos(".")){
                AStr = AStr.Insert(",",AStr.Pos("."));
                AStr = AStr.Delete(AStr.Pos("."),1);
            }
            try{temp_dbl = AStr.ToDouble();}catch(Exception &e){break;}
            if(temp_dbl>-20 && temp_dbl<60) return "Tin(C) = " + str;
            break;
        case 128:
            if(AStr.Pos(".")){
                AStr = AStr.Insert(",",AStr.Pos("."));
                AStr = AStr.Delete(AStr.Pos("."),1);
            }
            try{temp_dbl = AStr.ToDouble();}catch(Exception &e){break;}
            if(temp_dbl>0 && temp_dbl<100) return "RH(%) = " + str;
            break;
        case 131:
            if(AStr.Pos(".")){
                AStr = AStr.Insert(",",AStr.Pos("."));
                AStr = AStr.Delete(AStr.Pos("."),1);
            }
            try{temp_dbl = AStr.ToDouble();}catch(Exception &e){break;}
            if(temp_dbl>10 && temp_dbl<120) return "P(kPa) = " + str;
            break;
        case 132:
            if(AStr.Pos(".")){
                AStr = AStr.Insert(",",AStr.Pos("."));
                AStr = AStr.Delete(AStr.Pos("."),1);
            }
            try{temp_dbl = AStr.ToDouble();}catch(Exception &e){break;}
            if(temp_dbl>-20 && temp_dbl<60) return "Tout(C) = " + str;
            break;
/*        case 129:

            double val;
            for(int i=0;i<2;i++){
                if(AStr.Pos(",")){
                    BStr = AStr.SubString(1,AStr.Pos(",")-1);
                    if(BStr.Pos(".")){
                        BStr = BStr.Insert(",",BStr.Pos("."));
                        BStr = BStr.Delete(BStr.Pos("."),1);
                    }
                    try{val = BStr.ToDouble();}catch(Exception &e){return 1;}
                    AStr.Delete(1,AStr.Pos(","));
                }
                if(AStr.Pos("\r\n")){
                    BStr = AStr.SubString(1,AStr.Pos("\r\n")-1);
                    if(BStr.Pos(".")){
                        BStr = BStr.Insert(",",BStr.Pos("."));
                        BStr = BStr.Delete(BStr.Pos("."),1);
                    }
                    try{val = BStr.ToDouble();}catch(Exception &e){return 1;}
                    AStr.Delete(1,AStr.Pos("\r\n")+1);
                }
                if(val<-1 && val>1)return 1;
            }
            BStr = AStr.SubString(1,AStr.Pos("\x1A")-1);
            if(BStr.Pos(".")){
                BStr = BStr.Insert(",",BStr.Pos("."));
                BStr = BStr.Delete(BStr.Pos("."),1);
            }
            try{val = BStr.ToDouble();}catch(Exception &e){return 1;}
            if(val<-1 && val>1)return 1;
            return 0;
        case 130:
            double mval;
            for(int i=0;i<2;i++){
                if(AStr.Pos(",")){
                    BStr = AStr.SubString(1,AStr.Pos(",")-1);
                    if(BStr.Pos(".")){
                        BStr = BStr.Insert(",",BStr.Pos("."));
                        BStr = BStr.Delete(BStr.Pos("."),1);
                    }
                    try{mval = BStr.ToDouble();}catch(Exception &e){return 1;}
                    AStr.Delete(1,AStr.Pos(","));
                }
                if(AStr.Pos("\r\n")){
                    BStr = AStr.SubString(1,AStr.Pos("\r\n")-1);
                    if(BStr.Pos(".")){
                        BStr = BStr.Insert(",",BStr.Pos("."));
                        BStr = BStr.Delete(BStr.Pos("."),1);
                    }
                    try{mval = BStr.ToDouble();}catch(Exception &e){return 1;}
                    AStr.Delete(1,AStr.Pos("\r\n")+1);
                }
                if(mval<-1 && mval>1)return 1;
            }
            BStr = AStr.SubString(1,AStr.Pos("\x1A")-1);
            if(BStr.Pos(".")){
                BStr = BStr.Insert(",",BStr.Pos("."));
                BStr = BStr.Delete(BStr.Pos("."),1);
            }
            try{mval = BStr.ToDouble();}catch(Exception &e){return 1;}
            if(mval<-1 && mval>1)return 1;
            return 0;

*/
        case 199:
            double *arrayOfDouble;
            arrayOfDouble = new double[6];
            for(int i=0;i<5&&AStr.Pos(",");i++){
                try{
                    temp_s = AStr.SubString(1,AStr.Pos(",")-1);
                    if(temp_s.Pos(".")) temp_s.c_str()[temp_s.Pos(".")-1]=',';
                    arrayOfDouble[i] = temp_s.ToDouble();
                }catch(Exception &e){
                    MThrd->ALS("Error in converting string: [" + StrToHexStr(str) + "]", clPurple);
                    break;
                }
                AStr.Delete(1,AStr.Pos(","));
            }
            try{
                if(AStr.Pos(".")) AStr.c_str()[AStr.Pos(".")-1]=',';
                arrayOfDouble[5] = AStr.ToDouble();
            }catch(Exception &e){
                MThrd->ALS("Error in converting string: [" + StrToHexStr(str) + "]", clPurple);
                break;
            }
            delete [] arrayOfDouble;
            return "Ax,Ay,Az,Mx,My,Mz = " + str;
        default:
            return "[" + StrToHexStr(str) + "]";
    }
    return "";
}
//---------------------------------------------------------------------------
void __fastcall TMThread::SetNC(bool ANC){
    if(D->df>3)ALS("SetNC(" + AnsiString(ANC?"true":"false") +")");
    if(ANC&&!FNC){
        FNC = true;
        D->error_count++;
        ((TMaster*)FOwner)->error_count++;
        for(int i=0;i<12;i++){
            ((TMaster*)FOwner)->Channels[i].state = -1;
            ((TMaster*)FOwner)->Channels[i].SensorsState = -1;
        }
        return;
    }
    if(FNC&&!ANC){
        FNC = false;
    }
}
//---------------------------------------------------------------------------
bool __fastcall TMThread::GetNC(void){
    //if(D->df>3)AddLogString("GetNC() = " + AnsiString(FNC?"true":"false"));
    return FNC;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMaster::C_CRB(void){
    AnsiString temp_s="";
    int L;
    int att;

    if(temp_buf_c.Length())MThrd->ALS("C_CRB() temp_buf_c not empty. temp_buf_c = [" + StrToHexStr(temp_buf_c) + "]");

    for(att=0;att<4;att++){
        if(temp_buf_c.Length()<6){
            temp_buf_c += Client->CurrentReadBuffer();
            if(D->df>3)MThrd->ALS("C_CRB() R1 temp_s = [" + StrToHexStr(temp_s) + "] was <6 bytes");
            continue;
        }
        L = temp_buf_c[5]*256+temp_buf_c[6];
        if(temp_buf_c.Length()==L){
            temp_s = temp_buf_c;
            temp_buf_c = "";
            //MThrd->AddLogString("C_CRB() R2 temp_s = [" + StrToHexStr(temp_s) + "]");
            break;
        }
        if(temp_buf_c.Length()<L){
            temp_buf_c += Client->CurrentReadBuffer();
            if(D->df>3)MThrd->ALS("C_CRB() Receive [" + StrToHexStr(temp_buf_s) + "] was <L=" + AnsiString(L) + " bytes");
            continue;
        }
        if(temp_buf_c.Length()>L){
            temp_s = temp_buf_c.SubString(1,L);
            temp_buf_c.Delete(1,L);
            break;
        }
    }

    if(att&&D->df>3)MThrd->ALS("C_CRB() R5 Receive [" + StrToHexStr(temp_s) + "] in " + AnsiString(att) + " attempts.");

    return temp_s;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMaster::S_CRB(TIdPeerThread *AThread){
    AnsiString temp_s="";
    int L;
    int att;

    if(temp_buf_s.Length())MThrd->ALS("S_CRB() temp_buf_s not empty. temp_buf_s = [" + StrToHexStr(temp_buf_s) + "] temp_s = " + "[" + StrToHexStr(temp_s) + "]");

    for(att=0;att<4;att++){
        if(temp_buf_s.Length()<6){
            temp_buf_s += AThread->Connection->CurrentReadBuffer();
            if(D->df>3)MThrd->ALS("S_CRB() C1 temp_buf_s = [" + StrToHexStr(temp_buf_s) + "] temp_s = " + "[" + StrToHexStr(temp_s) + "]");
            continue;
        }
        L = temp_buf_s[5]*256+temp_buf_s[6];
        if(temp_buf_s.Length()==L){
            temp_s = temp_buf_s;
            temp_buf_s = "";
            if(D->df>3)MThrd->ALS("S_CRB() B1 temp_buf_s = [" + StrToHexStr(temp_buf_s) + "] temp_s = " + "[" + StrToHexStr(temp_s) + "]");
            break;
        }
        if(temp_buf_s.Length()<L){
            temp_buf_s += AThread->Connection->CurrentReadBuffer();
            if(D->df>3)MThrd->ALS("S_CRB() C2 temp_buf_s = [" + StrToHexStr(temp_buf_s) + "] temp_s = " + "[" + StrToHexStr(temp_s) + "]");
            continue;
        }
        if(temp_buf_s.Length()>L){
            temp_s = temp_buf_s.SubString(1,L);
            temp_buf_s.Delete(1,L);
            if(D->df>3)MThrd->ALS("S_CRB() B2 temp_buf_s = [" + StrToHexStr(temp_buf_s) + "] temp_s = " + "[" + StrToHexStr(temp_s) + "]");
            break;
        }
    }

    if(att&&D->df>3){
        MThrd->ALS("S_CRB() R1 temp_buf_s = [" + StrToHexStr(temp_buf_s) + "] in " + AnsiString(att) + " attempts. temp_s = " + "[" + StrToHexStr(temp_s) + "]");
    }

    return temp_s;
}
//---------------------------------------------------------------------------
void __fastcall TMThread::ALS(AnsiString str){
    MessageColor = clBlack;
    VALS_BUF = str;
    // yyy D->LogBuf->Add(str);
    Synchronize(VALS);
}
//---------------------------------------------------------------------------
void __fastcall TMThread::ALS(AnsiString str, TColor color){
    MessageColor = color;
    VALS_BUF = str;
    // yyy D->LogBuf->Add(str);
    Synchronize(VALS);
}
//---------------------------------------------------------------------------
void __fastcall TMThread::VALS(void){
    D->Form->LogMemo->SelAttributes->Color = MessageColor;
    // yyy D->LogBuf->Add(AnsiString().sprintf("[%4d] ",ThreadID) + VALS_BUF);
    D->Form->LogMemo->Lines->Add(AnsiString().sprintf("[%4d] ",ThreadID) + VALS_BUF);
}
//---------------------------------------------------------------------------
