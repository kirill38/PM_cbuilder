//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
#include "mainUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TYRange *YRange;
//---------------------------------------------------------------------------
__fastcall TYRange::TYRange(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TYRange::Edit1Change(TObject *Sender)
{
    try{
        MainForm->Chart1->LeftAxis->Minimum = Edit1->Text.ToDouble();
        MainForm->Chart1->LeftAxis->AutomaticMinimum = false;
        Edit1->Color = clWindow;
    }catch(Exception &e){
        MainForm->LogMemo->Lines->Add("Exc. : " + e.Message + " err double convert");
        MainForm->Chart1->LeftAxis->AutomaticMinimum = true;
        Edit1->Color = clRed;
    }
}
//---------------------------------------------------------------------------
void __fastcall TYRange::Edit2Change(TObject *Sender)
{
    try{
        MainForm->Chart1->LeftAxis->Maximum = Edit2->Text.ToDouble();
        MainForm->Chart1->LeftAxis->AutomaticMaximum = false;
        Edit2->Color = clWindow;
    }catch(Exception &e){
        MainForm->LogMemo->Lines->Add("Exc. : " + e.Message + " err double convert");
        MainForm->Chart1->LeftAxis->AutomaticMaximum = true;
        Edit2->Color = clRed;
    }
}
//---------------------------------------------------------------------------
void __fastcall TYRange::FormCreate(TObject *Sender)
{
    Edit1->Text = MainForm->Chart1->LeftAxis->Minimum;
    Edit2->Text = MainForm->Chart1->LeftAxis->Maximum;
}
//---------------------------------------------------------------------------
void __fastcall TYRange::LegendLBClick(TObject *Sender)
{
    //LegendLB->ItemRect(LegendLB->ItemIndex) = LegendLB->ItemRect(LegendLB->ItemIndex+1);
}
//---------------------------------------------------------------------------

void __fastcall TYRange::LegendLBDrawItem(TWinControl *Control, int Index,
      TRect &Rect, TOwnerDrawState State)
{
    LegendLB->Canvas->Font->Color = clLime;
    LegendLB->Canvas->Brush->Color = clBlack;
    LegendLB->Canvas->FillRect(Rect);
    LegendLB->Canvas->TextOut(Rect.Left, Rect.Top,
       LegendLB->Items->Strings[Index]);
}
//---------------------------------------------------------------------------

void __fastcall TYRange::Button1Click(TObject *Sender)
{
    //LegendLB->Canvas->Font->Color = clRed;
    LegendLB->Items->Add("123");
}
//---------------------------------------------------------------------------

void __fastcall TYRange::FormShow(TObject *Sender)
{
    //LegendLB-    
}
//---------------------------------------------------------------------------

