//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("mainUnit.cpp", MainForm);
USEFORM("EDUnit.cpp", EDForm);
USEFORM("CfgUnit.cpp", CfgForm);
USEFORM("Unit2.cpp", YRange);
USEFORM("PowerUnit.cpp", PowerForm);
USEFORM("SettingsUnit.cpp", SettingsForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->CreateForm(__classid(TMainForm), &MainForm);
                 Application->CreateForm(__classid(TEDForm), &EDForm);
                 Application->CreateForm(__classid(TCfgForm), &CfgForm);
                 Application->CreateForm(__classid(TYRange), &YRange);
                 Application->CreateForm(__classid(TSettingsForm), &SettingsForm);
                 Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    catch (...)
    {
         try
         {
             throw Exception("");
         }
         catch (Exception &exception)
         {
             Application->ShowException(&exception);
         }
    }
    return 0;
}
//---------------------------------------------------------------------------
