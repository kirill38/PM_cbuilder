//---------------------------------------------------------------------------
#ifndef clsUnitH
#define clsUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Series.hpp>
#include <IniFiles.hpp>
#include <IdTCPServer.hpp>
#include "mainUnit.h"
#include "srvUnit.h"
#include <fstream.h>
//---------------------------------------------------------------------------
struct TDFhdr {
    unsigned __int16 DataFileVersion;
    char rawdata[16];           //rawdata
    unsigned __int16 CRC;       //data file header crc16
};
//---------------------------------------------------------------------------
class TData{
private:
    unsigned int FCI;
    TStringList *LogListA;
    TStringList *LogListB;
    bool p2A;

protected:
public:
    bool busy;
    bool ff1;
    
    __fastcall TData(TIniFile *AIni);
    __fastcall ~TData(void);

    void __fastcall NextFile(void);

    std::ofstream datafile;
    std::ofstream lockfile;
    std::ofstream tempfile;
    //std::ofstream bufffile;
    std::ifstream datafile_to_read;

    unsigned __int16 file_index;
    unsigned __int16 MaxFiles;
    long int filesize;
    long int MaxFileSize;
    AnsiString filename;
    AnsiString lockfilename;
    AnsiString filepath;
    unsigned long int ofc,cfc;

    TDataHeader *dhdr;

    void __fastcall StoreData(TDateTime APCTime, AnsiString AMasterAddress, TChannel *AChannel, int AParameter, int AValueSize, AnsiString s);
    void __fastcall StoreMasterData(TDateTime APCTime, AnsiString AMasterAddress, int AParameter, int AValueSize, AnsiString s);

    unsigned long int error_count, SDBCounter, partial_read_count, inadequate, NSR;

    TMainForm *Form;
    TIniFile *Ini;
    AnsiString *ClientsPriority;
    int *ClientLastChannel;
    int *ParametersToRead;
    int AllListSize,ShortListSize;
    int NumOfServers,NumOfClients;
    int MaxURA;     //Max of Unsuccess Read Attempts
    int MaxESRA;    //Max of Empty String Read Attempts
    TList *tcpClients_list;
    TList *threads_list;
    TStringList *versions;
    TServer **srvrs;

    int **PArrays;
    int *PASizes;
    int *PAShortSizes;

    unsigned char df; //verbose level
    bool doSetBR;
    bool doSetPower;

    bool doSetHV;
    unsigned int allHV;

    bool syncByTimer;
    bool syncByLast;
    bool delay_tune;
    bool f3004; //TRUE if using flag system and 3004 port
    bool AutoStart;

    TTimer *SyncTimer;

    int *ExtSessionReceivedBytes;
    int *ExtSessionSentBytes;
    TDFhdr dfhdr;

    int NumOfVers;

    //TIMINGS
    unsigned int WaitForConnectionInterval; // 6000 ms
    unsigned int ParameterInterval; // 100 ms
    unsigned int WaitIfReadErrorInterval; // 2000 ms
    unsigned int WaitForResponseInterval; // 2000 ms
    unsigned int XLInterval; //10000 ms
    unsigned int SleepModeInterval; //10000 ms
    unsigned int PermissionRequestInterval;

    int PMCommand(AnsiString s, int Amdi, TIdPeerThread *AThread);
    AnsiString hosts_string, ports_string;

    __property unsigned int ChannelsInterval = { read=FCI, write=SetCI };
    void SetCI(unsigned int ACI);

    TStringList *pLogList;
    TStringList *gLogList;
    //TStringList *pgBuffer;
    //void SwapLog(void);
    void __fastcall AddLogString(AnsiString str);
    unsigned long int line_counter;
    //TVisThread *VisThread;

    TStringList *LogBuf;
};
//---------------------------------------------------------------------------
extern PACKAGE TData *Data;
//---------------------------------------------------------------------------
#endif
