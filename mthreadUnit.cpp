//---------------------------------------------------------------------------
#pragma hdrstop
#include "mthreadUnit.h"
#include "masterUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall TMThread::TMThread(TMaster *OMaster):TThread(false){
    FMaster = OMaster;
}
//---------------------------------------------------------------------------
__fastcall TMThread::~TMThread(void){}
//-------------------------------------------------- Main Thread Function ---
void __fastcall TMThread::Execute(){
    int sleep = 0;
    while(!StopCheckPoint()){
        ::Sleep(sleep);

        //case SCP {
        //    1:  //if(!Client->
                //Connected()){
                //    ((TIdTCPClient*)D->tcpClients_list->Items[ThreadNumber])->Connect();


            //try to connect/reconnect ifttcr()
                //validation of connection
                //if connection error then sleep=10000(fEx); break;
        //    2:  //monitoring cycle
                //
        //    3:  //external session
                //suspend thread (0 = infinite stop)
        //    4:  //end monitoring session
                //
        //    0:  //close connection
                //destroy objects
                //end thread
        //}

        try{
        }catch(Exception &exception){
            AddLogString(exception.Message);
            sleep = 10000; //need to check error type (continue or not)
            //if connection error then reconnect
            //if other error type
            //then ???
        }
    } //end while
}
//---------------------------------------------------------------------------
bool __fastcall TMThread::StopCheckPoint(void){
    MSG msg;
    if(SF){
        //TimeoutTimer->Enabled = false;
        AddLogString("Stopped by user.");
        return true;
    }
	while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
        if(msg.message==WM_QUIT){
            //TimeoutTimer->Enabled = false;
            AddLogString("Stopped by Windows.");
            return true;
        }else{
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
	}
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TMThread::AddLogString(AnsiString str){
    LogString = str;
    Synchronize(VALS);
}
//---------------------------------------------------------------------------
void __fastcall TMThread::VALS(void){
   // MainForm->LogMemo->Lines->Add(LogString);
}
//---------------------------------------------------------------------------
