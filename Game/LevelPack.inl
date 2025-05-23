static const int Sys_UPD=64;
//#define LEVEL_LIST(F)F(Level3);F(Level2);F(Level0);F(Level1);
#define LEVEL_LIST(F)F(Level_MarketGame);
class Level_MarketGame:public TGame::ILevel{
public:
  struct t_item{
    int id=0;
    int amount=0;
    int price=0;
  };
  struct t_market{
    vec2d pos;
    vector<t_item> items;
  };
  struct t_city{
    vector<t_market> arr;
  };
  struct t_bookmark{
    vector<t_item> items;
  };
  struct t_cargo_item{
    int id=0;
    int amount=0;
  };
  struct t_cargo{
    vector<t_cargo_item> items;
  };
  struct t_car{
    t_cargo cargo;
    int money=1000;
    vec2d pos;
    vec2d v;
    bool deaded=false;
  };
  struct t_dynamic_obstacle{
    vec2d pos;
    real ang=0;
    real len=100;
    real speed=1;
    real r=8;
    bool circle=false;
    real dt=0;
    real gang=0;
    real gspd=0;
  };
  struct t_world{
    int t=0;
    real obstacle_r=48;
    real tank_r=48;
    real market_r=32;
    t_car car;
    t_city city;
    vector<t_dynamic_obstacle> dyn_obs;
    vector<vec2d> obstacles;
  };
  bool sell(t_world&w,int market_id,const t_cargo_item&item){
    auto&cit=w.car.cargo.items[item.id];
    if(cit.amount<item.amount)return false;
    cit.amount-=item.amount;
    auto&it=w.city.arr[market_id].items[item.id];
    it.amount+=item.amount;
    w.car.money+=it.price*item.amount;
    return true;
  }
  bool buy(t_world&w,int market_id,const t_cargo_item&item){
    auto&it=w.city.arr[market_id].items[item.id];
    if(it.amount<item.amount)return false;
    auto dm=it.price*item.amount;
    if(w.car.money<dm)return false;
    it.amount-=item.amount;
    w.car.cargo.items[item.id].amount+=item.amount;
    w.car.money-=dm;
    return true;
  }
public:
  t_world w;
public:
#define PRO_VARIABLE()\
ADDVAR(TGame*,Game,NULL)\
//=====+>>>>>Level_MarketGame
#include "GenVar.inl"
//<<<<<+=====Level_MarketGame
public:
  bool init_city(){
    vector<int> base_price;
    base_price.resize(5);
    for(int i=0;i<5;i++){
      base_price[i]=25+rand()%100+pow(3,i+1);
    }
    int try_count=0;
    for(int i=0;i<10;i++){
      t_market m;
      m.pos=vec2d(rand()%1000-500,rand()%1000-500);
      bool ignore=false;
      for(auto&market:w.city.arr){
        if((market.pos-m.pos).Mag()<w.market_r*10)ignore=true;
      }
      if(ignore){i--;try_count++;if(try_count>2000)return false;continue;}
      m.items.resize(5);
      for(int i=0;i<5;i++){m.items[i].id=i;m.items[i].price=base_price[i]+rand()%(50+int(pow(3,i+1)));m.items[i].amount=50+rand()%200;}
      w.city.arr.push_back(m);
    }
    return true;
  }
  void init_cargo_items(){
    w.car.cargo.items.resize(5);
    for(int i=0;i<5;i++)w.car.cargo.items[i].id=i;
  }
  void init_obstacles(){
    for(int i=0;i<20;i++){
      vec2d p=vec2d(rand()%1000-500,rand()%1000-500);
      if((p-w.car.pos).Mag()<w.obstacle_r+w.tank_r)continue;
      bool ignore=false;
      for(auto&ex:w.city.arr){
        if((ex.pos-p).Mag()<w.obstacle_r+w.market_r)ignore=true;
      }
      if(ignore)continue;
      w.obstacles.push_back(p);
    }
  }
  bool init_dyn_obs_v2(){
    vector<vec2d> PA;
    for(auto&ex:w.city.arr){PA.push_back(ex.pos);}
    auto EA=get_voronoi_edges(PA);edges=EA;
    int try_count=0;
    for(int i=0;i<EA.size();i++){
      for(int j=0;j<3;j++){
        auto&it=EA[i];
        real k=(rand()%1000)/1000.0-0.5;
        vec2d p=((it.a+it.b)*0.5+(it.b-it.a)*k);//(it.a+it.b)*0.5;
        real r=18;//rand()%8+10;
        t_dynamic_obstacle ex;
        ex.circle=false;
        ex.pos=p;
        ex.ang=(rand()%(314*2))/100.0;
        ex.len=ex.circle?rand()%100+w.tank_r*2+r:rand()%500+200;
        ex.speed=(rand()%1000)/1000.0+1;
        ex.r=r;
        ex.gang=(rand()%1000)*Pi*2/1000.0;
        ex.gspd=(rand()%2000)/1000.0-1;
        ex.dt=(rand()%32000)*3.14*2*40/32000;
        bool ignore=is_dangerous(ex,w.car.pos,w.tank_r*1.5);
        for(auto&market:w.city.arr){
          if(ignore)break;
          if(is_dangerous(ex,market.pos,w.tank_r*1.5))ignore=true;
        }
        if(ignore){j--;try_count++;if(try_count>2000){j++;bad_edges.push_back(EA[i]);return false;}continue;}
        w.dyn_obs.push_back(ex);
      }
    }
    return bad_edges.empty();
  }
  void init_dyn_obs(){
    int try_count=0;
    for(int i=0;i<7+3+4;i++){
      vec2d p=vec2d(rand()%500-250,rand()%500-250);
      real r=rand()%8+10;
      t_dynamic_obstacle ex;
      ex.circle=i%2;
      ex.pos=p;
      ex.ang=(rand()%(314*2))/100.0;
      ex.len=ex.circle?rand()%500+w.tank_r*2+r:rand()%500+200;
      ex.speed=(rand()%1000)/1000.0+1;
      ex.r=r;
      ex.dt=rand();
      bool ignore=is_dangerous(ex,w.car.pos,w.tank_r*1.5);
      for(auto&market:w.city.arr){
        if(is_dangerous(ex,market.pos,w.tank_r*1.5))ignore=true;
      }
      if(ignore){i--;try_count++;if(try_count>1000)i++;continue;}
      w.dyn_obs.push_back(ex);
    }
  }
  t_world world_at_begin;
  void reinit_the_same_level(){
    w=world_at_begin;
    Game->ReloadWinFail();
  }
  bool init_attempt(){
    static QapClock clock;
    srand(seed=(clock.qpc()-clock.beg)%INT_MAX);
    bad_edges.clear();
    w={};
    if(!init_city())return false;
    init_cargo_items();
    init_obstacles();
    if(!init_dyn_obs_v2())return false;
    //init_dyn_obs();
    world_at_begin=w;
    return true;
  }
  int init_attempts=1;
  bool inited=false;
  struct t_rec{
    int place;
    string user;
    real sec;
    string date;
    int seed;
    string game;
    vector<string> to_str()const{
      return {IToS(place),user,FToS(sec),date,IToS(seed)};
    }
  };
  vector<t_rec> tops;
  string ref="github";
  void reinit_top20(){
    tops={};
    auto s=TGame::wget(TGame::get_host(),"/c/game_players_table.js?unique&csv&game=market&n=20&ref="+ref+"&user="+Game->user_name);
    auto arr=split(s,"\n");
    if(arr.size())QapPopFront(arr);
    for(auto&ex:arr){
      auto t=split(ex,",");
      QapAssert(t.size()>=6);
      t_rec r;
      r.place=stoi(t[0]);
      r.user=t[1];
      r.sec=stof(t[2]);
      r.date=t[3];
      r.seed=stoi(t[4]);
      r.game=t[5];
      tops.push_back(r);
    }
  }
  void Init(TGame*Game){
    this->Game=Game;
    inited=init_attempt();
    if(!inited){Sys.UPS_enabled=false;}
    reinit_top20();
    //for(;;init_attempts++){
    //  bool ok=init_attempt();
    //  if(ok)break;
    //}
  }
  bool is_dangerous(const t_dynamic_obstacle&ex,vec2d pos,real r){
    for(int i=0;i<Sys.UPS*20;i++){
      if((dyn_pos(ex,i)-pos).Mag()<ex.r+r)return true;
    }
    return false;
  }
  bool Win(){return w.car.money>60000/*||w.car.pos.x>10*/;}
  bool Fail(){return w.car.deaded;}
public:
  struct t_edge{
    vec2d a,b;
  };
  vector<t_edge> edges,bad_edges;
  static vector<t_edge> get_voronoi_edges(vector<vec2d>&points){
    double eps=1e-12;
    using value_type=double;
    struct t_hacked_vec2d:vec2d{
      t_hacked_vec2d():vec2d(){}
      t_hacked_vec2d(real x,real y):vec2d(x,y){}
      bool operator<(const t_hacked_vec2d&p)const{return std::tie(x,y)<std::tie(p.x,p.y);}
    };
    using point=t_hacked_vec2d;
    using t_points=std::vector<point>;
    using site=typename t_points::const_iterator;
    using sweepline_type=sweepline<site,point,value_type>;
    auto arr=(vector<t_hacked_vec2d>&)points;
    sweepline_type SL{eps};
    // fill points_ with data
    std::sort(std::begin(arr), std::end(arr));
    //quick_sort(arr);
    SL(std::cbegin(arr),std::cend(arr));
    //SL.vertices_
    vector<t_edge> edges;
    edges.clear();
    vector<vec2d> VA;
    for(auto&ex:SL.vertices_)VA.push_back(ex.c);
    for(auto&ex:SL.edges_){
      if(ex.b==SL.inf&&ex.e==SL.inf)continue;
      auto get=[&](size_t vid,int sign,size_t other){
        if(vid!=SL.inf){
          QapAssert(vid<VA.size());
          return VA[vid];
        }
      };
      //edges.push_back({VA[ex.b],VA[ex.e]});
      edges.push_back({*ex.l,*ex.r});
    }
    return edges;
  }
public:
  vec2d get_dir_from_keyboard_wasd_and_arrows()
  {
    vec2d dp=vec2d_zero;
    auto dir_x=vec2d(1,0);
    auto dir_y=vec2d(0,1);
    #define F(dir,key_a,key_b)if(QapInput::Down[key_a]||QapInput::Down[key_b]){dp+=dir;}
    F(-dir_x,VK_LEFT,'A');
    F(+dir_x,VK_RIGHT,'D');
    F(+dir_y,VK_UP,'W');
    F(-dir_y,VK_DOWN,'S');
    #undef F
    return dp;
  }
  string id2str(int id){
    static vector<string> arr={"wood","coal","gas","stell","copper"};
    return arr[id];
  }
  int get_market_id(vec2d pos,bool ignore_r){
    auto mpos=pos;//QapInput::MousePos
    int market_id=0;
    for(int i=0;i<w.city.arr.size();i++){
      auto a=(mpos-w.city.arr[i].pos).Mag();
      auto b=(mpos-w.city.arr[market_id].pos).Mag();
      if(a<b)market_id=i;
    }
    auto dist=(mpos-w.city.arr[market_id].pos).Mag();
    if(!ignore_r)if(dist>w.market_r)return -1;
    return market_id;
  }
  void AddText(TextRender*TE){
    string BEG="^7";
    string SEP=" ^2: ^8";
    #define GOO(TEXT,VALUE)TE->AddText(string(BEG)+string(TEXT)+string(SEP)+string(VALUE));
    GOO("curr_t",FToS(w.t*1.0/Sys.UPS));
    GOO("prev_best_t",FToS(prev_best_t*1.0/Sys.UPS));
    GOO("curr_best_t",FToS(best_t*1.0/Sys.UPS));
    GOO("px",IToS(QapInput::MousePos.x));
    GOO("py",IToS(QapInput::MousePos.y));
    GOO("init_attempts",IToS(init_attempts));
    TE->AddText("^7---");
    /*auto market_id=get_market_id();
    for(auto&it:w.city.arr[market_id].items){
      TE->AddText(BEG+id2str(it.id)+SEP+IToS(it.price)+SEP+IToS(it.amount));
    }*/
    GOO("Money",IToS(w.car.money));
    TE->AddText("^8Need ^760000 ^8money to ^2win^8 in this game");
    TE->AddText("^7---");
    TE->AddText("^8Your cargo:");
    for(auto&it:w.car.cargo.items){
      TE->AddText(BEG+id2str(it.id)+SEP+IToS(it.amount));
    }
    TE->AddText("^7---");
    TE->AddText("TOP20:");
    vector<vector<string>> arr;
    vector<int> lens;lens.resize(5);
    for(auto&it:tops){
      arr.push_back(it.to_str());
    }
    for(auto&it:arr){
      for(int i=0;i<5;i++)lens[i]=max(lens[i],TE->text_len(it[i]));
    }
    auto seplen=TE->text_len("  ");
    vector<string> c={"^8","^7","^8","^7","^8"};
    for(auto&it:arr){
      for(int i=0;i<3;i++){auto x=TE->x;TE->AddTextNext(c[i]+it[i]);TE->x=x+lens[i]+seplen;}
      TE->BR();
    }
    #undef GOO
  }
  void RenderText(QapDev&RD)
  {
    TextRender TE(&RD);
    vec2d hs=vec2d(Sys.SM.W,Sys.SM.H)*0.5;
    hs.x=100;
    real ident=24.0;
    real Y=0;
    RD.SetColor(0xff000000);
    TE.BeginScope(-hs.x+ident,+hs.y-ident,&Game->NormFont,&Game->BlurFont);
    {
      string BEG="^7";
      string SEP=" ^2: ^8";
      //TE.AddText("");
      RenderText(&RD,&TE);
    }
    TE.EndScope();
  }
  //vector<TextRender::TextLine> tls;
  void DrawMarketMenu(QapDev*RD,TextRender*TE,real text_posx,int mid,bool buttons)
  {
    if(w.car.cargo.items.empty())return;
    if(mid<0)return;
    string BEG="^7";
    string SEP=" ^2: ^8";
    const real dy=32;
    auto&items=w.city.arr[mid].items;
    TE->bx=text_posx;
    TE->x=TE->bx;
    TE->y=+0.5*items.size()*dy;
    //RD->BindTex(0,Game->Atlas.pTex);
    //Game->FrameMenuItem->Bind(RD);
    //RD->SetColor(0x80ffffff);
    //RD->DrawQuad(0,y-dy*CurID,Game->FrameMenuItem->w,Game->FrameMenuItem->h,0);
    vector<string> tearr,idarr,pricearr,amountarr;
    for(int i=0;i<items.size();i++){
      idarr.push_back(BEG+id2str(i));
      pricearr.push_back(IToS(items[i].price));
      amountarr.push_back(IToS(items[i].amount));
      tearr.push_back(BEG+id2str(i)+SEP+IToS(items[i].price)+SEP+IToS(items[i].amount));
    }
    auto get_maxlen=[&](vector<string>&arr){
      int maxlen=0;
      for(int i=0;i<arr.size();i++){
        auto len=TE->text_len(arr[i]);
        if(len>maxlen)maxlen=len;
      }
      return maxlen;
    };
    int idlen=get_maxlen(idarr);
    int pricelen=get_maxlen(pricearr);
    int amountlen=get_maxlen(amountarr);
    int maxlen=get_maxlen(tearr);
    int seplen=TE->text_len(SEP);
    bt_buy_all={};bt_sell_all={};
    for(int i=0;i<idarr.size();i++){
      TE->AddTextNext(idarr[i]);
      TE->x=TE->bx+idlen;
      TE->AddTextNext(SEP);
      TE->x=TE->bx+idlen+seplen+pricelen-TE->text_len(pricearr[i]);
      TE->AddTextNext(pricearr[i]);
      TE->x=TE->bx+idlen+seplen+pricelen;
      TE->AddTextNext(SEP);
      TE->x=TE->bx+idlen+seplen+pricelen+seplen+amountlen-TE->text_len(amountarr[i]);
      TE->AddTextNext(amountarr[i]);
      TE->x=TE->bx+idlen+seplen+pricelen+seplen+amountlen;
      TE->AddTextNext(SEP);
      //TE->x=TE->bx+maxlen;
      if(buttons)
      {
        auto dpos=QapInput::MousePos-vec2d(TE->x,TE->y-TE->ident);
        auto es=vec2d(TE->text_len(" ^7[BuyAll]"),TE->ident);
        bool hovered=check_rect(dpos,es);
        TE->AddTextNext(" "+string(hovered?"^8":"^7")+"[BuyAll]");
        t_cargo_item ci;
        ci.id=i;
        ci.amount=w.car.money/items[i].price;
        if(ci.amount>items[i].amount)ci.amount=items[i].amount;
        if(hovered&&QapInput::Down[mbLeft]){/*buy(w,mid,ci);*/TE->LV.back().text=" ^2[BuyAll]";}
        if(hovered){
          if(QapInput::Down[mbLeft]){
            int gg=1;
          }
          bt_buy_all={ci,mid,hovered};
        }
      }
      if(buttons)
      {
        auto dpos=QapInput::MousePos-vec2d(TE->x,TE->y-TE->ident);
        auto es=vec2d(TE->text_len(" ^7[SellAll]"),TE->ident);
        bool hovered=check_rect(dpos,es);
        TE->AddTextNext(" "+string(hovered?"^8":"^7")+"[SellAll]");
        t_cargo_item ci;
        ci.id=i;
        ci.amount=w.car.cargo.items[i].amount;
        if(hovered&&QapInput::Down[mbLeft]){/*sell(w,mid,ci);*/TE->LV.back().text=" ^2[SellAll]";}
        if(hovered){bt_sell_all={ci,mid,hovered};}
      }
      TE->BR();
    }
    //tls=TE->LV;
  }
  struct t_button{t_cargo_item ci;int mid=0;bool hovered=false;};
  t_button bt_buy_all;
  t_button bt_sell_all;
  bool check_rect(vec2d dpos,vec2d es){
    return (dpos.x>0&&dpos.x<es.x)&&(dpos.y>0&&dpos.y<es.y);
  }
  void RenderText(QapDev*RD,TextRender*TE)
  {
    int mid=get_market_id(QapInput::MousePos,true);
    int pid=get_market_id(w.car.pos,false);
    DrawMarketMenu(RD,TE,400,mid,false);
    DrawMarketMenu(RD,TE,0,pid,true);
  }
  vec2d dyn_pos(const t_dynamic_obstacle&ex,int dt=0)
  {
    vec2d offset=Vec2dEx(ex.ang,ex.len*sin(M_PI*2*ex.speed*(w.t+dt)/30/Sys.UPS));
    vec2d offset2=Vec2dEx(M_PI*2*ex.speed*(w.t+dt+ex.dt)/30/Sys.UPS,ex.len);
    return ex.pos+(ex.circle?offset2:offset);
  }
  bool need_draw_dyn_obs_lines=false;
  void Render(QapDev*RD){
    if(bool need_draw_rock0_as_thrt=true){
      auto&qDev=*RD;
      if(need_draw_dyn_obs_lines/*QapInput::Down['L']*/){
        RD->BindTex(0,0);
        qDev.SetColor(0xffbbbbbb);
        for(auto&ex:w.dyn_obs){
          auto d=Vec2dEx(ex.ang,ex.len);
          DrawLine(qDev,ex.pos-d,ex.pos+d,ex.r*2);
        }
        qDev.SetColor(0xffbbbbff);
        for(auto&ex:w.dyn_obs){
          auto d=Vec2dEx(ex.ang,ex.len);
          qDev.DrawQuad(ex.pos.x,ex.pos.y,8,8);
        }
      }
      auto draw_shadow_quad_v2=[&](QapDev&qDev,const TGame::t_frame&f,vec2d pos,real r,QapColor c){
        real zoom=r*2/real(f.pF->w);
        draw_shadow_quad(qDev,f.pS,true,pos,vec2d(f.pS->w*zoom,f.pS->h*zoom),c);
        draw_shadow_quad(qDev,f.pF,false,pos,vec2d(1,1)*r*2,c);
      };
      if(bool need_draw_obstacles=true){
        //RD->BindTex(0,0);
        qDev.SetColor(0xff888888);
        RD->BindTex(0,Game->Atlas.pTex);
        QapDev::BatchScope Scope(qDev);
        auto&F=*Game->FrameObstacle;
        qDev.SetColor(0xffffffff);
        F.Bind(RD);
        for(auto&ex:w.obstacles){
          qDev.DrawQuad(ex.x,ex.y,F.w,F.h);
          //qDev.DrawCircleEx(ex,0,w.obstacle_r,32,0);
          //draw_shadow_quad_v2(qDev,Game->frame_BigDot,ex,w.obstacle_r,0xff77aa77);
        }
      }
      if(bool need_draw_dyn_obs=true){
        //qDev.SetColor(0xff777777);
        qDev.SetColor(0xffffffff);
        RD->BindTex(0,Game->Atlas.pTex);
        Game->FrameEnemy->Bind(RD);
        QapDev::BatchScope Scope(qDev);
        for(auto&m:w.dyn_obs){
          auto p=dyn_pos(m);
          qDev.DrawQuad(p.x,p.y,128,128,m.gang);
          //draw_shadow_quad_v2(qDev,Game->frame_BigDot,dyn_pos(m),m.r,0xff777777);
        }
        //for(auto&ex:w.dyn_obs){
        //  RD->DrawCircleEx(dyn_pos(ex),0,ex.r,32,0);
        //}
      }
      if(bool need_draw_tank=true){
        bool cargo_empty=true;
        for(auto&ex:w.car.cargo.items)if(ex.amount>0)cargo_empty=false;
        auto*pF=cargo_empty?Game->th_rt_tex:Game->th_rt_tex_full;auto&qDev=*RD;
        qDev.BindTex(0,pF);
        qDev.SetColor(0xffffffff);
        auto scale=0.5;
        RD->DrawQuad(w.car.pos.x,w.car.pos.y,pF->W*scale,pF->H*scale,(-w.car.v.Ort()).GetAng());
      }
      if(bool need_draw_voronoi=QapInput::Down['V']){
        RD->BindTex(0,0);
        qDev.SetColor(0xff0000ff);
        for(auto&ex:edges){
          DrawLine(*RD,ex.a,ex.b,4);
        }
        qDev.SetColor(0xffff0000);
        for(auto&ex:bad_edges){
          DrawLine(*RD,ex.a,ex.b,4);
        }
      }
      if(bool need_draw_markets=true){
        qDev.SetColor(0xffffffff);
        RD->BindTex(0,Game->Atlas.pTex);
        QapDev::BatchScope Scope(qDev);
        for(auto&m:w.city.arr){
          //RD->DrawQuad(m.pos.x,m.pos.y,40,40);
          Game->FrameMarket->Bind(RD);
          RD->DrawQuad(m.pos.x,m.pos.y,128,128);
          //draw_shadow_quad_v2(qDev,Game->frame_Dot,m.pos,16,0xFFffFFff);
        }
      }
      int mid=get_market_id(QapInput::MousePos,true);
      if(mid>=0){
        auto mpos=w.city.arr[mid].pos;
        RD->BindTex(0,Game->Atlas.pTex);
        qDev.SetColor(0xffff0000);
        draw_shadow_quad_v2(qDev,Game->frame_Dot,mpos,16,0xffff0000);
        //RD->DrawQuad(mpos.x,mpos.y,23,23);
      }
      int pid=get_market_id(w.car.pos,false);
      if(pid>=0){
        auto mpos=w.city.arr[pid].pos;
        RD->BindTex(0,Game->Atlas.pTex);
        qDev.SetColor(0xff00ff00);
        //RD->DrawQuad(mpos.x,mpos.y,20,20);
        draw_shadow_quad_v2(qDev,Game->frame_Dot,mpos,16*0.45,0xff00ff00);
      }
      qDev.SetColor(0xffffffff);
      RenderText(*RD);
    }
  }
  static void draw_shadow_quad(QapDev&qDev,QapAtlas::TFrame*pF,bool shadow,vec2d pos,vec2d wh,QapColor color,real ang=0){
    qDev.SetColor(shadow?0xff000000:color);
    pF->Bind(&qDev);
    auto p=pos;
    if(shadow)p+=vec2d(1.0,-1.0);
    qDev.DrawQuad(p.x,p.y,wh.x,wh.y,ang);
  }
  void DrawLine(QapDev&qDev,const vec2d&a,const vec2d&b,real line_size)
  {
    auto p=(b+a)*0.5;
    qDev.DrawQuad(p.x,p.y,(b-a).Mag(),line_size,(b-a).GetAng());
  }
  bool colide(){
    for(auto&ex:w.dyn_obs){
      if((w.car.pos-dyn_pos(ex)).Mag()<ex.r+w.tank_r)return true;
    }
    return false;
  }
  void Update(TGame*Game){
    if(!inited){
      bool ok=init_attempt();
      init_attempts++;
      if(ok){inited=true;Sys.UPS_enabled=true;Sys.ResetClock();}else{return;}
    }
    {
      auto&bt=bt_buy_all;
      if(QapInput::OnDown(mbLeft)){
        if(bt.hovered){
          buy(w,bt.mid,bt.ci);
        }
      }
    }
    {
      auto&bt=bt_sell_all;
      if(bt.hovered&&QapInput::OnDown(mbLeft)){sell(w,bt.mid,bt.ci);}
    }
    for(auto&ex:w.dyn_obs){ex.gang+=Pi*0.9*ex.gspd/180;}
    if(QapInput::OnDown('L'))need_draw_dyn_obs_lines=!need_draw_dyn_obs_lines;
    //v+=get_dir_from_keyboard_wasd_and_arrows()*0.003;
    auto dk=0.0;auto dAng=6.28*0.33/(2*Sys_UPD);
    if(QapInput::OnDown(VK_F5)){reinit_the_same_level();}
    if(QapInput::Down[VK_LEFT]||QapInput::Down['A']){dk=+1;}
    if(QapInput::Down[VK_RIGHT]||QapInput::Down['D']){dk=-1;}
    if(w.car.deaded)dk=0;
    w.car.v=Vec2dEx(w.car.v.GetAng()+Clamp(dk,-5.0,+5.0)*dAng,1);
    auto v2=vec2d_zero;
    if(QapInput::Down[VK_UP]||QapInput::Down['W'])v2=+w.car.v;
    if(QapInput::Down[VK_DOWN]||QapInput::Down['S'])v2=-w.car.v;
    auto new_pos=w.car.pos+v2;
    auto new_v2=vec2d(0,0);int n=0;
    for(auto&ex:w.obstacles){
      auto dist=w.obstacle_r+w.tank_r;
      if((new_pos-ex).Mag()<dist){
        //v2=vec2d_zero;
        new_v2+=(new_pos-ex).SetMag(dist)+ex-w.car.pos;
        n++;
      }
    }
    v2=n?new_v2*(1.0/n):v2;
    bool runned=!Win()&&!Fail();
    if(runned)w.car.pos+=v2;
    if(runned)w.t++;
    if(colide())w.car.deaded=true;
    on_win=false;
    if(Win()){if(!wined)on_win=true;wined=true;}
    if(bool need_best_t=true){
      auto fn="score.txt";
      if(w.t==1){
        auto s=file_get_contents(fn);
        prev_best_t=s.empty()?1e9:SToI(s);
        best_t=prev_best_t;
      }
      if(on_win){
        if(prev_best_t>w.t){best_t=w.t;file_put_contents(fn,IToS(w.t));}
      }
    }
    if(on_win){
      auto host=TGame::get_host();
      TGame::wget(host,"/c/game_players_table.js?game=market&user="+Game->user_name+"&sec="+FToS(w.t*1.0/Sys.UPS)+"&seed="+IToS(seed)+"&ref="+ref);
      //QapDebugMsg(s);
      reinit_top20();
    }
  }

  int seed=0;
  int prev_best_t=0;
  int best_t=0;
  bool wined=false;
  bool on_win=false;
};