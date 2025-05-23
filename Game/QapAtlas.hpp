class QapAtlas{
public:
  int W,H;
  int X,Y;
  int Ident,dY;
  QapDX::QapTexMem*pMem;
  QapDX::QapTex*pTex;
  struct TFrame{
    QapAtlas*atlas;
    int x,y,w,h;
    TFrame():atlas(NULL),x(0),y(0),w(0),h(0){}
    TFrame(QapAtlas*atlas,QapDX::QapTexMem*Mem,int x,int y):atlas(atlas),x(x),y(y),w(Mem->W),h(Mem->H){}
    void Bind(QapDev*RD){atlas->Bind(RD,this);}
  };
  QapPool<TFrame>pool;
  vector<TFrame*>frames;
public:
  QapAtlas():pMem(NULL),pTex(NULL),W(2048),H(2048),X(0),Y(0),Ident(8),dY(0),pool(256){
    pMem=new QapDX::QapTexMem("Atlas.qap",W,H,new QapColor[W*H]);
  }
  TFrame*AddFrame(QapDX::QapTexMem*Mem)
  {
    if(!Mem||!Mem->pBits)return NULL;
    if(X<Ident||Ident+X+Mem->W>W)
    {
      QapAssert(W>=Mem->W);
      X=Ident;Y+=dY+Ident;dY=Mem->H;
      pMem->FillLine(Y-Ident/2,0xff000000);
    }
    {
      TFrame*pFrame;
      pool.NewInstance(pFrame);
      *pFrame=TFrame(this,Mem,X,Y);
      frames.push_back(pFrame);
      pMem->FillBorder(X,Y,Mem);
      pMem->FillMem(X,Y,Mem);
      X+=Mem->W+Ident;dY=max(dY,Mem->H);
      return pFrame;
    }
  }
  QapDX::QapTex*AddTex(QapDX::QapTexMem*Mem)
  {
    AddFrame(Mem);
    return QapDX::GenTextureMipMap(Mem);
  }
  QapDX::QapTex*GenTex(){return pTex=QapDX::GenTextureMipMap(pMem);}
  void Bind(QapDev*RD,TFrame*frame)
  {
    b2Transform xf;
    float inv_w=1.f/float(W);
    float inv_h=1.f/float(H);
    xf.p=vec2f(float(frame->x)*inv_w,float(frame->y)*inv_h);
    xf.r=MakeZoomTransform(vec2d(float(frame->w)*inv_w,float(frame->h)*inv_h)).r;
    RD->SetTextureTransform(xf);
  }
};