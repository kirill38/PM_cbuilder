#include <vcl.h>
#include <stdio.h>
#include <fstream>
#include <time.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "mainUnit.h"
#include "clsUnit.h"
#include "masterUnit.h"
#include "srvUnit.h"
#include "func.h"
#include "VisUnit.h"
#include "EDUnit.h"
#include "CfgUnit.h"
#include "Unit2.h"
#include "PowerUnit.h"
#include "SettingsUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CGAUGES"
#pragma resource "*.dfm"
TMaster *Master;
TMainForm *MainForm;
TData *Data;
TDateTime DateTime;
TDateTime CDT;
TVisThread *VT;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner){}
//---------------------------------------------------------------------------
void __fastcall TMainForm::StartButtonClick(TObject *Sender){
    if(!runned){
        cbF3004->Enabled = false;
        stopAll_signal = false;
        StartButton->Caption = "Stop";
        if(!Data->AutoStart)LogMemo->SetFocus();
        DateTime = Time();
        StartAll(Sender);
        runned = true;
        ticks = 0;
        VT->Resume();
        mthreadCheckBox->OnClick = mthreadCheckBoxClick;
    }else{
        StartButton->Enabled = false;
        StartButton->Caption = "Wait...";
        runned = false;
        StopAll(Sender);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ReFreshTimerTimer(TObject *Sender){
    //LogMemo->Lines->AddStrings(LogBuf);
    //LogBuf->Clear();
    if(Data->f3004)LogMemo->Lines->Add("Using 3004 port");
    ReFreshTimer->Enabled = false;
    TDateTime T = Now();
        if(LogMemo->Lines->Count>20000){
            if(ltf){
                unsigned short y,m,d,h,min,s,ms;
                T.DecodeDate(&y,&m,&d);
                T.DecodeTime(&h,&min,&s,&ms);
                VT->SaveToFile("RE" + AnsiString().sprintf("%02d",y-2000) +
                                          AnsiString().sprintf("%02d",m) +
                                          AnsiString().sprintf("%02d",d) +
                                          AnsiString().sprintf("%02d",h) +
                                          AnsiString().sprintf("%02d",min) +
                                          AnsiString().sprintf("%02d",s) +
                                          ".txt");
            }
        }
 

    if(stopAll_signal && runned){
        LogMemo->Lines->Add("Stop from RefreshTimerTimer");
        ::Sleep(1000);
        stopAll_signal = false;
        StartButtonClick(Sender);
    }

    if(startAll_signal && !runned){
        ::Sleep(1000);
        startAll_signal = false;
        StartButtonClick(Sender);
        ss_busy = false;
    }

    if(!runned){
        StartButton->Caption = "Start";
        lbServers->Clear();
        mthreadCheckBox->OnClick = NULL;
        mthreadCheckBox->Checked = false;
        mthreadCheckBox->Enabled = false;
        ss_busy = false;
        //Data->f3004 = false;   //used for port 3004
        cbF3004->Enabled = true; //3004
        //Button1->Enabled = true;  //3004
        StartButton->Enabled = true;
        return;
    }

    TMaster *TM;

    if(lbServers->ItemIndex>=0)((TMaster*)lbServers->Items->Objects[lbServers->ItemIndex])->RefreshInfo(Sender);

    for(int i=0;i<lbServers->Count;i++){
        TM = (TMaster*)Masters->Items[i];

        lbServers->Items->Strings[i] = AnsiString(TM->Port) + (TM->EI ? " -> ":" <- ") +
            TM->Client->Host + " (" + AnsiString(TM->MThrd->ThreadID) + ")" +
            AnsiString(TM->MThrd->NC ? (TM->mthread ? " Offline *":" Offline"):(TM->mthread ?(TM->MI ? " Online *":" Online"):" Online"));

        if(ticks==300){
            TM->MThrd->rate = 1000*((float)(TM->MThrd->count - TM->MThrd->last))/(MilliSecondsBetween(T,TM->MThrd->Adt));
            TM->MThrd->last = TM->MThrd->count;
            TM->MThrd->Adt = T;
            if(i<10){
                Chart1->Series[i]->AddXY(SecondsBetween(TM->MThrd->Adt,TM->MThrd->Fdt),TM->MThrd->rate);
                while(Chart1->Series[i]->Count()>720)Chart1->Series[i]->Delete(0);
            }
        }
    }

    if(ticks==300)ticks=0;
    ticks++;

    StatusBar->Panels->Items[0]->Text = Data->filesize;
    StatusBar->Panels->Items[1]->Text = "file " + AnsiString().sprintf("%03d", Data->file_index) + " (" + AnsiString(Data->filesize*100/Data->MaxFileSize) + "%)";
    StatusBar->Panels->Items[2]->Text = "errors: " + AnsiString(Data->error_count);
    StatusBar->Panels->Items[3]->Text =
                        "PAR: " + AnsiString(Data->partial_read_count) +
                        " SDB: " + AnsiString(Data->SDBCounter) +
                        " NSR: " + AnsiString(Data->NSR) +
                        " INA: " + AnsiString(Data->inadequate) +
                        " RTB: " + AnsiString(RTBCounter);
    ReFreshTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SaveAs2Click(TObject *Sender){
    /*FILE *datfile;
    if(SaveDialog->Execute()){
        SaveDialog->FileName.cat_sprintf(".dat");
        datfile = fopen(SaveDialog->FileName.c_str(),"w+");
        fprintf(datfile,"Mode: %s\n",
            ModeComboBox->Items->Strings[Data->FromMode]);
        fprintf(datfile,"Channel    Events\n");
        fprintf(datfile,"-----------------\n");
        for(int i=0;i<Data->mainArray_Size[Data->FromMode];i++){
            fprintf(datfile,"%7u%10u\n",i,Data->mainArray[i]);
        }
        fclose(datfile);
        LogMemo->Lines->Add("Data saved to file: " + SaveDialog->FileName);
    }*/
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ApplicationEvents1Exception(TObject *Sender,
      Exception *E){
    LogMemo->Lines->Add("(!) ERROR: "+E->Message);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ApplicationEvents1Message(tagMSG &Msg,
      bool &Handled){
    if(Msg.message==WM_QUIT&&runned){
        LogMemo->Lines->Add(Msg.message);
        if(runned)StopAll(NULL);
        LogMemo->Lines->Add("No! By WINDOWS.");
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::StatusBarDrawPanel(TStatusBar *StatusBar,
        TStatusPanel *Panel, const TRect &Rect){
    TCanvas *pCanvas = StatusBar->Canvas;
    pCanvas->Brush->Color = clNavy;
    TRect R = Rect;
    if(runned){R.right = (Data->filesize)*Rect.Width()/Data->MaxFileSize;
    pCanvas->FillRect(R);}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender){
    startTime = Time();
    Path = ExtractFilePath(ParamStr(0));
    ExeFileName = ExtractFileName(ParamStr(0));
    ExeFileName = ExeFileName.SubString(1, ExeFileName.Pos(".")-1);
    Ini = new TIniFile(Path + ExeFileName + ".ini");

    ltf = Ini->ReadBool("Options","LogToFile",true);

    logfile.open("log.txt", ios::in | ios::out | ios::ate);
    if(logfile.is_open())LogMemo->Lines->Add("Log file is open...");
    else{
        LogMemo->Lines->Add("Log file error...");
        std::ofstream temp_df("log.txt");
        if(temp_df.is_open())LogMemo->Lines->Add("Log file created...");
        temp_df.close();
        logfile.clear();
        logfile.open("log.txt", ios::in | ios::out | ios::ate);
        LogMemo->Lines->Add("Log file is open...");
    }

    Data = new TData(Ini);
    Data->filepath = Path;
    Data->Form = MainForm;
    runned = false;
    VT = new TVisThread(true);   //VatchTog
    //VT->Resume();

    //LogMemo->Lines->Add("Test VT created");

    MainForm->Top = Ini->ReadInteger("Window", "Top", 390);
    MainForm->Left = Ini->ReadInteger("Window", "Left", 1040);
    Ini->WriteDateTime("Misc","OpenAt", startTime);

    cbDoSetBR->Checked = Data->doSetBR;
    CheckBox3->Checked = Data->doSetPower;
    cbVerbose->ItemIndex = Data->df;
    if(Data->doSetHV)MainMenu1->Items->Items[2]->Caption = "Set HV (!)";
    else MainMenu1->Items->Items[2]->Caption = "Set HV";
    cbSync->Checked = Data->syncByTimer;
    CheckBox2->Checked = Data->delay_tune;
    cbF3004->Checked = Data->f3004;
    cbAutoStart->Checked = Data->AutoStart;

    eChInterval->Text = AnsiString(Data->ChannelsInterval);

    Data->SyncTimer->OnTimer = SyncTimerTimer;
    
    CheckBox1->Checked = ltf;
    temp_buf_s = "";

    ss_busy = false;

    RichText_busy = false;
    RTBCounter = 0;
    //LogBuf = new TStringList();
    
    if(Data->AutoStart)StartButtonClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action){
    if(runned){
        StartButton->Enabled = false;
        StartButton->Caption = "Wait...";
        StopAll(Sender);
    }

    VT->Terminate();
    ::Sleep(2000);
    delete VT;
    delete Data;

    //delete LogBuf;

    Ini->WriteInteger("Window", "Top", MainForm->Top);
    Ini->WriteInteger("Window", "Left", MainForm->Left);
    Ini->WriteDateTime("Misc","LastUpTime", Time() - startTime);
    Ini->WriteDateTime("Misc","CloseAt", Time());
    Ini->WriteBool("Options","LogToFile",ltf);
    Ini->UpdateFile();
    delete Ini;
    logfile.close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::StartAll(TObject *Sender){
    TTS->Active = false;
    TTS->Bindings->Clear();
    Masters = new TList;
    TFastLineSeries *S;
    Chart1->RemoveAllSeries();
    AnsiString m_num;

    for(int i=0;i<Data->NumOfServers;i++){
        Master = new TMaster(Data,i);
        Master->Server = TTS;

        Masters->Add(Master);
        lbServers->Items->Add(AnsiString(Master->MDataIndex));
        LogMemo->Lines->Add("Master " + AnsiString(i) + " created.");

        if(i<10){
            S = new TFastLineSeries(NULL);
            m_num = Master->Client->Host;
            while(m_num.Pos('.'))m_num.Delete(1,m_num.Pos('.'));
            S->Title = m_num;
            S->LinePen->Width = 2;
            Chart1->AddSeries(S);
        }

    }

    for(int i=0;i<TTS->Bindings->Count;i++){
        TTS->Bindings->Items[i]->Port = Data->srvrs[i]->LocalPort;
        LogMemo->Lines->Add("TTS Port = " + AnsiString(TTS->Bindings->Items[i]->Port));
    }
    
    //TTS->Active = true;
    if(Data->syncByTimer)Data->SyncTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TTSExecute(TIdPeerThread *AThread){
    if(!runned){
        DoOfflineCommand(AThread);
        return;
    }

    int MasterNumber;
    for(MasterNumber = 0 ; MasterNumber<Masters->Count ; MasterNumber++)
        if(Data->srvrs[MasterNumber]->LocalPort == AThread->Connection->Binding->Port)break;
    ((TMaster*)Masters->Items[MasterNumber])->ExtSession(AThread);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::StopAll(TObject *Sender){
    for(int i=0;i<Masters->Count;i++){
        delete ((TMaster*)Masters->Items[i]);
    }
    TTS->ThreadMgr->TerminateThreads(TTS->ThreadMgr->ActiveThreads);
    TTS->Active = false;
    TTS->Bindings->Clear();
    if(Data->SyncTimer->Enabled)Data->SyncTimer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Button1Click(TObject *Sender){

    Button1->Enabled = false;   //3004
    Data->f3004 = true;         //3004
    StartButtonClick(Sender);   //3004

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::lbServersClick(TObject *Sender){
    if(lbServers->ItemIndex>=0){
        mthreadCheckBox->Checked = ((TMaster*)Masters->Items[lbServers->ItemIndex])->mthread;
        mthreadCheckBox->Enabled = true;
        return;
    }
    mthreadCheckBox->Checked = false;
    mthreadCheckBox->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::mthreadCheckBoxClick(TObject *Sender){
    ((TMaster*)Masters->Items[lbServers->ItemIndex])->mthread = mthreadCheckBox->Checked;
    if(Data->df>3)LogMemo->Lines->Add("mthreadCheckBoxClick ii=" + AnsiString(lbServers->ItemIndex) + " Sender: " +Sender->ClassName());
    ((TMaster*)Masters->Items[lbServers->ItemIndex])->RefreshInfo(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Clearlog1Click(TObject *Sender){
    LogMemo->Lines->Clear();
    for(int i=0;i<Chart1->SeriesCount();i++){
        Chart1->Series[i]->Clear();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ScanButtonClick(TObject *Sender){
    //LogMemo->Lines->Add(MessageDlg("Scan with power ON?", mtInformation, TMsgDlgButtons() << mbNo << mbYes << mbCancel, 0));
    //return;

    switch(MessageDlg("Scan with power ON?", mtInformation, TMsgDlgButtons() << mbNo << mbYes << mbCancel, 0)){
        case 2:
            return;
        case 6:
            LogMemo->Lines->Add("Scan with power ON.");
            //for(int i=0;i<Masters->Count;i++){
                //((TMaster*)Masters->Items[i])->Scan(true);
            //}
            break;
        case 7:
            LogMemo->Lines->Add("Scan with NO power on.");
            //for(int i=0;i<Masters->Count;i++){
                //((TMaster*)Masters->Items[i])->Scan(false);
            //}
    }
    switch(MessageDlg("Scan 1...12 (short list) addresses?", mtInformation, TMsgDlgButtons() << mbNo << mbYes << mbCancel, 0)){
        case 2:
            return;
        case 6:
            LogMemo->Lines->Add("Scan with power ON.");
            //for(int i=0;i<Masters->Count;i++){
                //((TMaster*)Masters->Items[i])->Scan(true);
            //}
            break;
        case 7:
            LogMemo->Lines->Add("Scan with NO power on.");
            //for(int i=0;i<Masters->Count;i++){
                //((TMaster*)Masters->Items[i])->Scan(false);
            //}
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::cbVerboseClick(TObject *Sender){
    Data->df = cbVerbose->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ParseData(AnsiString filename){

    clock_t sta,end;
    sta = clock();
    int RecordsCount = 0;
    int fi = 0;
    int iFL = 0;
    TStringList* ST;

    ST = new TStringList();

    Data->datafile_to_read.open(filename.c_str(), ios::in | ios::out | ios::binary);
    Data->datafile_to_read.seekg(0, ios::beg);

    Data->datafile_to_read.read((char*)&Data->dfhdr, sizeof(TDFhdr));
    LogMemo->Lines->Add("Reading data file: " + filename);
    
    LogMemo->Lines->Add("Version: " + AnsiString(Data->dfhdr.DataFileVersion));
    LogMemo->Lines->Add("Raw data 16 bytes: " + StrToHexStr(AnsiString(Data->dfhdr.rawdata,16)));
    LogMemo->Lines->Add("CRC16 of file header: " + AnsiString(Data->dfhdr.CRC));

    ST->Add("Version: " + AnsiString(Data->dfhdr.DataFileVersion));
    ST->Add("Raw data 16 bytes: " + StrToHexStr(AnsiString(Data->dfhdr.rawdata,16)));
    ST->Add("CRC16 of file header: " + AnsiString(Data->dfhdr.CRC));

    Thdr hdr;
    AnsiString s;
    LogMemo->Lines->Add("Timestamp\tMaster\tChannel\tParameter\tDT\tValeSize\tValue");

    while(!Data->datafile_to_read.eof()){
        for(fi=0;fi<1000;fi++){
            Data->datafile_to_read.read((char*)&hdr, sizeof(Thdr));
            if(Data->datafile_to_read.eof()){
                LogMemo->Lines->Add("EOF Reached");
                break;
            }
            RecordsCount++;
            if(hdr.ValueSize){
                s.SetLength(hdr.ValueSize);
                Data->datafile_to_read.read((char*)&s.c_str()[0], hdr.ValueSize);

            }else{
                s = "[empty]";
            }

            LogMemo->Lines->Add(hdr.PCTime.DateTimeString() + "\t" +
                        AnsiString(hdr.masterIP[3]) + "\t" +
                        AnsiString(hdr.RelayChannel) + "(" + AnsiString(hdr.Address) + ")" + "\t" +
                        AnsiString(hdr.Parameter) + "\t" +
                        AnsiString(hdr.GetDTms) + "\t" +
                        AnsiString(hdr.ValueSize) + "\t" +
                        StrToHexStr(s)
                        );
            ST->Add(hdr.PCTime.DateTimeString() + "\t" +
                        AnsiString(hdr.masterIP[3]) + "\t" +
                        AnsiString(hdr.RelayChannel) + "(" + AnsiString(hdr.Address) + ")" + "\t" +
                        AnsiString(hdr.Parameter) + "\t" +
                        AnsiString(hdr.GetDTms) + "\t" +
                        AnsiString(hdr.ValueSize) + "\t" +
                        s
                        );
        }
        iFL++;
        LogMemo->Lines->Add("Save data file: " + AnsiString().sprintf("data%04d",iFL) + ".txt");
        ST->SaveToFile(AnsiString().sprintf("data%04d",iFL) + ".txt");
        ST->Clear();
    }
    Data->datafile_to_read.close();
    Data->datafile_to_read.clear();

    end = clock();

    LogMemo->Lines->Add("Total " + AnsiString(RecordsCount) + " records proceed in " + AnsiString().sprintf("%2.1fs",(float)(end-sta)/1000));

    delete ST;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ReadDataFileClick(TObject *Sender){
    if(OpenDialog1->Execute()){
        ParseData(OpenDialog1->FileName);
    }
}
//---------------------------------------------------------------------------
/*void __fastcall AddLogString(AnsiString LogString){
    while(RichText_busy){
        RTBCounter++;
        ::Sleep(5);
    }
    RichText_busy = true;
    //if(Data)Data->line_counter++;
    TDateTime T(Now());
    LogMemo->Lines->Add("L" + AnsiString().sprintf("%06d",0) + " " + T.TimeString() + ":" + LogString);
    if(LogMemo->Lines->Count>20000){
        if(ltf){
            unsigned short y,m,d,h,min,s,ms;
            T.DecodeDate(&y,&m,&d);
            T.DecodeTime(&h,&min,&s,&ms);
            LogMemo->Lines->SaveToFile("RE" + AnsiString().sprintf("%02d",y-2000) +
                                          AnsiString().sprintf("%02d",m) +
                                          AnsiString().sprintf("%02d",d) +
                                          AnsiString().sprintf("%02d",h) +
                                          AnsiString().sprintf("%02d",min) +
                                          AnsiString().sprintf("%02d",s) +
                                          ".txt");
        }
        LogMemo->Clear();
    }
    if(ltf)logfile<<AnsiString().sprintf("L%06d ",0).c_str()<<T.DateTimeString().c_str()<<":"<<LogString.c_str()<<"\n";
    RichText_busy = false;
}*/
//---------------------------------------------------------------------------
void __fastcall TMainForm::CheckBox1Click(TObject *Sender){
    ltf = CheckBox1->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Button3Click(TObject *Sender){
    //TDateTime T(Now());
    //if(!VT)VT = new TVisThread(false);
    //VT->AddLogString(T.TimeString());
    //VT->AddLogString(T.TimeString());

    std::ofstream ofl;
    ofl.open("testofl.txt",ios::in | ios::out | ios::ate);
    ofl << LogMemo->Lines->GetText();
    ofl.close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::cbDoSetBRClick(TObject *Sender){
    Data->doSetBR = cbDoSetBR->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::cbSyncClick(TObject *Sender){
    Data->syncByTimer = cbSync->Checked;
    //if(Data->syncThreads && Masters->Count)Data->SyncTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SyncTimerTimer(TObject *Sender){
    for(int i=0;i<Masters->Count;i++){
        if(((TMaster*)Masters->Items[i])->MThrd->stopForSync &&
           ((TMaster*)Masters->Items[i])->MThrd->Suspended)
            
            ((TMaster*)Masters->Items[i])->MThrd->Resume();
    }
    if(!Data->syncByTimer)Data->SyncTimer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::eChIntervalChange(TObject *Sender){
    Data->ChannelsInterval = eChInterval->Text.ToIntDef(60000);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClientListChange(TObject *Sender){
    lbClients->Items->Clear();
    for(int i=0;i<ConnectedThreads->ActiveThreads->Count;i++){
        lbClients->Items->Add(AnsiString(((TIdPeerThread*)ConnectedThreads->ActiveThreads->Items[i])->Connection->Binding->PeerIP) + " -> "
            + AnsiString(((TIdPeerThread*)ConnectedThreads->ActiveThreads->Items[i])->Connection->Binding->Port) + " ("
            + AnsiString(((TIdPeerThread*)ConnectedThreads->ActiveThreads->Items[i])->ThreadID) + ")");
    }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMainForm::S_CRB(TIdPeerThread *AThread){
    AnsiString temp_s="";
    int L;
    int att;

    if(temp_buf_s.Length())LogMemo->Lines->Add("Offline C_CRB() temp_buf_s not empty. temp_buf_s = [" + StrToHexStr(temp_buf_s) + "]");

    for(att=0;att<4;att++){
        if(temp_buf_s.Length()<6){
            temp_buf_s += AThread->Connection->CurrentReadBuffer();
            if(Data->df)LogMemo->Lines->Add("Offline S_CRB() R1 Receive [" + StrToHexStr(temp_buf_s) + "] was <6 bytes");
            continue;
        }
        L = temp_buf_s[5]*256+temp_buf_s[6];
        if(temp_buf_s.Length()==L){
            temp_s = temp_buf_s;
            temp_buf_s = "";
            break;
        }
        ::Sleep(1000);
        if(temp_buf_s.Length()<L){
            temp_buf_s += AThread->Connection->CurrentReadBuffer();
            LogMemo->Lines->Add("Offline S_CRB() Receive [" + StrToHexStr(temp_buf_s) + "] was <L=" + AnsiString(L) + " bytes");
            continue;
        }
        if(temp_buf_s.Length()>L){
            temp_s = temp_buf_s.SubString(1,L);
            temp_buf_s.Delete(1,L);
            break;
        }
    }

    if(att&&Data->df){
        LogMemo->Lines->Add("Offline S_CRB() Receive [" + StrToHexStr(temp_s) + "] in " + AnsiString(att) + " attempts.");
    }

    return temp_s;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TTSConnect(TIdPeerThread *AThread){
    temp_buf_s = "";
    ClientListChange(NULL);
    AThread->Priority = tpHighest;
}
//---------------------------------------------------------------------------
int __fastcall TMainForm::DoOfflineCommand(TIdPeerThread *AThread){
    AnsiString temp_s = S_CRB(AThread);
    int strLength = CheckInputStr(temp_s);
    if(strLength){
        LogMemo->Lines->Add("--OK string: " + StrToHexStr(temp_s));
    }else{
        LogMemo->Lines->Add("--Wrong string: " + StrToHexStr(temp_s));
        return 0;
    }
    if(strLength>=11 && temp_s.Pos("\xF\xFA\xF\xFA")){
    //ID ID ID ID LL LL 00 00 LL MM NN XX XX XX 0D
    //                           GG
        if(temp_s.SubString(10,1) == "G"){
            int r = Data->PMCommand(temp_s.SubString(11,temp_s.Pos("\xD")-11),-1,AThread);
            if(r>0){
                return 1;
            }else{
                LogMemo->Lines->Add("--Unknown G-command for system.");
                return 0;
            }
        }
        if(temp_s.SubString(10,1) == "M"){
            unsigned int m = temp_s.SubString(11,1).c_str()[0];
            LogMemo->Lines->Add("--Offline command for master " + Data->srvrs[m]->Address);
            return 1;
        }
        LogMemo->Lines->Add("--Command no G or M");
        return 0;
    }
    LogMemo->Lines->Add("--Offline command too short.");
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TTSDisconnect(TIdPeerThread *AThread){
    ClientListChange(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CheckBox2Click(TObject *Sender){
    Data->delay_tune = CheckBox2->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ReconnectClick(TObject *Sender){
    ((TMaster*)Masters->Items[popupitm])->MThrd->NC = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::lbServersMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y){
    if(Button==mbRight){
        popupitm = lbServers->ItemAtPos(TPoint(X,Y),true);
        //if(popupitm>=0)pmMaster->Popup(Mouse->CursorPos.x,Mouse->CursorPos.y);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CheckBox3Click(TObject *Sender){
    Data->doSetPower = CheckBox3->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::pmMasterPopup(TObject *Sender){
    if(popupitm<0){
        pmMaster->Items->Items[0]->Enabled = false;
        pmMaster->Items->Items[0]->Caption = "Nothing to reconnect";
    }else{
        pmMaster->Items->Items[0]->Enabled = true;
        pmMaster->Items->Items[0]->Caption = "Reconnect " + ((TMaster*)Masters->Items[popupitm])->Client->Host;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Savetologtxt1Click(TObject *Sender){
    LogMemo->Lines->SaveToFile("log_em.txt");
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ExtractDataClick(TObject *Sender){
    EDForm->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::cbF3004Click(TObject *Sender){
    Data->f3004 = cbF3004->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::cbAutoStartClick(TObject *Sender){
    Data->AutoStart = cbAutoStart->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button2Click(TObject *Sender)
{
    //YRange->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Chart1DblClick(TObject *Sender)
{
    if(!YRange->Showing)YRange->Show();
    else YRange->Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::PowerManager1Click(TObject *Sender)
{
    if(PowerForm==NULL){
        Application->CreateForm(__classid(TPowerForm), &PowerForm);
        LogMemo->Lines->Add("CreateForm(PowerForm) Called");
    }
    PowerForm->D = Data;
    PowerForm->ShowModal();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::Settings1Click(TObject *Sender)
{
        SettingsForm->ShowModal();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::TabControl1Change(TObject *Sender){
        switch(TabControl1->TabIndex){
                case 0:
                        Chart1->Visible = true;
                        Chart2->Visible = false;
                break;
                case 1:
                        Chart1->Visible = false;
                        Chart2->Visible = true;
                break;
        }
}
//---------------------------------------------------------------------------


