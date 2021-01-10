//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "VisUnit.h"
#include "masterUnit.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall TVisThread::TVisThread(bool CreateSuspended)
    :TThread(CreateSuspended){
    Form = MainForm;
    str = "";
    FreeOnTerminate = false;
}
//---------------------------------------------------------------------------
__fastcall TVisThread::~TVisThread(void){
    AddLogString("VT destructor called");
}
//---------------------------------------------------------------------------
void __fastcall TVisThread::Execute(){
    AddLogString("start VT");
    while(!Terminated){
        Synchronize(ReFresh);
        //Form->LogMemo->Refresh();
        //AddLogString("VT tick");
        //AddStringsToMemo();
        //Synchronize(AddStringsToMemo);
        ::Sleep(333);
    }
    AddLogString("end VT");
}
//---------------------------------------------------------------------------
void __fastcall TVisThread::AddToMemo(void){
    Form->LogMemo->Lines->Add(str);
}
//---------------------------------------------------------------------------
void __fastcall TVisThread::AddStringsToMemo(void){
    Form->LogMemo->Lines->AddStrings(Data->LogBuf);
    //Form->LogMemo->Refresh();
    Data->LogBuf->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TVisThread::AddLogString(AnsiString Astr){
    str = T.TimeString() + " " + Astr;
    Synchronize(AddToMemo);
}
//---------------------------------------------------------------------------
void __fastcall TVisThread::STF(void){
    Form->LogMemo->Lines->SaveToFile(fname);
}
//---------------------------------------------------------------------------
void __fastcall TVisThread::SaveToFile(AnsiString afname){
    fname = afname;
    Synchronize(STF);
}
//---------------------------------------------------------------------------
void __fastcall TVisThread::ReFresh(void){
    //AddStringsToMemo();
    if(!Form->runned){
        Form->LogMemo->Lines->Add("System stopped");
        ::Sleep(1000);
        Form->StartButton->Caption = "Start";
        Form->lbServers->Clear();
        Form->mthreadCheckBox->OnClick = NULL;
        Form->mthreadCheckBox->Checked = false;
        Form->mthreadCheckBox->Enabled = false;
        Form->ss_busy = false;
        Form->StartButton->Enabled = true;
        Form->cbF3004->Enabled = true;
        Form->LogMemo->Lines->Add("VT Suspended");
        Suspend();
    }

    T = Now();

        if(Form->LogMemo->Lines->Count>1000){
            if(Form->ltf){
                unsigned short y,m,d,h,min,s,ms;
                T.DecodeDate(&y,&m,&d);
                T.DecodeTime(&h,&min,&s,&ms);
                VT->SaveToFile(Form->Path + "\\savedlog\\" +
                                    "RE" + AnsiString().sprintf("%02d",y-2000) +
                                          AnsiString().sprintf("%02d",m) +
                                          AnsiString().sprintf("%02d",d) +
                                          AnsiString().sprintf("%02d",h) +
                                          AnsiString().sprintf("%02d",min) +
                                          AnsiString().sprintf("%02d",s) +
                                          ".txt");
            }
            Form->LogMemo->Clear();
        }


    if(Form->stopAll_signal && Form->runned){
        Form->LogMemo->Lines->Add("Stop from RefreshTimerTimer");
        ::Sleep(1000);
        Form->stopAll_signal = false;
        Form->StartButtonClick(NULL);
    }

    if(Form->startAll_signal && !Form->runned){
        ::Sleep(1000);
        Form->startAll_signal = false;
        Form->StartButtonClick(NULL);
        Form->ss_busy = false;
    }

    TMaster *TM;
    AnsiString addr;

    if(Form->lbServers->ItemIndex>=0)((TMaster*)Form->Masters->Items[Form->lbServers->ItemIndex])->RefreshInfo(NULL);

    for(int i=0;i<Form->lbServers->Count;i++){
        TM = (TMaster*)Form->Masters->Items[i];

        addr = TM->Client->Host;
        while(addr.Pos("."))addr.Delete(1,addr.Pos("."));

        Form->lbServers->Items->Strings[i] = AnsiString(TM->Port) + (TM->EI ? " > ":" < ") +
            addr + AnsiString().sprintf(" (%*d)",(TM->MThrd->ThreadID<1000?5:4),TM->MThrd->ThreadID) + " err:" + AnsiString(TM->error_count) + " " +
            AnsiString(TM->MThrd->NC ? (TM->mthread ? " ------ *":" ------"):(TM->mthread ?(TM->MI ? " Online *":" Online"):" Online"));

        if(Form->ticks==180){
            TM->MThrd->rate = 1000*((float)(TM->MThrd->count - TM->MThrd->last))/(MilliSecondsBetween(T,TM->MThrd->Adt));
            TM->MThrd->last = TM->MThrd->count;
            TM->MThrd->Adt = T;
            if(i<10){
                Form->Chart1->Series[i]->AddXY(SecondsBetween(TM->MThrd->Adt,TM->MThrd->Fdt),TM->MThrd->rate);
                while(Form->Chart1->Series[i]->Count()>500)Form->Chart1->Series[i]->Delete(0);
            }
        }
    }
    if(Form->ticks==180)Form->ticks=0;
    Form->ticks++;

    Form->StatusBar->Panels->Items[0]->Text = Data->filesize;
    Form->StatusBar->Panels->Items[1]->Text = "file " + AnsiString().sprintf("%03d", Data->file_index) + " (" + AnsiString(Data->filesize*100/Data->MaxFileSize) + "%)";
    Form->StatusBar->Panels->Items[2]->Text = "errors: " + AnsiString(Data->error_count);
    Form->StatusBar->Panels->Items[3]->Text =
                        "PAR: " + AnsiString(Data->partial_read_count) +
                        " SDB: " + AnsiString(Data->SDBCounter) +
                        " NSR: " + AnsiString(Data->NSR) +
                        " INA: " + AnsiString(Data->inadequate) +
                        " RTB: " + AnsiString(Form->RTBCounter) +
                        " FRC: " + ((Data->cfc - Data->ofc) ? AnsiString(Data->ofc) + "(!)" : AnsiString(Data->ofc));
                        //(!(Data->cfc - Data->ofc) ? AnsiString(Data->ofc) : AnsiString(Data->ofc) + "(!)")

}
//--------------------------------------------------------------------------
