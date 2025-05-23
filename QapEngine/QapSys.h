/*------------------------------------------*/
/*   {<|author : Adler3D(Kashin Ivan) |>   }*/
/*   {<|e-mail : Adler3D@gmail.com    |>   }*/
/*   {<|site   : Adler3d.narod.ru     |>   }*/
/*------------------------------------------*/
#pragma once
#define MACRO_ADD_LOG(MSG,LEVEL) \
  Sys.LogEx(__FILE__,__LINE__,__FUNCTION__,MSG,LEVEL);
#ifdef _DEBUG
  #define SET_DEBUG_VAR(VAR)VAR=true;
#else
  #define SET_DEBUG_VAR(VAR)VAR=false;
#endif
/************************************************************************/
/* Подключаем всякое разное                                             */
/************************************************************************/
#include "d3d9.h"
#include "QapType.h"
#include "QapTools.h"
#include "QapClock.h"
/************************************************************************/
/* Объявления Типов                                                     */
/************************************************************************/
enum ProcType 
{
  PROC_UPDATE,
  PROC_RENDER,
  PROC_MESSAGE,
  PROC_INIT,
  PROC_FREE,
};
enum LogMsgLevel
{
  lml_EVENT,
  lml_WARNING,
  lml_SYSTEM,
  lml_ERROR,
  lml_FATAL_ERROR,
  lml_FORCE_DWORD = 0x7FFFFFF
};
typedef void (*TProcUpdate)(void);
typedef void (*TProcRender)(void);
typedef void (*TProcInit)(void);
typedef void (*TProcFree)(void);
typedef void (*TProc)(void);
class QapSys;
/************************************************************************/
/* Переменные                                                           */
/************************************************************************/
#define ENG_NAME "QapEngine"
#define ENG_VER "0.03"
extern HINSTANCE hInst;
extern int CmdShow;
extern QapSys Sys;
/************************************************************************/
/* Классы                                                               */
/************************************************************************/
//#define SetTexture(A,B)SetTexture();
class QapSys
{
public:
  bool UPS_enabled=true;
private:
  TProcRender ProcRender;
  TProcUpdate ProcUpdate;
  TProcInit ProcInit;
  TProcFree ProcFree;  
private:
  std::ofstream *pLog;
  QapClock LogClock,SysClock,LoopClock;
protected:
  ATOM MyRegisterClass();
  bool InitInstance(string Caption);
public:
  void SetProc(ProcType ID,void* Proc);
  void WindowCreate(string Caption);
  bool WindowMode(bool FullScreen,int W,int H,int BPP,int Freq);
  D3DPRESENT_PARAMETERS GetPresentParameters();
  bool InitD3D(bool VSync=false);
  void Show(bool Minimized);
  void Restore();
  void Run(int UPS);
  void LogEx(string sn,int ln,string fn,string msg,LogMsgLevel lvl);
  void Log(string msg);
  void Quit();
  void ResetClock();
public:
  string AppName;
  HWND hWnd;
  bool Debug;
  bool wnd_noqap;
  bool wnd_active;
  bool wnd_mode;
  bool wnd_dx;
  bool OnlyActive;
  bool IsQuit;
  bool VSync;
  bool ResetFlag;
  int UPS;
  IDirect3D9*pD3D;
  IDirect3DDevice9*pDev;
  TScreenMode SM;
public:
  QapSys(string LogFileName);
  ~QapSys();
};
/************************************************************************/
/* Конец                                                                */
/************************************************************************/