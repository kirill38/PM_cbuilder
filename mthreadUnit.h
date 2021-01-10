//---------------------------------------------------------------------------
#ifndef mthreadUnitH
#define mthreadUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "masterUnit.h"
//---------------------------------------------------------------------------
class TMThread : public TThread{
private:
    AnsiString LogString;
protected:
    void __fastcall Execute(void);
public:
    bool SF;        //Stop flag
    bool EI;        //External Input flag
    bool Ended;     //End Thread Flag;
    TMaster *FMaster;

    __fastcall TMThread(TMaster *OMaster);
    __fastcall ~TMThread(void);

    bool __fastcall StopCheckPoint(void);
    void __fastcall AddLogString(AnsiString astr);
    void __fastcall VALS(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TMThread *MThread;
//---------------------------------------------------------------------------
#endif
