//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PowerUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPowerForm *PowerForm;
TPowerThread *PowerThread;
AnsiString AS_buf = "";
AnsiString AI_buf = "";
unsigned short pow_state = 0;
byte ai_chan=0;
unsigned int cntr = 0;
bool started = false;

//---------------------------------------------------------------------------
unsigned int CRC16_2(unsigned char *buf, int len)
{  
  unsigned int crc = 0xFFFF;
  for (int pos = 0; pos < len; pos++)
  {
  crc ^= (unsigned int)buf[pos];    // XOR byte into least sig. byte of crc

  for (int i = 8; i != 0; i--) {    // Loop over each bit
    if ((crc & 0x0001) != 0) {      // If the LSB is set
      crc >>= 1;                    // Shift right and XOR 0xA001
      crc ^= 0xA001;
    }
    else                            // Else LSB is not set
      crc >>= 1;                    // Just shift right
    }
  }

  return crc;
}

//---------------------------------------------------------------------------
__fastcall TPowerForm::TPowerForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TPowerForm::PowerCLBClickCheck(TObject *Sender)
{
    //ListBox1->Items->Add(PowerCLB->ItemIndex);
    //PowerCLB->Checked[PowerCLB->ItemIndex] = true;
    for(int i=0;i<PowerCLB->Count;i++){
        if(PowerCLB->Checked[i])pow_state |= 1 << i;
        else pow_state &= ~(1 << i);
    }

    //PowerCLB->ItemEnabled[PowerCLB->ItemIndex] = false;
    //PowerCLB->ItemEnabled[PowerCLB->ItemIndex] = true;
    //ListBox1->Items->Add(AnsiString().sprintf("%04X",pow_state));
    UdpSocket1->Sendln("@01"+AnsiString().sprintf("%04X",pow_state),"\r");
    //werCLB->Checked[PowerCLB->ItemIndex] = true;
    //PowerCLB->
}
//---------------------------------------------------------------------------
void __fastcall TPowerThread::ALS(AnsiString str){
    MessageColor = clBlack;
    VALS_BUF = str;
    // yyy D->LogBuf->Add(str);
    Synchronize(VALS);
}
//---------------------------------------------------------------------------
void __fastcall TPowerThread::ALS(AnsiString str, TColor color){
    MessageColor = color;
    VALS_BUF = str;
    Synchronize(VALS);
}
//---------------------------------------------------------------------------
void __fastcall TPowerThread::VALS(void){
    D->Form->LogMemo->SelAttributes->Color = MessageColor;
    D->Form->LogMemo->Lines->Add(AnsiString().sprintf("[%4d] ",ThreadID) + VALS_BUF);
}
//---------------------------------------------------------------------------
__fastcall TPowerThread::TPowerThread(TObject *Owner):TThread(false){
    Priority = tpHighest;
    OnTerminate = PowerThreadTerminate;
    FOwner = Owner;
    SuspendTimer = new TTimer(NULL);
    SuspendTimer->Interval = 999;
    SuspendTimer->Enabled = false;
    SuspendTimer->OnTimer = SuspendTimerTimer;
    ResponseTimer = new TTimer(NULL);
    ResponseTimer->Interval = 1000;
    ResponseTimer->Enabled = false;
    ResponseTimer->OnTimer = ResponseTimerTimer;
    started = false;
    D = ((TPowerForm*)Owner)->D;

    MessageColor = D->Form->LogMemo->SelAttributes->Color;

}
//---------------------------------------------------------------------------
__fastcall TPowerThread::~TPowerThread(void){
    ALS("TPowerThread destructor called...");
    delete SuspendTimer;
    delete ResponseTimer;
}
//------------------------------------------------- Power Thread Function ---
void __fastcall TPowerThread::Execute(){

    unsigned int con_try_cnt=5;
    char buf1[] = {0x0,0x3,0x10,0x3,0x0,0x1,0x71,0x1B};
    char buf2[] = {0x0,0x3,0x10,0x3,0x0,0x1};
    PowerForm->ListBox1->Items->Add( AnsiString().sprintf( "%04X", CRC16_2(buf2,6) ) );

    PowerForm->UdpSocket1->RemoteHost = PowerForm->LabeledEdit1->Text;
    PowerForm->UdpSocket1->RemotePort = PowerForm->LabeledEdit2->Text;

    while(!PowerForm->UdpSocket1->Connected && con_try_cnt){
        PowerForm->ListBox1->Items->Add("Try connect...");
        PowerForm->UdpSocket1->Open();
        con_try_cnt--;
        ::Sleep(500);
    }
    if(!con_try_cnt){
        PowerForm->ListBox1->Items->Add("Connecting fault.");
        return;
    }

    started = true;
    do{
        if(!PowerForm->UdpSocket1->Connected){
            PowerForm->LabeledEdit4->Text = "--offline--";
            PowerForm->UdpSocket1->Open();
        }
        if(!PowerForm->UdpSocket2->Connected){
            PowerForm->LabeledEdit7->Text = "--offline--";
            PowerForm->UdpSocket2->Open();
        }

        PowerForm->UdpSocket1->Sendln("#02" + AnsiString().sprintf("%X",ai_chan),"\r");
        ResponseTimer->Enabled = true;
        Suspend();

        cntr++;

        if(ai_chan>=8){
            PowerForm->UdpSocket1->Sendln("@01","\r");
            ResponseTimer->Enabled = true;
            Suspend();

            ai_chan=0;
            SuspendTimer->Enabled = true;
            Suspend();

            PowerForm->UdpSocket2->SendBuf(buf1,8,0);
            ResponseTimer->Enabled = true;
            Suspend();
        }else ai_chan++;

    }while(!Terminated);
    PowerForm->UdpSocket1->Close();
    PowerForm->UdpSocket2->Close();
}
//---------------------------------------------------------------------------
void __fastcall TPowerForm::FormCreate(TObject *Sender)
{
    PThrd = new TPowerThread((TObject*)this);
}
//---------------------------------------------------------------------------
void __fastcall TPowerForm::FormClose(TObject *Sender,
      TCloseAction &Action){

    D->Form->LogMemo->Lines->Add(Action);
    D->Form->LogMemo->Lines->Add("PowerForm->FormClose() called...");
    PThrd->Terminate();
    D->Form->LogMemo->Lines->Add("PowerThread->Terminate()...ok");
    PThrd->Resume();
    D->Form->LogMemo->Lines->Add("PowerThread->Resume()...ok");
    PThrd->WaitFor();
    D->Form->LogMemo->Lines->Add("PowerThread->WaitFor()...ok");
    delete PThrd;
    D->Form->LogMemo->Lines->Add("PowerForm->FormClose()...ok");
    Action = caFree;
    PowerForm = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TPowerThread::PowerThreadTerminate(TObject *Sender){
    ALS("Terminate PowerThread() called. Sender = " + Sender->ClassName(), clBlue);
}
//---------------------------------------------------------------------------
void __fastcall TPowerThread::SuspendTimerTimer(TObject *Sender){
    SuspendTimer->Enabled = false;
    Resume();
}
//---------------------------------------------------------------------------
void __fastcall TPowerThread::ResponseTimerTimer(TObject *Sender){
    ResponseTimer->Enabled = false;
    PowerForm->ListBox1->Items->Add("no answer...ch" + AnsiString(ai_chan) + " " + AnsiString(cntr));
    Resume();
    //D->error_count++;
    //if(D->df>0)ALS("Wait for response timeout (" + AnsiString(ResponseTimer->Interval) + " ms). Set NC = true. " +
    //"GetParamCC(" + AnsiString(ChansToRead[chanIndex]) + "," + AnsiString(((TMaster*)FOwner)->CurrentChannelRef->ParamsToRead[paramIndex]) + ") for master " +
    //((TMaster*)FOwner)->Client->Host, clRed);
    //NC = true;
    //((TMaster*)FOwner)->Client->Disconnect();
}
//---------------------------------------------------------------------------
void __fastcall TPowerForm::Button2Click(TObject *Sender)
{
    //char buf[50]={0};
    //int buf_l=0;
    //sockaddr_in isk;
    //if(UdpSocket1->Select())ListBox1->Items->Add("Connecting successful.");
    //isk = UdpSocket1->GetSocketAddr("192.168.1.60","4001");
    //ListBox1->Items->Add(isk.sin_addr.S_un.
    //UdpSocket1->
    //UdpSocket1->Active = true;

    //ListBox1->Items->Add("Received " + AnsiString(UdpSocket1->ReceiveBuf(buf,50,0)));
    //if(UdpSocket1->WaitForData(1000))ListBox1->Items->Add("...socket ready");


    if(Button2->Caption=="On"){
        UdpSocket1->Sendln("@010003","\r");
        ListBox1->Items->Add("bytes received: " + AnsiString(UdpSocket1->BytesReceived));
        Button2->Caption="Off";
    }else{
        UdpSocket1->Sendln("@010001","\r");
        ListBox1->Items->Add("bytes received: " + AnsiString(UdpSocket1->BytesReceived));
        Button2->Caption="On";

    }

    
}
//---------------------------------------------------------------------------

void __fastcall TPowerForm::UdpSocket1Receive(TObject *Sender, PChar Buf,
      int &DataLen)
{
    ListBox1->Items->Add("...something received: " + AnsiString(Buf,DataLen));
}
//---------------------------------------------------------------------------

void __fastcall TPowerForm::UdpSocket1Send(TObject *Sender, PChar Buf,
      int &DataLen)
{
    //ListBox1->Items->Add("...something send: " + AnsiString(Buf,DataLen));
}
//---------------------------------------------------------------------------

void __fastcall TPowerForm::UdpSocket1CreateHandle(TObject *Sender)
{
    ListBox1->Items->Add("...handle created");
}
//---------------------------------------------------------------------------

void __fastcall TPowerForm::UdpSocket1Error(TObject *Sender,
      int SocketError)
{
    ListBox1->Items->Add("...error " + AnsiString(SocketError));
}
//---------------------------------------------------------------------------

void __fastcall TPowerForm::Button3Click(TObject *Sender)
{
    unsigned int con_try_cnt=5;

    while(UdpSocket1->Connected && con_try_cnt){
        ListBox1->Items->Add("Disconnecting attempt #" + AnsiString(con_try_cnt-4) + "...");
        UdpSocket1->Close();
        con_try_cnt--;
        ::Sleep(1000);
    }
    if(!con_try_cnt){
        ListBox1->Items->Add("Disonnecting fault.");
        return;
    }

    ListBox1->Items->Add("Disconnecting successful.");
    //UdpSocket1->Close();
}
//---------------------------------------------------------------------------

void __fastcall TPowerForm::IdUDPServer1UDPRead(TObject *Sender,
      TStream *AData, TIdSocketHandle *ABinding)
{
    char *buf;
    buf = new char [AData->Size];
    AData->Read(buf,AData->Size);
    if(ABinding->Port == 4002){
        AnsiString s = "";
        for(int i=0;i<AData->Size;i++){
            //s = s + AnsiString().sprintf("%02X ",(unsigned char)buf[i]);
            s = s + buf[i];
        }

        AI_buf = AI_buf + s;

        if(AI_buf.Length()==7){
            //ListBox1->Items->Add("Received to port 4002 " + AnsiString(AData->Size) + " bytes: " + AI_buf);
            LabeledEdit7->Text = AnsiString((unsigned short)AI_buf[5]) + " mA";
            AI_buf = "";
            if(started && PThrd->ResponseTimer->Enabled){
                PThrd->ResponseTimer->Enabled = false;
                PThrd->Resume();
            }
        }
        //ListBox1->Items->Add("Received to port 4002 " + AnsiString(AData->Size) + " bytes: " + s);
    }else{
    if(buf[AData->Size-1]!='\r'){
        AS_buf = AS_buf + AnsiString(buf,AData->Size);
    }else{
        AS_buf = AS_buf + AnsiString(buf,AData->Size-1);

        if(AS_buf.SubString(2,1)=="+" || AS_buf.SubString(1,1)=="-")
            PowerCLB->Items->Strings[ai_chan] = AS_buf;
        else{
            LabeledEdit4->Text = AS_buf;
        }
        AS_buf = "";

        if(started && PThrd->ResponseTimer->Enabled){
            PThrd->ResponseTimer->Enabled = false;
            PThrd->Resume();
        }
    }
    }
    delete[] buf;
}
//---------------------------------------------------------------------------


void __fastcall TPowerForm::Button4Click(TObject *Sender)
{
    ListBox1->Items->Add(UdpSocket1->LocalHostAddr());
    ListBox1->Items->Add(UdpSocket1->LocalHostName());
    ListBox1->Items->Add(UdpSocket1->LocalPort);
    //UdpSocket1->
    //sockaddr_in isk;
    //isk = UdpSocket1->GetSocketAddr("192.168.1.14",4001);


}
//---------------------------------------------------------------------------
void __fastcall TPowerForm::PowerCLBClick(TObject *Sender){
    //PowerCLB->State[0] = cbGrayed;
}
//---------------------------------------------------------------------------

void __fastcall TPowerForm::LabeledEdit1Change(TObject *Sender)
{
    PowerForm->UdpSocket1->RemoteHost = PowerForm->LabeledEdit1->Text;
}
//---------------------------------------------------------------------------
void __fastcall TPowerForm::LabeledEdit2Change(TObject *Sender)
{
    PowerForm->UdpSocket1->RemotePort = PowerForm->LabeledEdit2->Text;
}
//---------------------------------------------------------------------------
void __fastcall TPowerForm::LabeledEdit5Change(TObject *Sender)
{
    PowerForm->UdpSocket2->RemoteHost = PowerForm->LabeledEdit5->Text;
}
//---------------------------------------------------------------------------
void __fastcall TPowerForm::LabeledEdit6Change(TObject *Sender)
{
    PowerForm->UdpSocket2->RemotePort = PowerForm->LabeledEdit6->Text;
}
//---------------------------------------------------------------------------


void __fastcall TPowerForm::Edit1Change(TObject *Sender)
{
        //223
        IdUDPServer1->Bindings->Items[0]->IP = Edit1->Text;
        IdUDPServer1->Bindings->Items[1]->IP = Edit1->Text;
}
//---------------------------------------------------------------------------

void __fastcall TPowerForm::CheckBox4Click(TObject *Sender)
{
        IdUDPServer1->Active = CheckBox4->Checked;
}
//---------------------------------------------------------------------------



