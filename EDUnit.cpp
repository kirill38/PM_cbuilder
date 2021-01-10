//---------------------------------------------------------------------------

#include <vcl.h>
#include <vector>
#pragma hdrstop

#include "EDUnit.h"
#include "clsUnit.h"
#include "mainUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEDForm *EDForm;
TEDThread *EDT;
//---------------------------------------------------------------------------
__fastcall TDataSet::TDataSet(AnsiString iM, int iCH, int iP){
    M = iM;
    CH = iCH;
    P = iP;
    D = 1;
    for(int i=0;i<3;i++){
        IP[i] = iM.SubString(1,iM.Pos(".")-1).ToIntDef(0);iM.Delete(1,iM.Pos("."));
    }
    IP[3] = iM.ToIntDef(0);

}
//---------------------------------------------------------------------------
__fastcall TDataSet::TDataSet(AnsiString iM, int iCH, int iP, int iD){
    M = iM;
    CH = iCH;
    P = iP;
    D = iD;
    for(int i=0;i<3;i++){
        IP[i] = iM.SubString(1,iM.Pos(".")-1).ToIntDef(0);iM.Delete(1,iM.Pos("."));
    }
    IP[3] = iM.ToIntDef(0);
}
//---------------------------------------------------------------------------
__fastcall TDataSet::TDataSet(AnsiString iM, int iCH, int iP, int iD, int iADDR, int iX){
    M = iM;
    CH = iCH;
    P = iP;
    D = iD;
    X = iX;
    ADDR = iADDR;
    for(int i=0;i<3;i++){
        IP[i] = iM.SubString(1,iM.Pos(".")-1).ToIntDef(0);iM.Delete(1,iM.Pos("."));
    }
    IP[3] = iM.ToIntDef(0);
}
//---------------------------------------------------------------------------
__fastcall TDataSet::TDataSet(AnsiString iM, int iCH, int iP, int iD, int iADDR, int iX, int iDT, bool iCount){
    count_cum = iCount;
    recs_parsed = 0;
    recs_count = 0;
    M = iM;
    CH = iCH;
    P = iP;
    D = iD;
    X = iX;
    DT = iDT;
    ADDR = iADDR;
    for(int i=0;i<3;i++){
        IP[i] = iM.SubString(1,iM.Pos(".")-1).ToIntDef(0);iM.Delete(1,iM.Pos("."));
    }
    IP[3] = iM.ToIntDef(0);
}
//---------------------------------------------------------------------------
__fastcall TDataSet::TDataSet(AnsiString iM, int iCH, int iP, int iD, int iADDR, int iX, int iDT, bool iCount, int N, int P){
    n_pf = N;
    p_n = P;

    count_cum = iCount;
    recs_parsed = 0;
    recs_count = 0;
    M = iM;
    CH = iCH;
    P = iP;
    D = iD;
    X = iX;
    DT = iDT;
    ADDR = iADDR;
    for(int i=0;i<3;i++){
        IP[i] = iM.SubString(1,iM.Pos(".")-1).ToIntDef(0);iM.Delete(1,iM.Pos("."));
    }
    IP[3] = iM.ToIntDef(0);
}
//---------------------------------------------------------------------------
__fastcall TDataSet::~TDataSet(void){
    VV.clear();
}
//---------------------------------------------------------------------------
__fastcall TEDForm::TEDForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::FormCreate(TObject *Sender)
{
    for(int i=0;i<Data->NumOfServers;i++){
        MasterCB->Items->Add(Data->srvrs[i]->Address);
    }
    EDT = new TEDThread(true);
    EDT->np = 0;
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::MasterCBChange(TObject *Sender)
{
    //ChannelCB->Items->Clear();
    //ParameterCB->Items->Clear();
    //for(int i=0;i<Data->srvrs[MasterCB->ItemIndex]->NumOfChannels;i++){
    //    ChannelCB->Items->Add(AnsiString(Data->srvrs[MasterCB->ItemIndex]->Channels[i]->Address));
    //}
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::ChannelCBChange(TObject *Sender)
{
    //ParameterCB->Items->Clear();
    //int ver = Data->srvrs[MasterCB->ItemIndex]->Channels[ChannelCB->ItemIndex]->Version;
    //for(int i=0;i<Data->PASizes[ver];i++){
    //    ParameterCB->Items->Add(AnsiString(Data->PArrays[ver][i]));
    //}
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::AddButtonClick(TObject *Sender){
    TDataSet *d;
    d = new TDataSet(MasterCB->Text,ChannelCB->Text.ToIntDef(0)-1,ParameterCB->Text.ToIntDef(-1),NDimCB->Text.ToIntDef(1),AddressCB->Text.ToIntDef(0),XidxCB->Text.ToIntDef(0),DataTypeCB->Text.ToIntDef(0),CheckBox2->Checked);
    //ParametersLB->Items->Add(MasterCB->Text + " Ch" + ChannelCB->Text + " " + ParameterCB->Text);
    ParametersLB->Items->AddObject(MasterCB->Text + " Ch" + ChannelCB->Text + "(" + AnsiString(d->ADDR) + ")" +
        " " + ParameterCB->Text + "(" + AnsiString(d->X) + "," + AnsiString(d->D) + ")", (TObject*)d);

    EDT->PL->Add((TObject*)d);

    EDT->np++;
    //MainForm->Chart1->AddSeries()
    FromPickerChange(Sender);
    ToPickerChange(Sender);
    //delete d; //было закоментировано
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::ParametersLBDblClick(TObject *Sender)
{
    delete (TDataSet*)ParametersLB->Items->Objects[ParametersLB->ItemIndex];
    ParametersLB->Items->Delete(ParametersLB->ItemIndex);
    EDT->PL->Delete(ParametersLB->ItemIndex);
    EDT->np--;
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::CloseButtonClick(TObject *Sender)
{
    EDForm->Close();
}
//---------------------------------------------------------------------------
void __fastcall TEDThread::ReadData(void){
    TDFhdr dfhdr;
    Thdr hdr;
    int recs;
    AnsiString s,ip;
    recs = 0;
    //unsigned int cnt = 0;
    
    //Form->ParametersLB->Items->Add(AnsiString(((TDataSet*)PL->Items[0])->M));
    //Form->ParametersLB->Items->Add(AnsiString(((TDataSet*)PL->Items[0])->CH));
    //Form->ParametersLB->Items->Add(AnsiString(((TDataSet*)PL->Items[0])->P));
    //Form->ParametersLB->Items->Add(AnsiString(((TDataSet*)PL->Items[0])->D));
    Form->ParametersLB->Items->Add("--- start processing thread ---");

    for(int i=0;i<EDT->FileRange->Count && !Terminated;i++){
        Form->ParametersLB->Items->Add(EDT->FileRange->Strings[i] + " processing...");
        file2read.open(EDT->FileRange->Strings[i].c_str(),ios::in | ios::out | ios::binary);
        file2read.seekg(0, ios::beg);
        file2read.read((char*)&dfhdr, sizeof(TDFhdr));
        while(!file2read.eof() && !Terminated){
            file2read.read((char*)&hdr, sizeof(Thdr));
            ip = AnsiString(hdr.masterIP[0]) + "." +
                AnsiString(hdr.masterIP[1]) + "." +
                AnsiString(hdr.masterIP[2]) + "." +
                AnsiString(hdr.masterIP[3]);

            if(hdr.ValueSize){
                s.SetLength(hdr.ValueSize);
                file2read.read((char*)&s.c_str()[0], hdr.ValueSize);
            }
            for(int j=0;j<np && !Terminated;j++){ // for all added parameters
                if(!IsNan(hdr.PCTime.Val)
                    && hdr.ValueSize
                    && hdr.DataType == ((TDataSet*)PL->Items[j])->DT
                    && hdr.PCTime >= t1
                    && hdr.PCTime <= t2
                    && hdr.RelayChannel == ((TDataSet*)PL->Items[j])->CH
                    && hdr.Address == ((TDataSet*)PL->Items[j])->ADDR
                    && hdr.Parameter == ((TDataSet*)PL->Items[j])->P
                    && ip == ((TDataSet*)PL->Items[j])->M
                    ) {
                        recs++;
                        ((TDataSet*)PL->Items[j])->recs_count++;
                        if(((TDataSet*)PL->Items[j])->count_cum)ParseParameter(hdr.PCTime,hdr.Parameter,s,((TDataSet*)PL->Items[j])->recs_count,((TDataSet*)Form->ParametersLB->Items->Objects[j])->VV);
                        else{
                            ((TDataSet*)PL->Items[j])->recs_parsed =
                            ((TDataSet*)PL->Items[j])->recs_parsed +
                            ParseParameter(hdr.PCTime,hdr.Parameter,s,0,((TDataSet*)Form->ParametersLB->Items->Objects[j])->VV);
                        }
                    }
            }
        }
        Form->ParametersLB->Items->Add("records found: " + AnsiString(recs));
        file2read.close();
        file2read.clear();
    }

    Form->ParametersLB->Items->Add("--- end processing thread ---");
    Form->ParametersLB->Items->Add("Total records found: " + AnsiString(recs));
    for(int i_ds=0;i_ds<np;i_ds++){
        Form->ParametersLB->Items->Add("DS" + AnsiString(i_ds+1) + ": records found/parsed: " +
            AnsiString(((TDataSet*)PL->Items[i_ds])->recs_parsed) + " / " +
            AnsiString(((TDataSet*)PL->Items[i_ds])->recs_count)
            );
    }
}
//---------------------------------------------------------------------------
void __fastcall TEDThread::DisplayFiles(AnsiString path)
{
    TSearchRec sr;

    TDateTime L1,Ai,AI,AJ;
    AnsiString FirstFile,FNameI,FNameJ,T;

    t1 = Form->FromPicker->DateTime;
    t2 = Form->ToPicker->DateTime;
    dtype = Form->DataTypeCB->Text.ToIntDef(0);
    //TStringList *FileRange;
    //FileRange = new TStringList();

            Form->ParametersLB->Items->Add("--- start file search ---");
    //ParametersLB->Items->Add(L1.DateTimeString());

    //Code for read file list for t1 - t2 range

    if(FindFirst(path + "datacat\\*", 0x3F, sr) == 0)
    {
        do
        {
            if (sr.Attr != faDirectory && ExtractFileExt(sr.Name).SubString(2,3).ToIntDef(-1) != -1 ){
                //ParametersLB->Items->Add(sr.Name);
                Ai = TDateTime(2000 + (unsigned short)sr.Name.SubString(1,2).ToIntDef(0),
                                (unsigned short)sr.Name.SubString(3,2).ToIntDef(0),
                                (unsigned short)sr.Name.SubString(5,2).ToIntDef(0))
                    + TDateTime((unsigned short)sr.Name.SubString(7,2).ToIntDef(0),
                                (unsigned short)sr.Name.SubString(9,2).ToIntDef(0),
                                (unsigned short)sr.Name.SubString(11,2).ToIntDef(0),
                                0x0
                                );

                if (Ai > L1 && Ai < t1){
                    L1 = Ai;
                    FirstFile = sr.Name;
                }
                if (Ai > t1 && Ai < t2) FileRange->Add(path + "datacat\\" + sr.Name);

                //ParametersLB->Items->Add(Ai.DateTimeString());
            }
        } while (FindNext(sr) == 0);
        FindClose(sr);
    }
        FileRange->Insert(0,path + "datacat\\" + FirstFile);

    //Form->ParametersLB->Items->Add(FirstFile);
    //FileRange->Exchange(5,10);

    // Code for sort file list by dates in filenames

    int I, J;
    for (I=0; I <= FileRange->Count-2; I++)
        for (J = FileRange->Count-1; J >= I+1; J--){
            //ParametersLB->Items->Add(AnsiString(I) + "," + AnsiString(J));
            FNameI = ExtractFileName(FileRange->Strings[I]);
            FNameJ = ExtractFileName(FileRange->Strings[J]);
            AI =  TDateTime(2000 + (unsigned short)FNameI.SubString(1,2).ToIntDef(0),
                                (unsigned short)FNameI.SubString(3,2).ToIntDef(0),
                                (unsigned short)FNameI.SubString(5,2).ToIntDef(0))
                    + TDateTime((unsigned short)FNameI.SubString(7,2).ToIntDef(0),
                                (unsigned short)FNameI.SubString(9,2).ToIntDef(0),
                                (unsigned short)FNameI.SubString(11,2).ToIntDef(0),
                                0x0
                                );
            AJ = TDateTime(2000 + (unsigned short)FNameJ.SubString(1,2).ToIntDef(0),
                                (unsigned short)FNameJ.SubString(3,2).ToIntDef(0),
                                (unsigned short)FNameJ.SubString(5,2).ToIntDef(0))
                    + TDateTime((unsigned short)FNameJ.SubString(7,2).ToIntDef(0),
                                (unsigned short)FNameJ.SubString(9,2).ToIntDef(0),
                                (unsigned short)FNameJ.SubString(11,2).ToIntDef(0),
                                0x0
                                );

            if (AI > AJ){
                FileRange->Exchange(I,J);
                //ParametersLB->Items->Add(AnsiString(I) + "," + AnsiString(J));
            }
        }

        Form->ParametersLB->Items->Add(AnsiString(FileRange->Count) + " file(s) found:");
        for(int i=0;i<FileRange->Count;i++){
            Form->ParametersLB->Items->Add(FileRange->Strings[i]);
        }
        Form->ParametersLB->Items->Add("for time range:");

        Form->ParametersLB->Items->Add("from " + t1.DateTimeString());
        Form->ParametersLB->Items->Add("to " + t2.DateTimeString());
        //Form->ParametersLB->Items->Add(L1.DateTimeString());
        //ParametersLB->Items->Add(L2.DateTimeString());
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::Button1Click(TObject *Sender)
{
    EDT->DisplayFiles(ExtractFilePath(ParamStr(0)));
    EDT->Resume();
}
//---------------------------------------------------------------------------
void __fastcall TEDThread::ParseParameter(TDateTime iDT, int iP, AnsiString iS, std::vector<XY> &oV){

    XY xy;
    if(iS.Pos(".")) iS.c_str()[iS.Pos(".")-1]=',';
    try{
        xy.Y = iS.ToDouble();
        xy.X = iDT;
        oV.push_back(xy);
    }catch(Exception &e){
        Form->ParametersLB->Items->Add(iDT.DateTimeString() + " " + iS + " err");
    }
}
//---------------------------------------------------------------------------
int __fastcall TEDThread::ParseParameter(TDateTime iDT, int iP, AnsiString iS, long count, std::vector<double*> &oV){

    AnsiString temp_s;
    double *arrayOfDouble;
    int ret;

    switch(iP){

    case 107: //Get Noise (P/SEC, D)
        arrayOfDouble = new double[3];
        arrayOfDouble[0] = iDT;
        for(int i=1;i<2&&iS.Pos(",");i++){
            try{
                temp_s = iS.SubString(1,iS.Pos(",")-1);
                if(temp_s.Pos(".")) temp_s.c_str()[temp_s.Pos(".")-1]=',';
                arrayOfDouble[i] = temp_s.ToDouble();
                ret=1;
            }catch(Exception &e){
                ret=0;
                Form->ParametersLB->Items->Add(iDT.DateTimeString() + " " + iS + " err1");
            }
            iS.Delete(1,iS.Pos(","));
        }
        try{
            if(iS.Pos(".")) iS.c_str()[iS.Pos(".")-1]=',';
            arrayOfDouble[2] = iS.ToDouble();
            ret=1;
        }catch(Exception &e){
            ret=0;
            Form->ParametersLB->Items->Add(iDT.DateTimeString() + " " + iS + " err2");
        }
        oV.push_back(arrayOfDouble);
    break;

    case 199:   //Get Orientation data
        arrayOfDouble = new double[7];
        arrayOfDouble[0] = iDT;
        for(int i=1;i<6&&iS.Pos(",");i++){
            try{
                temp_s = iS.SubString(1,iS.Pos(",")-1);
                if(temp_s.Pos(".")) temp_s.c_str()[temp_s.Pos(".")-1]=',';
                arrayOfDouble[i] = temp_s.ToDouble();
                ret=1;
            }catch(Exception &e){                                             
                ret=0;
                Form->ParametersLB->Items->Add(iDT.DateTimeString() + " " + iS + " err1");
            }
            iS.Delete(1,iS.Pos(","));
        }
        try{
            if(iS.Pos(".")) iS.c_str()[iS.Pos(".")-1]=',';
            arrayOfDouble[6] = iS.ToDouble();
            ret=1;
        }catch(Exception &e){
            ret=0;
            Form->ParametersLB->Items->Add(iDT.DateTimeString() + " " + iS + " err2");
        }
        oV.push_back(arrayOfDouble);
    break;

    default:
        arrayOfDouble = new double[2];
        arrayOfDouble[0] = iDT;
        if(iS.Pos(".")) iS.c_str()[iS.Pos(".")-1]=',';
        if(count)arrayOfDouble[1] = (double)count;
        else{
            try{
                arrayOfDouble[1] = iS.ToDouble();
                ret = 1;
            }catch(Exception &e){
                ret=0;
                Form->ParametersLB->Items->Add(iDT.DateTimeString() + " " + iS + " err");
            }
        }
        oV.push_back(arrayOfDouble);
    break;
    }
    return ret;
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::DrawButtonClick(TObject *Sender)
{
    TPointSeries *S;
    MainForm->Chart1->RemoveAllSeries();
    MainForm->Chart1->Title->Text->Strings[0] = "Parameters graph";
    ParametersLB->Items->Add("Draw started...");
    for(int k=0;k<EDT->np;k++){
        S = new TPointSeries(NULL);
        //S->Pointer->Style = psSmallDot;
        //TSeriesPointerStyle
        //S->Pointer->InflateMargins = false;
        S->Pointer->Pen->Visible = false;
        S->Pointer->HorizSize = 1;
        S->Pointer->VertSize = 1;
        S->Title = ((TDataSet*)ParametersLB->Items->Objects[k])->M;
        while(S->Title.Pos('.'))S->Title.Delete(1,S->Title.Pos('.'));
        S->Title = S->Title + "|" + AnsiString(((TDataSet*)ParametersLB->Items->Objects[k])->CH+1) +
            "|" + AnsiString(((TDataSet*)ParametersLB->Items->Objects[k])->ADDR);
        S->XValues->DateTime = true;
        MainForm->Chart1->AddSeries(S);
        for(unsigned int i=0; i<((TDataSet*)ParametersLB->Items->Objects[k])->VV.size(); i++){
            //MainForm->Chart1->Series[k]->AddXY( ((TDataSet*)ParametersLB->Items->Objects[k])->V[i].X,
            //                               ((TDataSet*)ParametersLB->Items->Objects[k])->V[i].Y);
            MainForm->Chart1->Series[k]->AddXY( ((TDataSet*)ParametersLB->Items->Objects[k])->VV[i][((TDataSet*)ParametersLB->Items->Objects[k])->X],
                                            ((TDataSet*)ParametersLB->Items->Objects[k])->VV[i][((TDataSet*)ParametersLB->Items->Objects[k])->D]);
            //ParametersLB->Items->Add(AnsiString(((TDataSet*)ParametersLB->Items->Objects[0])->V[i].Y));
        }
    }
    ParametersLB->Items->Add("...Draw done.");
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::DateTimePicker1Change(TObject *Sender){
    FromPicker->Time = DateTimePicker1->Time;
    //EDForm->Caption = FromPicker->DateTime.DateTimeString();
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::DateTimePicker2Change(TObject *Sender){
    ToPicker->Time = DateTimePicker2->Time;
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::FromPickerChange(TObject *Sender){
    FromPicker->Time = DateTimePicker1->Time;
    //EDForm->Caption = FromPicker->DateTime.DateTimeString();
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::ToPickerChange(TObject *Sender){
    ToPicker->Time = DateTimePicker2->Time;
}
//---------------------------------------------------------------------------
__fastcall TEDThread::TEDThread(bool CreateSuspended)
    :TThread(CreateSuspended){
    PL = new TList();
    FileRange = new TStringList();
    Form = EDForm;
}
//---------------------------------------------------------------------------
__fastcall TEDThread::~TEDThread(void){
    TDataSet *d;
    for(int i_item;i_item<PL->Count;i_item++){
        d = (TDataSet*)PL->Items[i_item];
        delete d;
    }
    delete PL;
    delete FileRange;
}
//---------------------------------------------------------------------------
void __fastcall TEDThread::Execute(){
    ReadData();
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::Button2Click(TObject *Sender)
{
    //ProgressBar2->Parent = Button2;
    //ProgressBar2->Align = alClient;   
}
//---------------------------------------------------------------------------

void __fastcall TEDForm::FormClose(TObject *Sender, TCloseAction &Action){
    EDT->Terminate();
    EDT->WaitFor();
    delete EDT;
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::Button4Click(TObject *Sender){
    TDataSet *d;
    for(int i_chan=0;i_chan<12;i_chan++){
        d = new TDataSet(MasterCB->Text,i_chan,ParameterCB->Text.ToIntDef(-1),NDimCB->Text.ToIntDef(1),i_chan,XidxCB->Text.ToIntDef(0));
        //ParametersLB->Items->Add(MasterCB->Text + " Ch" + ChannelCB->Text + " " + ParameterCB->Text);
        ParametersLB->Items->AddObject(MasterCB->Text + " Ch" + AnsiString(i_chan) + "(" + AnsiString(d->ADDR) + ")" +
        " " + ParameterCB->Text + "(" + AnsiString(d->X) + "," + AnsiString(d->D) + ")", (TObject*)d);

        EDT->PL->Add((TObject*)d);

        EDT->np++;
        //MainForm->Chart1->AddSeries()
        FromPickerChange(Sender);
        ToPickerChange(Sender);
        //delete d; //было закоментировано
    }
}
//---------------------------------------------------------------------------



void __fastcall TEDForm::CheckBox2Click(TObject *Sender)
{
    XidxCB->Text = "0";
    XidxCB->Enabled = !CheckBox2->Checked;
    NDimCB->Enabled = !CheckBox2->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TEDForm::Button5Click(TObject *Sender){
    for (int i_cc = 1; i_cc<=12; i_cc++){
        ChannelCB->Text = i_cc;
        AddressCB->Text = i_cc;
        AddButtonClick(Sender);   
    }
}
//---------------------------------------------------------------------------

