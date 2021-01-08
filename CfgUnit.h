//---------------------------------------------------------------------------

#ifndef CfgUnitH
#define CfgUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <CheckLst.hpp>
//---------------------------------------------------------------------------
class TCfgForm : public TForm
{
__published:	// IDE-managed Components
    TCheckListBox *CheckListBox1;
    TCheckListBox *CheckListBox2;
    TCheckListBox *CheckListBox3;
    TCheckBox *CheckBox1;
    TCheckBox *CheckBox2;
    TCheckBox *CheckBox3;
    TCheckListBox *CheckListBox4;
    TCheckBox *CheckBox4;
    TCheckListBox *CheckListBox5;
    TCheckBox *CheckBox5;
    TCheckListBox *CheckListBox6;
    TCheckBox *CheckBox6;
    TCheckListBox *CheckListBox7;
    TCheckBox *CheckBox7;
    TCheckListBox *CheckListBox8;
    TCheckBox *CheckBox8;
    TCheckListBox *CheckListBox9;
    TCheckBox *CheckBox9;
    TCheckListBox *CheckListBox10;
    TCheckBox *CheckBox10;
    TButton *Button1;
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TCfgForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TCfgForm *CfgForm;
//---------------------------------------------------------------------------
#endif
