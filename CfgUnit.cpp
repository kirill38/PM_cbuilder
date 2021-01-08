//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CfgUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TCfgForm *CfgForm;
//---------------------------------------------------------------------------
__fastcall TCfgForm::TCfgForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TCfgForm::Button1Click(TObject *Sender)
{
    TRect R = CheckListBox1->ItemRect(0);
    R.right = CheckListBox1->ItemRect(0).Width()/2;
    CheckListBox1->Canvas->FillRect(R);
    CheckListBox1->ItemRect(0);
}
//---------------------------------------------------------------------------
