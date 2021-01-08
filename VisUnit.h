//---------------------------------------------------------------------------

#ifndef VisUnitH
#define VisUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "mainUnit.h"
#include "clsUnit.h"
//---------------------------------------------------------------------------
class TVisThread : public TThread{
private:
    TMainForm *Form;
    void __fastcall AddToMemo(void);
    void __fastcall AddStringsToMemo(void);
    AnsiString str;
    AnsiString fname;
    void __fastcall AddLogString(AnsiString str);
    void __fastcall STF(void);
    void __fastcall ReFresh(void);
    TDateTime T;
protected:
    void __fastcall Execute();
public:
    __fastcall TVisThread(bool CreateSuspended);
    void __fastcall SaveToFile(AnsiString afname);
    __fastcall ~TVisThread(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TVisThread *VT;
//---------------------------------------------------------------------------
#endif
 