//---------------------------------------------------------------------------

#ifndef EDUnitH
#define EDUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <fstream.h>
#include <vector.h>
#include "clsUnit.h"
//---------------------------------------------------------------------------
struct XY {
public:
    double X;
    double Y;
};

class TDataSet {
public:
    AnsiString M; //address of master
    unsigned long recs_count;
    unsigned long recs_parsed;
    bool count_cum;
    int n_pf; //n per file
    int p_n;  //1 ...p... 1  skip p some records between n
    int DT;
    int CH;
    int ADDR;
    int P;
    int D;
    int X;
    unsigned short IP[4];
    __fastcall TDataSet(AnsiString iM, int iCH, int iP);
    __fastcall TDataSet(AnsiString iM, int iCH, int iP, int iD);
    __fastcall TDataSet(AnsiString iM, int iCH, int iP, int iD, int iADDR, int iX);
    __fastcall TDataSet(AnsiString iM, int iCH, int iP, int iD, int iADDR, int iX, int iDT, bool iCount);
    __fastcall TDataSet::TDataSet(AnsiString iM, int iCH, int iP, int iD, int iADDR, int iX, int iDT, bool iCount, int N, int P);
    __fastcall ~TDataSet(void);
    //std::vector<XY> V;
    std::vector<double*> VV;
};

class TEDForm : public TForm
{
__published:	// IDE-managed Components
    TDateTimePicker *FromPicker;
    TDateTimePicker *ToPicker;
    TLabel *Label1;
    TLabel *Label2;
    TComboBox *MasterCB;
    TComboBox *ChannelCB;
    TComboBox *ParameterCB;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TListBox *ParametersLB;
    TButton *AddButton;
    TCheckBox *CheckBox1;
    TButton *DrawButton;
    TButton *SaveButton;
    TButton *CloseButton;
    TButton *ReadDataButton;
    TButton *Button1;
    TDateTimePicker *DateTimePicker1;
    TDateTimePicker *DateTimePicker2;
    TProgressBar *ProgressBar1;
    TButton *Button2;
    TComboBox *NDimCB;
    TLabel *NDimLabel;
    TButton *Button3;
    TButton *Button4;
    TButton *Button5;
    TComboBox *AddressCB;
    TComboBox *XidxCB;
    TComboBox *DataTypeCB;
    TLabel *Label6;
    TCheckBox *CheckBox2;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall MasterCBChange(TObject *Sender);
    void __fastcall ChannelCBChange(TObject *Sender);
    void __fastcall AddButtonClick(TObject *Sender);
    void __fastcall ParametersLBDblClick(TObject *Sender);
    void __fastcall CloseButtonClick(TObject *Sender);
    //void __fastcall ReadDataButtonClick(TObject *Sender);
    //void __fastcall DisplayFiles(AnsiString path);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall DrawButtonClick(TObject *Sender);
    void __fastcall DateTimePicker1Change(TObject *Sender);
    void __fastcall DateTimePicker2Change(TObject *Sender);
    void __fastcall FromPickerChange(TObject *Sender);
    void __fastcall ToPickerChange(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall Button4Click(TObject *Sender);
    void __fastcall CheckBox2Click(TObject *Sender);
    void __fastcall Button5Click(TObject *Sender);

private:	// User declarations

public:		// User declarations
    //TStringList *FileRange;
    //TList *PL; // Parameters List to read
    __fastcall TEDForm(TComponent* Owner);
    //void __fastcall ParseParameter(TDateTime iDT, int iP, AnsiString iS, std::vector<XY> &oV);
    //void __fastcall ParseParameter(TDateTime iDT, int iP, AnsiString iS, std::vector<double*> &oV);
    //std::ifstream file2read;
    //std::vector<double> v;
    //TDateTime t1,t2;
    //int np;
};
//---------------------------------------------------------------------------
extern PACKAGE TEDForm *EDForm;
//---------------------------------------------------------------------------
class TEDThread : public TThread{
private:
protected:
    void __fastcall Execute();
public:
    TEDForm *Form;
    __fastcall TEDThread(bool CreateSuspended);
    __fastcall ~TEDThread(void);
    void __fastcall DisplayFiles(AnsiString path);
    void __fastcall ReadData(void);
    void __fastcall ParseParameter(TDateTime iDT, int iP, AnsiString iS, std::vector<XY> &oV);
    int __fastcall ParseParameter(TDateTime iDT, int iP, AnsiString iS, long count, std::vector<double*> &oV);

    std::ifstream file2read;
    TList *PL;
    TStringList *FileRange;
    TDateTime t1,t2;
    int dtype;
    int np;
};
//---------------------------------------------------------------------------
extern PACKAGE TEDThread *EDT;
//---------------------------------------------------------------------------
#endif
