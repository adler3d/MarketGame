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
#include "QapSys.h"
#include "QapInput.h"
#include "QapIO.h"
#include "QapClock.h"
#include "QapTools.h"
#include "QapAssert.h"
/************************************************************************/
/* Объявления                                                           */
/************************************************************************/
LRESULT CALLBACK	WndProc(HWND,UINT,WPARAM,LPARAM);
/************************************************************************/
/* Переменные                                                           */
/************************************************************************/
#define WndClass "T"ENG_NAME"Form"
extern QapSys Sys;
/************************************************************************/
/* Реализация                                                           */
/************************************************************************/
void QapSys::SetProc(ProcType ID,void* Proc)
{
  switch(ID)
  {
  case PROC_RENDER: ProcRender=(TProcRender)Proc; break;
  case PROC_UPDATE: ProcUpdate=(TProcUpdate)Proc; break;
  case PROC_INIT: ProcInit=(TProcInit)Proc; break;
  case PROC_FREE: ProcFree=(TProcFree)Proc; break;
  }
}

ATOM QapSys::MyRegisterClass()
{
  WNDCLASSEX wcx;
  ZeroMemory(&wcx,sizeof(wcx));
  wcx.cbSize    = sizeof(wcx);
  wcx.hInstance	= hInst;
  wcx.hIcon			= LoadIcon(0, IDI_ASTERISK);
  wcx.hIconSm		= wcx.hIcon;
  wcx.hCursor		= LoadCursor(NULL, IDC_ARROW);
  wcx.hbrBackground	= NULL;//(HBRUSH)GetStockObject(BLACK_BRUSH);
  wcx.style			= NULL;//CS_HREDRAW | CS_VREDRAW; 
  wcx.lpfnWndProc	= WndProc;
  wcx.lpszClassName	= WndClass;	
  return RegisterClassExA(&wcx);
}

bool QapSys::InitInstance(string Caption)
{
  if(AppName.empty())AppName=Caption;
  hWnd = CreateWindowA(WndClass, Caption.c_str(), WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, hInst, NULL);
  if (!hWnd){return false;}
  //ShowWindow(hWnd, CmdShow);
  //UpdateWindow(hWnd);
  return true;
}
//static int PauseCounter=0;static int PrevPauseCounter=0; static bool paused=false; // - костыль спецально для паузы.
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  #define SETKEY(KEY,VALUE)QapInput::SysKeyUpdate(KEY,VALUE);//Down[KEY]=VALUE;
  //MACRO_ADD_LOG("msg = "+IToS(msg),lml_HINT);
  if(msg==WM_SYSKEYDOWN)
  {
    if(wParam==VK_F10)SETKEY(VK_F10,true);
    if(wParam==VK_MENU)
      SETKEY(VK_MENU,true);
  }
  if(msg==WM_SYSKEYUP)
  {
    if(wParam==VK_MENU)
      SETKEY(VK_MENU,false);
    if(wParam==VK_F10)SETKEY(VK_F10,false);
    if(wParam==VK_RETURN)
    {
      Sys.WindowMode(Sys.wnd_mode,Sys.SM.W,Sys.SM.H,Sys.SM.BPP,Sys.SM.Freq);
    }  
  }
  if(msg==WM_CLOSE){/*MACRO_RAISE_ERROR("WM_CLOSE");*/DestroyWindow(hWnd);};
  if(msg==WM_DESTROY){Sys.Quit(); /*PostQuitMessage(0);*/};  
  if(msg==WM_SIZE){/*
    RECT Rect;
    //GetWindowRgnBox(hWnd,&rect);
    //WINDOWPLACEMENT pl;GetWindowPlacement(hWnd,&pl);rect=pl.rcNormalPosition;
    //D3DVIEWPORT9 VP;
    //Sys.pDev->setp
    GetClientRect(hWnd,&Rect);
    Sys.SM.H=Rect.bottom-Rect.top;
    Sys.SM.W=Rect.right-Rect.left;//GetScreenMode();
    D3DVIEWPORT9 VP;
    if(Sys.pDev)Sys.pDev->GetViewport(&VP);
    VP.Height=Sys.SM.W;
    VP.Width=Sys.SM.H;
    VP.MaxZ=+1.0;
    VP.MinZ=-1.0;
    VP.X=Rect.left;
    VP.Y=Rect.right;
    if(Sys.pDev)Sys.pDev->SetViewport(&VP);*/
  };
  if(msg==WM_ACTIVATEAPP)
  {
    if(LOWORD(wParam)==WA_ACTIVE)
    {
      Sys.wnd_active=true;
      if(!Sys.wnd_mode)
      {
        Sys.Show(false);
        //Sys.Restore();
        //Sys.WindowMode(true,Sys.SM.W,Sys.SM.H,Sys.SM.BPP,Sys.SM.Freq);
        //if(Sys.OnlyActive)Sys.ResetTimer();
      }else{
        Sys.wnd_active=false;
        if(Sys.wnd_mode)
        {
          Sys.Show(true);
          //Sys.Restore();
          //Sys.WindowMode(false,Sys.SM.W,Sys.SM.H,Sys.SM.BPP,Sys.SM.Freq);
          Sys.wnd_mode=false;
        }
      }
    }
  }
  if((msg==WM_KEYUP)&&(wParam==VK_SNAPSHOT)){SETKEY(VK_SNAPSHOT,true);SETKEY(VK_SNAPSHOT,false);}
  if(msg==WM_MOUSEWHEEL){QapInput::zDelta=GET_WHEEL_DELTA_WPARAM(wParam);};
  if(msg==WM_KEYUP){SETKEY(wParam,false);}
  if(msg==WM_KEYDOWN){SETKEY(wParam,true);}
  if(msg==WM_LBUTTONUP){SETKEY(mbLeft,false);}
  if(msg==WM_RBUTTONUP){SETKEY(mbRight,false);}
  if(msg==WM_MBUTTONUP){SETKEY(mbMiddle,false);}
  if(msg==WM_LBUTTONDOWN){SETKEY(mbLeft,true);}
  if(msg==WM_RBUTTONDOWN){SETKEY(mbRight,true);}
  if(msg==WM_MBUTTONDOWN){SETKEY(mbMiddle,true);}
  if(msg==WM_CHAR){QapInput::SysCharUpdate(wParam);}
  return DefWindowProc(hWnd,msg,wParam,lParam);
  #undef SETKEY
}

void QapSys::WindowCreate(string Caption)
{
  if(!MyRegisterClass())MACRO_ADD_LOG("unable to register window class",lml_FATAL_ERROR);
  if(!InitInstance(Caption))MACRO_ADD_LOG("fails to create a window",lml_FATAL_ERROR);
}

bool QapSys::WindowMode(bool FullScreen,int W,int H,int BPP,int Freq)
{
  TScreenMode NM={W,H,BPP,Freq};
  string X=FullScreen?"FullScreen(":"NoFullScreen(";
  MACRO_ADD_LOG(X+IToS(W)+"x"+IToS(H)+" "+IToS(BPP)+"bpp "+IToS(Freq)+"hz)",lml_SYSTEM);
  wnd_mode=!FullScreen; SM=NM; Restore(); return true;
}

void QapSys::Show(bool Minimized)
{
  MACRO_ADD_LOG(Minimized?"Minimized":"Normalized",lml_EVENT);
  if(Sys.wnd_noqap)ShowWindow(Sys.hWnd,Minimized?SW_SHOWMINIMIZED:SW_SHOWNORMAL);
  ResetClock();
}

void QapSys::Restore()
{
  MACRO_ADD_LOG("Restore",lml_EVENT);
  if(!wnd_noqap)
  {
    DWORD Style=(wnd_mode)?WS_OVERLAPPEDWINDOW:WS_OVERLAPPED;
    SetWindowLong(hWnd,GWL_STYLE,Style|WS_VISIBLE);
    RECT Rect={0,0,SM.W,SM.H};
    AdjustWindowRect(&Rect,Style,false);
    SetWindowPos(hWnd,0,0,0,Rect.right-Rect.left,Rect.bottom-Rect.top,SWP_FRAMECHANGED|SWP_NOOWNERZORDER);
    ShowWindow(hWnd,SW_SHOW);
  }
}

namespace QapSysHideSpace{
  bool FlagStackOverFlow=false;
  inline int filter(unsigned int code, struct _EXCEPTION_POINTERS *ep){
    #define FINISH(){return EXCEPTION_EXECUTE_HANDLER;}
    #define F(X)if(code==EXCEPTION_##X){MACRO_ADD_LOG("caught "#X" as expected",lml_FATAL_ERROR);FINISH();}
    {
      if(code==EXCEPTION_STACK_OVERFLOW){FlagStackOverFlow=true;FINISH();};
      F(ACCESS_VIOLATION);
      F(DATATYPE_MISALIGNMENT);
      F(BREAKPOINT);
      F(SINGLE_STEP);
      F(ARRAY_BOUNDS_EXCEEDED);
      F(FLT_DENORMAL_OPERAND);
      F(FLT_DIVIDE_BY_ZERO);
      F(FLT_INEXACT_RESULT);
      F(FLT_INVALID_OPERATION);
      F(FLT_OVERFLOW);
      F(FLT_STACK_CHECK);
      F(FLT_UNDERFLOW);
      F(INT_DIVIDE_BY_ZERO);
      F(INT_OVERFLOW);
      F(PRIV_INSTRUCTION);
      F(IN_PAGE_ERROR);
      F(ILLEGAL_INSTRUCTION);
      F(NONCONTINUABLE_EXCEPTION);
      //F(STACK_OVERFLOW);
      F(INVALID_DISPOSITION);
      F(GUARD_PAGE);
      F(INVALID_HANDLE);
      //F(EXCEPTION_POSSIBLE_DEADLOCK);
    }
    #undef F
    #undef FINISH
    return EXCEPTION_CONTINUE_SEARCH;
  }
  void SafeLog(string &ProcName)
  {
    MACRO_ADD_LOG("Error in "+ProcName,lml_ERROR);
  };
  inline void DoAfterStackOverFlow()
  {
    FlagStackOverFlow=false;
    MACRO_ADD_LOG("caught STACK_OVERFLOW as expected",lml_FATAL_ERROR);  
  };
  bool SafeCall(TProc Proc,string &ProcName)
  {
#if(1)
    if(Proc)Proc();
#else
    FlagStackOverFlow=false;
    __try{
      if(Proc)Proc();
    }__except(filter(GetExceptionCode(),GetExceptionInformation())){
      if(FlagStackOverFlow){_resetstkoflw();DoAfterStackOverFlow();};
      SafeLog(ProcName);
      return false;
    }
#endif
    return true;
  }
}
#define TrySafeCall(PROCNAME)if(!QapSysHideSpace::SafeCall(PROCNAME,SToS(#PROCNAME)))return;
void QapSys::ResetClock(){ResetFlag=true;};
void QapSys::Run(int UPS)
{
  this->UPS=UPS;
  MSG msg; SysClock.Start();
  IsQuit=false;
  TrySafeCall(ProcInit);
  MACRO_ADD_LOG("Main Loop start",lml_SYSTEM);
  real dt=real(1000.0)/real(UPS),t=0;LoopClock.Start();
  int WarningCounter=0;bool new_state=true;
  while(!IsQuit)
  {  
    LoopClock.Stop();t+=LoopClock.MS();LoopClock.Start();
    QapClock UpdateClock,RenderClock;
    if(UPS_enabled){
      for(;(t>0)&&!IsQuit&&UPS_enabled;t-=dt)
      {
        UpdateClock.Start();  
        QapInput::Sync();
        while(PeekMessage(&msg,hWnd,0,0,PM_REMOVE)){TranslateMessage(&msg);DispatchMessage(&msg);}
        //if(paused)PrevPauseCounter=PauseCounter;
        //if(!paused&&(PauseCounter-PrevPauseCounter>1)){exit(0);}  // - костыль для проверки слишком быстрого нажатия паузы
        //PrevPauseCounter=PauseCounter;
        TrySafeCall(ProcUpdate);new_state=true;
        UpdateClock.Stop();
        if(ResetFlag){ResetFlag=false;t=0;break;};
        if(t/dt>real(UPS))if(UpdateClock.MS()*UPS>1000.0){WarningCounter++;MACRO_ADD_LOG("("+FToS(t)+"ms)Too much load on the CPU",lml_WARNING);t=0;break;}
      }
      {
        RenderClock.Start();
        if(new_state){TrySafeCall(ProcRender);}new_state=false;
        RenderClock.Stop();
        if(RenderClock.MS()>500.0){WarningCounter++;MACRO_ADD_LOG("("+FToS(RenderClock.MS())+"ms)Too much load on the GPU",lml_WARNING);}
      }
      if(WarningCounter>64){MACRO_ADD_LOG("Too weak system",lml_ERROR);break;}
    }else{
      QapInput::Sync();
      while(PeekMessage(&msg,hWnd,0,0,PM_REMOVE)){TranslateMessage(&msg);DispatchMessage(&msg);}
      TrySafeCall(ProcUpdate);
      TrySafeCall(ProcRender);
    }
  }
  MACRO_ADD_LOG("Main Loop stop",lml_SYSTEM);
  TrySafeCall(ProcFree);
  if(pDev)
    pDev->Release();
  if(pD3D)
    pD3D->Release();
  MACRO_ADD_LOG("WorkTime : "+FToS(SysClock.Time())+" sec",lml_SYSTEM);
}
#undef TrySafeCall
D3DPRESENT_PARAMETERS QapSys::GetPresentParameters()
{
  D3DDISPLAYMODE dm;
  pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);
  D3DPRESENT_PARAMETERS pp; ZeroMemory(&pp, sizeof(pp));
  //pp.BackBufferWidth=max(2048,SM.W);//SM.W;
  //pp.BackBufferHeight=max(2048,SM.H);//SM.H;
  pp.BackBufferWidth=SM.W;
  pp.BackBufferHeight=SM.H;
  pp.BackBufferFormat=D3DFMT_X8R8G8B8;//dm.Format;
  pp.BackBufferCount=1;
  pp.SwapEffect= D3DSWAPEFFECT_COPY;
  pp.PresentationInterval=VSync?D3DPRESENT_INTERVAL_ONE:D3DPRESENT_INTERVAL_IMMEDIATE;//D3DPRESENT_INTERVAL_ONE; //D3DPRESENT_INTERVAL_IMMEDIATE;
  pp.hDeviceWindow=hWnd;
  pp.Windowed=true;//Debug||wnd_mode;
  if(!(Debug||wnd_mode))pp.FullScreen_RefreshRateInHz=SM.Freq;
  return pp;
}

bool QapSys::InitD3D(bool VSync)
{
  pD3D=Direct3DCreate9(D3D_SDK_VERSION);
  if(!pD3D) {
    MACRO_ADD_LOG("Cannot create Direct3D",lml_FATAL_ERROR);
    return false;
  }
  {
    D3DADAPTER_IDENTIFIER9 pIdent;
    pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT,0,&pIdent);
    MACRO_ADD_LOG("Adapter Driver : "+SToS(pIdent.Driver),lml_SYSTEM);
    MACRO_ADD_LOG("Adapter Description : "+SToS(pIdent.Description),lml_SYSTEM);
    MACRO_ADD_LOG("Adapter VendorId : "+HToS(pIdent.VendorId),lml_SYSTEM);
    MACRO_ADD_LOG("Adapter DeviceId : "+HToS(pIdent.DeviceId),lml_SYSTEM);
    //MACRO_ADD_LOG("Adapter SubSysId : "+HToS(pIdent.SubSysId),lml_SYSTEM);
    //MACRO_ADD_LOG("Adapter Revision : "+HToS(pIdent.Revision),lml_SYSTEM);
  }
  this->VSync=VSync;
  D3DPRESENT_PARAMETERS pp=GetPresentParameters();
  pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&pp,&pDev);
  if(!pDev){
    MACRO_ADD_LOG("Cannot create device",lml_FATAL_ERROR);
    return false;
  }
  return true;
}

void QapSys::Quit()
{
  if(IsQuit)
  {
    MACRO_ADD_LOG("Retry exit",lml_WARNING);
  }else{
    MACRO_ADD_LOG("An attempt to exit",lml_EVENT);
  }
  IsQuit=true; 
}; 
//Sys.LogEx(SToS(GetFileName(__FILE__)+"("+IToS(__LINE__)+")")+" => "+SToSex(__FUNCTION__,25)+" : \""+(MSG)+"\"");
void QapSys::LogEx(string sn,int ln,string fn,string msg,LogMsgLevel lvl)
{
  static string LS[]={"HINT","WARNING","SYSTEM","ERROR","FATAL_ERROR"};
  static bool l_flags[]={1,1,1,1,1};
  #define X(A,B)std::make_pair(A,bool(B))
  std::pair<string,bool> log_states[]={
    X(LS[lvl]+":",true),
    X(" "+GetFileName(sn),false),
    X(+"("+IToS(ln)+")",false),
    X(" "+fn,true),
    X(" => "+msg,true)};
  #undef X
  string message="";
  for(int i=0;i<5;i++)if(log_states[i].second)message+=log_states[i].first;
  Log(message);
}
void QapSys::Log(string msg)
{
  LogClock.Stop();
  (*pLog)<<"["<<IToSex((int)LogClock.MS())<<"] "<<msg<<std::endl;
  LogClock.Start();
};

QapSys::QapSys(string LogFileName)
{
  pLog=new std::ofstream(LogFileName.c_str());
  LogClock.Start(); 
  Log("\"ENG_NAME log start");
}
QapSys::~QapSys()
{
  Log("\"ENG_NAME log close");
  pLog->close();
  delete pLog;
}
/************************************************************************/
/* Конец                                                                */
/************************************************************************/