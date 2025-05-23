/*class IMenu{
  virtual ~IMenu(){}
};*/
class IOnClick;
class TMenu;
struct MenuItem{
public:
  string Caption;
  IOnClick*OnClick;
  MenuItem(const string&Caption,IOnClick*OnClick){this->Caption=Caption;this->OnClick=OnClick;};
};
class IOnClick{
public:
  virtual void Call(TMenu*EX)=0;
  virtual bool IsEnabled(TMenu*EX){return true;}
};
class TMenu{
private:
  bool in_game;
public:
  bool InGame(){return in_game;}
  void Up(){in_game=false;}
  void Down(){if(Game->Level.get())in_game=true;}
public:
  vector<MenuItem>Items;
  string Caption;
  TGame*Game;
  std::unique_ptr<TMenu>OldMenu;
  vec2d oldmp;
  int CurID;
  TMenu(TGame*Game,const string&Caption){this->Game=Game;this->Caption=Caption;};
  void Add(const string&s,IOnClick*p){Items.push_back(MenuItem(s,p));}
  void Back(){
    QapAssert(OldMenu.get());
    Game->Menu.release();
    Game->Menu.reset(OldMenu.release());
    //create memory leak
    delete this;
  }
  void AddText(real&y,TextRender*TE,const string&s,const real dy=32){
    real x=QapDX::GetQ3TextLength(*TE->NormFont,s);
    TE->LV.push_back(TextRender::TextLine(-x*0.5,y+TE->NormFont->H[0]*0.5,s));y-=dy;
  };
  void Render(QapDev*RD,TextRender*TE)
  {
    const real dy=32;
    real y=+0.5*Items.size()*dy;
    RD->BindTex(0,Game->Atlas.pTex);
    Game->FrameMenuItem->Bind(RD);
    RD->SetColor(0x80ffffff);
    RD->DrawQuad(0,y-dy*CurID,Game->FrameMenuItem->w,Game->FrameMenuItem->h,0);
    for(int i=0;i<Items.size();i++)AddText(y,TE,string(Items[i].OnClick->IsEnabled(this)?CurID==i?"^3":"^7":"^D")+Items[i].Caption,dy);
  }
  void Update(TGame*Game)
  {
    CurID%=Items.size();
    const real dy=32;
    real y=+0.5*Items.size()*dy;
    auto mp=QapInput::GetMousePos();
    vec2d hmis=vec2d(Game->FrameMenuItem->w,dy)*0.5;
    if(mp!=oldmp)
    {
      for(int i=0;i<Items.size();i++)
      {
        if(Items[i].OnClick->IsEnabled(this))if(CD_Rect2Point(-hmis+vec2d(0,y),hmis+vec2d(0,y),mp))CurID=i;
        y-=dy;
      }
      oldmp=mp;
    }
    for(int i=CurID;i<CurID+Items.size();i++)if(Items[i%Items.size()].OnClick->IsEnabled(this)){CurID=i;break;}
    if(QapInput::Down[VK_DOWN]){
      CurID++;
      for(int i=CurID;i<CurID+Items.size();i++)if(Items[i%Items.size()].OnClick->IsEnabled(this)){CurID=i;break;}
      QapInput::Down[VK_DOWN]=false;
    }
    if(QapInput::Down[VK_UP]){
      CurID--;
      for(int i=CurID+Items.size();i>CurID;i--)
        if(Items[i%Items.size()].OnClick->IsEnabled(this))
        {
          CurID=i;break;
        }
      QapInput::Down[VK_UP]=false;
    }
    CurID+=Items.size();
    CurID%=Items.size();
    if(QapInput::OnDown(VK_RETURN)){
      Items[CurID].OnClick->Call(this);
    }
    if(QapInput::Down[mbLeft]){
      vec2d dy(0,0.5*Items.size()*dy-CurID*dy);
      if(Items[CurID].OnClick->IsEnabled(this))if(CD_Rect2Point(-hmis+dy,hmis+dy,mp))
      {
        Items[CurID].OnClick->Call(this);
        QapInput::Down[mbLeft]=false;
      }
    }
  }
};