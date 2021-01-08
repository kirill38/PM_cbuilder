//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SettingsUnit.h"
#include "clsUnit.h"
#include "mainUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSettingsForm *SettingsForm;
TData *D;
//---------------------------------------------------------------------------
__fastcall TSettingsForm::TSettingsForm(TComponent* Owner)
        : TForm(Owner){
}
//---------------------------------------------------------------------------
void __fastcall TSettingsForm::Edit1Change(TObject *Sender){
        Data->allHV = Edit1->Text.ToIntDef(0);
        if(Data->allHV<0 || Data->allHV>1100)Data->allHV=0;
        Label1->Caption = AnsiString("HV=") + AnsiString(Data->allHV) + AnsiString("V");
}
//---------------------------------------------------------------------------
void __fastcall TSettingsForm::CheckBox1Click(TObject *Sender){
        Data->doSetHV = CheckBox1->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TSettingsForm::FormShow(TObject *Sender){
        CheckBox1->Checked = Data->doSetHV;
        Edit1->Text = Data->allHV;
        Label1->Caption = AnsiString("HV=") + AnsiString(Data->allHV) + AnsiString("V");
}
//---------------------------------------------------------------------------
void __fastcall TSettingsForm::FormClose(TObject *Sender,
      TCloseAction &Action){
        if(Data->doSetHV)MainForm->MainMenu1->Items->Items[2]->Caption = "Set HV (!)";
        else MainForm->MainMenu1->Items->Items[2]->Caption = "Set HV";
}
//---------------------------------------------------------------------------
