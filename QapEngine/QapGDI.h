#pragma once
//-------------------------------------------//
//   {<| author : Adler3D(Kashin Ivan) |>}   //
//   {<| e-mail : Adler3D@gmail.com    |>}   //
//   {<| site   : Adler3d.narod.ru     |>}   //
//-------------------------------------------//
#include "QapType.h"
#include "QapSys.h"
#include "QapVec.h"
//-------------------------------------------//
namespace QapGDI
{
  inline void PrintText(const HDC&dc,const int&x,const int&y,const string&s){const char* c=s.c_str();TextOutA(dc,x,y,c,int(strlen(c)));}
  inline void TextDraw(const HDC&dc,int x,int y,const string&s)
  {
    const COLORREF CT[10]={0x252525,0x0000FF,0x00FF00,0x00FFFF,0xFF0000,0xFF00FF,0xFFFF00,0xFFFFFF,0xA8FFFF,0xFFA8FF};
    int i=0; string t;  SIZE temp;
    //SetBkColor(dc,0x000000);
    SetBkMode(dc,TRANSPARENT);
    while(i<(int)s.length()){
      if(s[i]=='^'){
        if(!t.empty()){
          PrintText(dc,x,y,t); const char *c=t.c_str(); GetTextExtentPoint32A(dc,c,(int)t.length(),&temp);     
          x+=temp.cx; t.clear();}   
        if(i<=(int)s.length()){i++;if((s[i]>='0')&&(s[i]<='9'))SetTextColor(dc,CT[s[i]-'0']);};
      }else t+=s[i];
      i++;}
    PrintText(dc,x,y,t);
    SetBkMode(dc,BKMODE_LAST);
  }
}
//-------------------------------------------//
//   {<|          03.06.2011           |>}   //
//-------------------------------------------//