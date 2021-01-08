//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TYRange : public TForm
{
__published:	// IDE-managed Components
    TEdit *Edit1;
    TEdit *Edit2;
    TLabel *Label1;
    TLabel *Label2;
    TListBox *LegendLB;
    TLabel *Label3;
    TButton *Button1;
    void __fastcall Edit1Change(TObject *Sender);
    void __fastcall Edit2Change(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall LegendLBClick(TObject *Sender);
    void __fastcall LegendLBDrawItem(TWinControl *Control, int Index,
          TRect &Rect, TOwnerDrawState State);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TYRange(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TYRange *YRange;
//---------------------------------------------------------------------------
#endif
