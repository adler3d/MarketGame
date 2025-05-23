/*------------------------------------------*/
/*   {<|author : Adler3D(Kashin Ivan) |>   }*/
/*   {<|e-mail : Adler3D@gmail.com    |>   }*/
/*   {<|site   : Adler3d.narod.ru     |>   }*/
/*------------------------------------------*/
#pragma warning(disable:4068)
/*------------------------------------------*/
#include "stdafx.h"
#include "Main.h"
#include <time.h>
#include <bitset>
#include "InetDownloader.hpp"
/*------------------------------------------*/
HINSTANCE hInst;
string CmdLine;
int CmdShow;
QapSys Sys("log.txt");
/*------------------------------------------*/
template<class TYPE>
inline void Clean(TYPE&Arr)
{
  int c=Arr.size();
  int j=-1;
  for(int i=0;i<c;i++)
  {
    if(!Arr[i].deaded&&i!=j)Arr[++j]=Arr[i];
  }
  Arr.resize(j+1);
}
/*------------------------------------------*/
#include "TextRender.hpp"
#include "QapAtlas.hpp"
#include "thirdparty/voronoi.h"
#include "thirdparty/sweepline/sweepline.hpp"
/*------------------------------------------*/
static bool file_put_contents(const string&FN,const string&mem){std::fstream f(FN,std::ios::out|std::ios::trunc);f<<mem;return true;}
static string file_get_contents(const string&fn){std::ifstream file(fn);return std::string((std::istreambuf_iterator<char>(file)),(std::istreambuf_iterator<char>()));}
/*------------------------------------------*/
class TGame{
public:
  typedef QapAtlas::TFrame TFrame;
public:
  class ILevel{
  public:
    virtual void Render(QapDev*RD)=0;
    virtual void Update(TGame*Game)=0;
    virtual bool Win()=0;
    virtual bool Fail()=0;
    virtual void AddText(TextRender*TR){}
    virtual ~ILevel(){}
  };
  class ILevelFactory{
  public:
    virtual ILevel*Build(TGame*Game)=0;
  };
  struct TLevelInfo{
    string Name;
    ILevelFactory&Factory;
    TLevelInfo(const string&Name,ILevelFactory&Factory):Name(Name),Factory(Factory){}
  };
  template<typename TYPE>
  class TLevelFactory:public ILevelFactory{
  public:
    virtual ILevel*Build(TGame*Game){
      auto*Result=new TYPE();
      Result->DoReset();
      Result->Init(Game);
      return Result;
    }
  };/*
  template<typename TYPE>
  class AutoPtr:public std::auto_ptr<TYPE>{
  public:
    TYPE*operator->(){return get();}
    operator bool(){return 
  };*/
public:
  #include "Entity.inl"
  #include "LevelPack.inl"
  #include "GameMenu.inl"
public:
  struct t_frame{
    TFrame*pF=0;
    TFrame*pS=0;
    string name,file,fn;
    int mode=0;
  };
public:
#define FRAMESCOPE(F)\
  F(Dot,"dot",2)\
  F(BigDot,"bigdot",2)\
  F(MenuItem,"MenuItem",0)\
  F(Market,"market",0)\
  F(Enemy,"enemy_v2",0)\
  F(Obstacle,"obstacle_v4",0)\
  //---
#define ADDFRAME(NAME,FILE,MODE)TFrame*Frame##NAME;TFrame*Frame##NAME##_s;t_frame frame_##NAME={0,0,#NAME,FILE,"",MODE};
  FRAMESCOPE(ADDFRAME)
#undef ADDFRAME
public:
#define PRO_VARIABLE()\
ADDVAR(TCounterIncEx,WaitWin,TCounterIncEx(0,0,Sys.UPS*2))\
ADDVAR(TCounterIncEx,WaitFail,TCounterIncEx(0,0,Sys.UPS*2))\
ADDVAR(TCounterInc,LevelCounter,TCounterInc(0,0,0))
//=====+>>>>>TGame
#include "GenVar.inl"
//<<<<<+=====TGame
public:
  vector<TLevelInfo>LevelsInfo;
  std::unique_ptr<ILevel>Level;
  std::unique_ptr<TMenu>Menu;
public:
  QapAtlas Atlas;
  QapDev RD;
  QapDX::QapFont NormFont;
  QapDX::QapFont BlurFont;
  QapDX::QapTex*th_rt_tex;
  QapDX::QapTex*th_rt_tex_full;
public:
  TGame(){DoReset();}
public:
  typedef QapDX::QapTexMem QapTexMem;
  QapTexMem*AddBorder(QapTexMem*pMem,int dHS=8,const QapColor&Color=0xffffffff)
  {
    int dS=dHS*2;
    QapDX::QapTexMem*sm=new QapDX::QapTexMem("ShadowBot",pMem->W+dS,pMem->H+dS,NULL);
    sm->pBits=new QapColor[sm->W*sm->H];
    sm->Clear(Color);
    sm->FillMem(dHS,dHS,pMem);
    //sm->FillBorder(dHS,dHS,pMem,dHS);
    return sm;
  }
  QapTexMem*GenShadow(QapTexMem*pMem,int dHS=8)
  {
    int dS=dHS*2;
    vec4f acc={0,0,0,0};
    for(int x:{0,1})for(int y:{0,1})acc+=pMem->get_color_at(x*(pMem->W-1),y*(pMem->H-1));
    acc*=0.25;
    QapColor c=acc.GetColor();
    auto*sm=AddBorder(pMem,dHS,c);
    sm->CalcAlpha(0xffffffff);
    sm->FillChannel(0x00ffffff,0x00ffffff);
    QapDX::BlurTexture(sm,dHS);
    return sm;
  }
  TFrame*GenShadowFrame(QapDX::QapTexMem*pMem,int dHS=8)
  {
    QapTexMem*sm=GenShadow(pMem,dHS);
    TFrame*FrameX=Atlas.AddFrame(sm);
    delete sm;
    return FrameX;
  }
  void LoadFrames(bool need_save_atlas=0,bool need_rewrite_tex=0)
  {
    //auto*ball=QapDX::LoadTexture("GFX\\Ball.png");
    //#define F(NAME)QapDX::LoadTexture("GFX\\"#NAME".png")->CopyAlpha(ball)->SaveToFile("GFX\\"#NAME".png");
    auto LT=QapDX::LoadTexture;
    auto m2=[&](QapTexMem*p){return p->CalcAlpha()->FillChannel(0xffffffff,0x00ffffff);};
    if(bool hack=false)
    {
      auto f=[&](auto NAME,string FILE,auto MODE){
        auto fn="GFX\\"+FILE+".png";
        if(MODE==2){auto*p=m2(LT(fn));if(need_rewrite_tex)p->SaveToFile(fn);}
        if(MODE==0){auto*p=LT(fn)->CalcAlpha(0xffffffff);if(need_rewrite_tex)p->SaveToFile(fn);}
        if(MODE==1){auto*p=LT(fn)->CopyAlpha(LT("GFX\\"+FILE+"_a.png")->CalcAlpha());if(need_rewrite_tex)p->SaveToFile(fn);}
      };
      #define F(NAME,FILE,MODE)f(Frame##NAME,FILE,MODE);
        FRAMESCOPE(F);
        //QapDX::LoadTexture("GFX\\You.png")->CopyAlpha(QapDX::GenBall(32))->SaveToFile("GFX\\You.png");
      #undef F
    }

    {
      #define F(NAME,FILE,MODE){\
        t_frame&f=frame_##NAME;f.fn="GFX\\"FILE".png";\
        QapDX::QapTexMem*tmp=LT(f.fn);\
        if(!tmp)QapDebugMsg("texture file not found - "+f.fn);\
        if(MODE==2)tmp=m2(tmp);\
        Frame##NAME=Atlas.AddFrame(tmp);\
        if(MODE==2)tmp->CalcAlphaToRGB_and_set_new_alpha()->InvertRGB();\
        if(MODE==2)Frame##NAME##_s=GenShadowFrame(tmp);\
        f.pF=Frame##NAME;f.pS=Frame##NAME##_s;\
        delete tmp;\
      }
      FRAMESCOPE(F);
      #undef F
    }
    if(need_save_atlas)Atlas.pMem->SaveToFile("Atlas.png");
    Atlas.GenTex();
  }
  void Init()
  {
    DoReset();
    srand(time(NULL));
    {
      QapDX::QapTexMem*pNormMem=NormFont.CreateFontMem("Arial",14,false,512);
      QapDX::QapTexMem*pBlurMem=pNormMem->Clone();
      //pBlurMem->Blur(10);
      QapDX::BlurTexture(pBlurMem,4);
      BlurFont=NormFont;
      BlurFont.Tex=QapDX::GenTextureMipMap(pBlurMem);
      NormFont.Tex=QapDX::GenTextureMipMap(pNormMem);
      //SysFont=QapDX::FontCreate("Arial",16,false,512);
    }
    LoadFrames();
    /*
    {
      auto*ptm=QapDX::LoadTexture("GFX\\tank_hodun_rt.png");
      ptm->GenEdge
      th_rt_tex_v2=QapDX::GenTextureMipMap(ptm);
    }*/
    if(0)
    {
      auto*ptm=QapDX::LoadTexture("GFX\\tank_hodun_rt.png");
      if(bool red_is_transparent=true){
        auto*p=ptm->pBits;
        int n=ptm->W*ptm->H;
        auto v=210;
        auto c=QapColor(0,v,v,v);
        for(int i=0;i<n;i++){auto&v=p[i];if(v==0xffff0000)v=c;}
      }
      th_rt_tex=QapDX::GenTextureMipMap(ptm);
    }
    if(1)
    {
      //auto*ptm=QapDX::LoadTexture("GFX\\tank512.png");
      auto*ptm=QapDX::LoadTexture("GFX\\market_car_v2.png");
      th_rt_tex=QapDX::GenTextureMipMap(ptm);
      ptm=QapDX::LoadTexture("GFX\\market_car_v2_full.png");
      th_rt_tex_full=QapDX::GenTextureMipMap(ptm);
    }
    RD.Init(1024*32,1024*32*2);
    InitLevelsInfo();
    //RestartLevel();
    InitMenuSystem();
  }
  void InitLevelsInfo()
  {
    #define ADDLEVEL(CLASS){static TLevelFactory<CLASS>tmp;LevelsInfo.push_back(TLevelInfo(#CLASS,tmp));}
    LEVEL_LIST(ADDLEVEL);
    #undef ADDLEVEL
    LevelCounter.Maximum=LevelsInfo.size();
  }
  void NewGame(){
    LevelCounter.Value=0;
    RestartLevel();
  }
  void RestartLevel()
  {
    if(!LevelCounter){
      QapAssert(("Поздравляю вы полностью прошли игру!(Это не баг, это фича!).",false));
      return;
    };
    Level.reset(LevelsInfo[LevelCounter.Value].Factory.Build(this));
    WaitWin.Stop();
    WaitFail.Stop();
  }
  void InitMenuSystem(){
    static class TOnResume:public IOnClick{
    public:
      void Call(TMenu*EX){
        EX->Game->Menu->Down();
      }
      virtual bool IsEnabled(TMenu*EX){return EX->Game->Level.get();}
    } OnResume;
    static class TOnRestartLevel:public IOnClick{
    public:
      void Call(TMenu*EX){
        EX->Game->RestartLevel();
        EX->Game->Menu->Down();
      }
      virtual bool IsEnabled(TMenu*EX){return EX->Game->LevelCounter.Value;}
    } OnRestartLevel;
    static class TOnNewGame:public IOnClick{
    public:
      void Call(TMenu*EX){
        EX->Game->NewGame();
        EX->Game->Menu->Down();
      }
    } OnNewGame;
    static class TOnBack:public IOnClick{
    public:
      void Call(TMenu*EX){
        EX->Back();
      }
    } OnBack;
    static class TOnNothing:public IOnClick{
    public:
      void Call(TMenu*EX){}
      virtual bool IsEnabled(TMenu*EX){return false;}
    } OnNothing;
    static class TOnSettings:public IOnClick{
    public:
      void Call(TMenu*EX){
        auto&Menu=EX->Game->Menu;
        auto OldMenu=std::unique_ptr<TMenu>(Menu.release());
        Menu.reset(new TMenu(EX->Game,"Settings"));
        Menu->Add("Under construction",&OnNothing);
        Menu->Add("Back",&OnBack);
        Menu->OldMenu=std::unique_ptr<TMenu>(OldMenu.release());
        Menu->Up();
      }
      //virtual bool IsEnabled(TMenu*EX){return false;}
    } OnSettings;
    static class TOnLevel:public IOnClick{
    public:
      void Call(TMenu*EX){
        auto&m=*EX->Game->Menu.get();
        //auto&lvls=EX->Game->LevelsInfo;
        //m.Items[m.CurID].Caption
        EX->Game->LevelCounter.Value=m.CurID;
        EX->Game->RestartLevel();
        m.Down();
      }
      //virtual bool IsEnabled(TMenu*EX){return false;}
    } TOnLevel;
    static class TOnLevels:public IOnClick{
    public:
      void Call(TMenu*EX){
        auto&Menu=EX->Game->Menu;
        auto OldMenu=std::unique_ptr<TMenu>(Menu.release());
        Menu.reset(new TMenu(EX->Game,"Levels"));
        for(int i=0;i<EX->Game->LevelsInfo.size();i++){
          auto&ex=EX->Game->LevelsInfo[i];
          Menu->Add(ex.Name,&TOnLevel);
        }
        Menu->Add("Back",&OnBack);
        Menu->OldMenu=std::unique_ptr<TMenu>(OldMenu.release());
        Menu->Up();
      }
      //virtual bool IsEnabled(TMenu*EX){return false;}
    } OnLevels;
    static class TOnAbout:public IOnClick{
    public:
      void Call(TMenu*EX){
        auto&Menu=EX->Game->Menu;
        auto OldMenu=std::unique_ptr<TMenu>(Menu.release());
        Menu.reset(new TMenu(EX->Game,"About"));
        Menu->Add("^2Aut^2hor ^2: ^8Ad^8ler^33D",&OnNothing);
        Menu->Add("^2Co^2de ^2: ^8Ad^8ler^33D",&OnNothing);
        Menu->Add("^2A^2r^2t ^2: ^8Ad^8ler^33D",&OnNothing);
        Menu->Add("Back",&OnBack);
        Menu->OldMenu=std::unique_ptr<TMenu>(OldMenu.release());
        Menu->Up();
      }
      //virtual bool IsEnabled(TMenu*EX){return false;}
    } OnAbout;
    static class TOnExit:public IOnClick{
    public:
      void Call(TMenu*EX){
        Sys.Quit();
      }
    } OnExit;
    Menu.reset(new TMenu(this,"Main menu"));
    Menu->Add("Resume",&OnResume);
    Menu->Add("Restart level",&OnRestartLevel);
    Menu->Add("New game",&OnNewGame);
    Menu->Add("Levels",&OnLevels);
    Menu->Add("Settings",&OnSettings);
    Menu->Add("About",&OnAbout);
    Menu->Add("Exit",&OnExit);
    LevelCounter.Value=0;
    update_user_name();
    RestartLevel();
    Menu->Down();
    //Menu->Up();
  }
  void Resume()
  {

  }
  string user_name;
  bool user_name_scene=true;
  void InputUserNameRender(){
    TextRender TE(&RD);
    vec2d hs=vec2d((Sys.SM.W,1024,Sys.SM.W),Sys.SM.H)*0.5;
    real ident=24.0;
    real Y=0;
    RD.SetColor(0xff000000);
    TE.BeginScope(-hs.x+ident,+hs.y-ident,&NormFont,&BlurFont);
    {
      const string PreesR=" ^7(^3press ^2R^7)";
      const string PreesSpace=" ^7(^3press ^2Space^7)";
      const string PreesEnter=" ^7(^3press ^2Return^7)";
      string BEG="^7";
      string SEP=" ^2: ^8";
      TE.AddText("^7The ^2Market Game");
      TE.AddText("");
      TE.AddText("^7Type your ^8user_name ^7and press enter!");
      TE.AddText("^8user_name ^2: ^7"+user_name);
    }
    TE.EndScope();
  }
  bool need_init=true;
  bool check_char(char c){
    return InDip('a',c,'z')||InDip('A',c,'Z')||InDip('а',c,'я')||InDip('А',c,'Я')||InDip('0',c,'9')||c=='ё'||c=='Ё';
  }
  void update_user_name(){
    user_name=file_get_contents(user_name_fn);
    string un;
    for(auto&c:user_name)if(check_char(c))un.push_back(c);
    user_name=un;
  }
  string user_name_fn="user_name.txt";
  void InputUserNameUpdate(){
    if(need_init){
      need_init=false;
      update_user_name();
      user_name_scene=user_name.empty();
    }
    if(QapInput::News){
      auto c=QapInput::LastChar;
      if(check_char(c))user_name.push_back(c);
    }
    if(QapInput::OnDown(VK_BACK))if(user_name.size())user_name.pop_back();
    if(QapInput::Down[VK_RETURN]){
      if(user_name.size()){
        user_name_scene=false;
        file_put_contents(user_name_fn,user_name);
      }
    }
  }
  void RenderScene()
  {
    /*
    RD.BindTex(0,0);
    RD.SetColor(0xff000000);
    RD.DrawQuad(0,0,512,512);
    if(!QapDX::EndScene())return;
    QapDX::Present();
    return;*/
    if(user_name_scene)return InputUserNameRender();
    if(bool need_draw_tank_hodun_rt=true)if(th_rt_tex)if(!Menu->InGame()){
      RD.BindTex(0,th_rt_tex);
      RD.SetColor(0xffffffff);
      RD.DrawQuad(-512,0,th_rt_tex->W,th_rt_tex->H,0);
      RD.BindTex(0,0);
    }
    if(QapInput::Down['A']&&!Menu->InGame()){
      if(1){
        RD.BindTex(0,0);
        QapDX::SetColor(0xffffffff);
        QapDX::DrawQuad(QapInput::MousePos.x,QapInput::MousePos.y,96,96,0);
        QapDX::SetColor(0xffff0000);
        QapDX::DrawQuad(QapInput::MousePos.x,QapInput::MousePos.y,64,64,0);
      }
      RD.BindTex(0,Atlas.pTex);
      RD.SetBlendMode(QapDX::BT_SUB);
      RD.SetColor(0xffffffff);
      RD.DrawQuad(0.5,0.5,Atlas.W,Atlas.H,0);
    }
    QapAssert(Menu.get());
    if(Menu->InGame())
    {
      if(Level.get()){
        Level->Render(&RD);
      }
    }else{
      TextRender TE(&RD);
      RD.SetColor(0xff000000);
      TE.BeginScope(0,0,&NormFont,&BlurFont);
      Menu->Render(&RD,&TE);
      TE.EndScope();
    };
    {
      RenderText(RD);
    }
  }
  void Render()
  {
    if(!QapDX::BeginScene())return;
    QapDX::Set2D();
    //QapDX::Clear2d(1?QapColor(180,180,180):0xffc8c8c8);
    {int v=231*0+206*0+210;QapDX::Clear2d(QapColor(v,v,v));}
    RD.NextFrame();
    RenderScene();
    if(!QapDX::EndScene())return;
    QapDX::Present();
  }
  void RenderText(QapDev&RD)
  {
    TextRender TE(&RD);
    vec2d hs=vec2d((Sys.SM.W,1024,Sys.SM.W),Sys.SM.H)*0.5;
    real ident=24.0;
    real Y=0;
    RD.SetColor(0xff000000);
    TE.BeginScope(-hs.x+ident,+hs.y-ident,&NormFont,&BlurFont);
    {
      const string PreesR=" ^7(^3press ^2R^7)";
      const string PreesSpace=" ^7(^3press ^2Space^7)";
      const string PreesEnter=" ^7(^3press ^2Return^7)";
      string BEG="^7";
      string SEP=" ^2: ^8";
      TE.AddText("^7The ^2Game");
      TE.AddText("");
      TE.AddText("^8user_name ^2: ^7"+user_name);
      TE.AddText("");
      #define GOO(TEXT,VALUE)TE.AddText(string(BEG)+string(TEXT)+string(SEP)+string(VALUE));
      GOO("Level",string(LevelCounter)+" ["+string(LevelCounter?LevelsInfo[LevelCounter.Value].Name:"noname")+"]");
      #undef GOO
      TE.AddText("");
      if(Level.get())Level->AddText(&TE);
      TE.AddText("");
      if(WaitWin.Runned)TE.AddText("^2You win!"+PreesEnter);
      if(WaitFail.Runned)TE.AddText("^1You lose!"+PreesR);
      if(!WaitFail||!WaitWin){TE.AddText("^7game over!");}
    }
    TE.EndScope();
  }
  void Collide()
  {    
  }
  void NextLevel()
  {
    LevelCounter++;
    RestartLevel();
  }
  void OnWin(){
    //Level.reset();
  }
  void OnFail(){
    //Level.reset();
  }
  void Win(){WaitWin.Start();}
  void Fail(){WaitFail.Start();}
  void ReloadWinFail(){WaitFail.Stop();WaitWin.Stop();}
  void Update()
  {
    if(user_name_scene)return InputUserNameUpdate();
    QapAssert(Menu.get());
    if(QapInput::Down[VK_ESCAPE]){if(Menu->InGame()){Menu->Up();}else{Menu->Down();}QapInput::Down[VK_ESCAPE]=false;}
    QapInput::UpdateMouse();
    if(Menu->InGame())
    {
      if(Level.get())
      {
        Level->Update(this);
        {WaitFail++;WaitWin++;}
        if(!WaitWin.Runned&&!WaitFail.Runned)
        {
          bool w=Level->Win();bool f=Level->Fail();
          if(f)Fail();
          if(w&&!f)Win();
        }else{
          if(!WaitFail)OnFail();
          if(!WaitWin)OnWin();
        }
      };
      if(QapInput::OnDown('R')){RestartLevel();}
      if(WaitFail.Runned||WaitWin.Runned)
      {
        if(WaitWin.Runned&&QapInput::Down[VK_RETURN]){NextLevel();}
      }
    }else{
      Menu->Update(this);
    }
  }
  void Free()
  {
    QapDX::FreeFont(NormFont);QapDX::FreeFont(BlurFont);
    QapDX::UnloadTextures();
  }
  static string wget(const string&host,const string&dir){
    DownLoader dl(host,dir,"");
    dl.port=80;
    dl.start();
    for(;dl.update();){
      Sleep(0);
    }
    auto s=dl.GetContent(dl.data,true);
    dl.stop();
    return s;
  }
  static string get_host(){
    static auto host=wget("adler3d.github.io","/qap_vm/trash/test2025/game_host.txt");
    if(host.size()&&host.back()=='\n')host.pop_back();
    return host;
  }
};
/*------------------------------------------*/
TGame*GetGame(TGame*pValue=NULL)
{
  static TGame*pGame=NULL;
  if(pValue)pGame=pValue;
  return pGame;
}
/*------------------------------------------*/
void Init(){GetGame()->Init();}
void Render(){GetGame()->Render();}
void Update(){for(int i=0;i<1;i++)GetGame()->Update();}
void Free(){GetGame()->Free();}
/*------------------------------------------*/
#include "../GenInfo/BuildInfo.h"

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
  //auto s=TGame::wget("185.92.223.117","/logs.json");
  MACRO_ADD_LOG("App.version : "+IToS(AfterBuildCount)+"/"+IToS(BeforeBuildCount+AfterBuildCount),lml_EVENT);
  TScreenMode SM=GetScreenMode();
  Sys.SetProc(PROC_RENDER,&Render);
  Sys.SetProc(PROC_UPDATE,&Update);
  Sys.SetProc(PROC_INIT,&Init);
  Sys.SetProc(PROC_FREE,&Free);
  SET_DEBUG_VAR(Sys.Debug);
  Sys.Debug=true;
  Sys.WindowCreate("QapEngine(by Adler3D)");
  Sys.WindowMode(true,SM.W,SM.H,SM.BPP,SM.Freq);
  Sys.InitD3D();
  {
    TGame Game;
    GetGame(&Game);
    Sys.Run(128);
  }
	return 0;
}
//-------------------------------------------//
//   {<|          27.07.2011           |>}   //
//-------------------------------------------//