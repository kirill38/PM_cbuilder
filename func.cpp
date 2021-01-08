
#include "func.h"

AnsiString StrToHexStr(AnsiString str){
    AnsiString hex_str;
    unsigned char ch;
        for (int i=1; i<=str.Length(); i++){
            ch=str[i];
            hex_str+=IntToHex(ch,2);
            if (i<str.Length())
            hex_str+=" ";
        }
    return hex_str;
}

unsigned int CheckInputStr(AnsiString str){
    if(str.Length()<6)return 0;
    if(str.Length()==str[5]*256+str[6])return str.Length();
    return 0;
}

