/*------------------------------------------*/
/*   {<|author : Adler3D(Kashin Ivan) |>   }*/
/*   {<|e-mail : Adler3D@gmail.com    |>   }*/
/*   {<|site   : Adler3d.narod.ru     |>   }*/
/*------------------------------------------*/
#pragma warning(disable:4068)
/************************************************************************/
/* Подключаем всякое разное                                             */
/************************************************************************/
#include "StdAfx.h"
#include "QapInput.h"
/************************************************************************/
/* Объявления                                                           */
/************************************************************************/

/************************************************************************/
/* Реализация                                                           */
/************************************************************************/
namespace QapInput
{
  vec2d MousePos;
  QapDX::QapTex*MouseTex;
  int zDelta;
  const int MAX_KEY=263;
  char LastChar;
  bool Down[MAX_KEY],Changed[MAX_KEY],News;
  int LastKey;
  void InitMouse(){
    //string t2="Cursor.tga";
    //MouseTex=QapDX::LoadTexture(t2);
    memset(&Down,0,sizeof(Down));
  };/*
  void RenderMouse()
  {
    QapDX::Blend(QapDX::BT_SUB);
    QapDX::SetColor(D3DCOLOR_XRGB(255,255,255));
    Sys.pDev->SetTexture(0,MouseTex);
    const real sd2=16.0f;
    QapDX::DrawQuad(MousePos.x+sd2,MousePos.y+sd2,sd2*2,sd2*2,0);
    Sys.pDev->SetTexture(0,0);
  };*/
  void UpdateMouse()
  {
    static POINT MP;
    GetCursorPos(&MP);ScreenToClient(Sys.hWnd,&MP);
    MousePos=vec2d(MP.x-Sys.SM.W/2,-MP.y+Sys.SM.H/2);
    //zDelta=0;
  };
  vec2d GetMousePos(){return MousePos;};
  void SysKeyUpdate(int Key,bool Value){
    if(Value)LastKey=Key;
    if(!InDip(0,Key,MAX_KEY-1))return;
    Down[Key]=Value;Changed[Key]=true;
  };
  void SysCharUpdate(char c){LastChar=c;News=true;};
  void Sync()
  {
    QapInput::News=false;QapInput::zDelta=0;
    for(int i=0;i<MAX_KEY;i++)Changed[i]=false;
  }
}
/************************************************************************/
/* Конец                                                                */
/************************************************************************/
