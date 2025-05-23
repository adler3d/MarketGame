/*------------------------------------------*/
/*   {<|author : Adler3D(Kashin Ivan) |>   }*/
/*   {<|e-mail : Adler3D@gmail.com    |>   }*/
/*   {<|site   : Adler3d.narod.ru     |>   }*/
/*------------------------------------------*/
#pragma once
/************************************************************************/
/* Подключаем всякое разное                                             */
/************************************************************************/
#include "QapSys.h"
#include "QapVec.h"
#include "QapMatrix.h"
#include "QapGDI.h"
/************************************************************************/
/* Объявления Типов                                                     */
/************************************************************************/
namespace QapDX
{
  class QapTexMem; class QapTex;
  void RegTexMem(QapTexMem* Tex);
  void UnRegTexMem(QapTexMem* Tex);
  void RegTex(QapTex* Tex);
  void UnRegTex(QapTex* Tex);
  enum BlendType
  {
    BT_NONE,
    BT_SUB,
    BT_ADD,
  };
  enum AlphaMode
  {
    AM_NONE,
    AM_GEQUAL_ONE,
    AM_NEQUAL_MAX,
  };
  class QapTexMem
  {
  public:
    QapColor*pBits;
    int W,H;
    string Name;
    int ID;
  public:
    QapTexMem(const string&name):pBits(NULL),W(0),H(0),Name(name){RegTexMem(this);};
    QapTexMem(const string&name,int w,int h,QapColor *pbits):pBits(pbits),W(w),H(h),Name(name){RegTexMem(this);};
    void SaveToFile(const string&FN);
    ~QapTexMem(){UnRegTexMem(this);delete[] pBits;};
    QapTexMem*Clone(){
      return new QapTexMem(Name+".Clone",W,H,(QapColor*)memcpy(new QapColor[W*H],pBits,sizeof(QapColor)*W*H));
    }
    QapColor get_color_at(int x,int y)const{return pBits[x+y*W];}
  public:
    class IPixelVisitor{
    public:
      virtual void Visit(const vec2i&p){}
    };
    class ITraceVisitor{
    public:
      virtual bool Visit(const vec2i&p){return true;}
      virtual bool NextWave(){return true;}
    };
    inline bool IsValid(const vec2i&p){return InDip(0,p.x,W-1)&&InDip(0,p.y,H-1);}
    inline QapColor&operator[](const vec2i&p){return pBits[p.x+p.y*W];}
    void Accept(const vec2i&p,IPixelVisitor&Visitor)
    {
      #define N(A,B)
      #define F(A,B){vec2i np=p+vec2i(A,B);if(IsValid(np))Visitor.Visit(np);};;;
      N(-1,-1)F(+0,-1)N(+1,-1)
      F(-1,+0)N(+0,+0)F(+1,+0)
      N(-1,+1)F(+0,+1)N(+1,+1)
      #undef F
      #undef N
    }
    QapTexMem*GenEdge(const vec2i&p,const QapColor&color=0xFF000000)
    {
      //15:27 06.07.2011
      QapDX::QapTexMem*pDest=Clone()->Clear(0xffffffff);
      class TraceVisitor:public ITraceVisitor{
      public:
        QapTexMem*pSrc;
        QapTexMem*pDest;
        QapColor color;
        TraceVisitor(QapTexMem*pSrc,QapTexMem*pDest,const QapColor&color):pSrc(pSrc),pDest(pDest){}
      public:
        bool Visit(const vec2i&p){
          QapColor&S=pSrc->operator[](p);
          QapColor&D=pDest->operator[](p);
          //if(&D==&S)exit(0);
          bool flag=S==color;
          if(flag)D=0xffffffff;
          if(!flag)D=0xff000000;
          return flag;
        }
      }TV(this,pDest,color);
      Accept(p,TV);
      return pDest->InvertRGB();
    }
    void Accept(const vec2i&p,ITraceVisitor&Visitor)
    {  
      class Tracer{
      public:
        vector<vec2i>NArr,DArr,Arr,IA;
        vector<int>Check;
      public:
        ITraceVisitor&Visitor;
        QapTexMem*pMem;
        Tracer(ITraceVisitor&Visitor,QapTexMem*pMem,const vec2i&p):pMem(pMem),Visitor(Visitor){NArr.push_back(p);Init();}
        void Init()
        {
          Check.resize(pMem->W*pMem->H);
          for(int i=0;i<Check.size();i++)Check[i]=1;
        }
      public:
        class PixelVisitor:public IPixelVisitor{
        public:
          Tracer*pT;
          PixelVisitor(Tracer*pT):pT(pT){}
          ~PixelVisitor(){}
          void Visit(const vec2i&p){
            vector<vec2i>*Arr[2]={&pT->DArr,&pT->NArr};
            bool Accepted=!!pT->Visitor.Visit(p);
            Arr[Accepted]->push_back(p);
          }
        };
        int GetID(const vec2i&p){return p.x+p.y*pMem->W;}
        void Run()
        {
          while(!NArr.empty()&&Visitor.NextWave())
          {
            Arr=NArr;NArr.clear();DArr.clear();
            for(int j=0;j<Arr.size();j++)
            {
              vec2i&p=Arr[j];
              int ID=GetID(p);
              if(ID<0||!Check[ID])continue;
              PixelVisitor PV(this);
              pMem->Accept(p,PV);
              Check[ID]=false;
            }
          }
        }
      } tracer(Visitor,this,p);
      tracer.NArr.push_back(p);
      tracer.Run();  
    }
  public:
    QapTexMem*FillBorder(int x,int y,QapTexMem*Source,int n=4)
    {
      if(!Source||!Source->pBits)return this;
      int sW=Source->W;
      int sH=Source->H;
      int mX=min(W,x+sW);
      int mY=min(H,y+sH);
      QapColor*pT=this->pBits;
      QapColor*pS=Source->pBits;
      #define F(i,j)pS[(i-x)+(j-y)*sW]
      for(int k=1;k<=n;k++)
      {
        {int j=00+y;if(InDip(0,j-k,H-1))for(int i=max(0,x);i<mX;i++){pT[i+(j-k)*W]=F(i,j);}}
        {int j=mY-1;if(InDip(0,j+k,H-1))for(int i=max(0,x);i<mX;i++){pT[i+(j+k)*W]=F(i,j);}}
        {int i=00+x;if(InDip(0,i-k,W-1))for(int j=max(0,y);j<mY;j++){pT[(i-k)+j*W]=F(i,j);}}
        {int i=mX-1;if(InDip(0,i+k,W-1))for(int j=max(0,y);j<mY;j++){pT[(i+k)+j*W]=F(i,j);}}
      }
      #undef F
      return this;
    }
    QapTexMem*FillMem(int x,int y,QapTexMem*Source)
    {
      if(!Source||!Source->pBits)return this;
      int sW=Source->W;
      int sH=Source->H;
      int mX=min(W,x+sW);
      int mY=min(H,y+sH);
      QapColor*pT=this->pBits;
      QapColor*pS=Source->pBits;
      for(int j=max(0,y);j<mY;j++){
        for(int i=max(0,x);i<mX;i++){
          pT[i+j*W]=pS[(i-x)+(j-y)*sW];
        }
      }
      return this;
    }
    QapTexMem*Clear(const QapColor&color=0xFF000000){
      for(int i=0;i<W*H;i++)pBits[i]=color;
       return this;
    }
    QapTexMem*FillLine(const int Line,const QapColor&Color){
      for(int i=0;i<W;i++){QapColor&pix=pBits[Line*W+i];pix=Color;}
      return this;
    }
    QapTexMem*Circle(const QapColor&color,int x,int y,int r){
      vec2i pos(x,y);
      int rr=r*r;
      int i0=max(0,x-r);
      int iz=min(W-1,x+r);
      int j0=max(0,y-r);
      int jz=min(H-1,y+r);
      for(int j=j0;j<=jz;j++)
        for(int i=i0;i<=iz;i++)
        {
          int mm=abs(r-(vec2i(i,j)-pos).SqrMag());
          if(mm<rr)
          {
            QapColor&FragColor=pBits[j*W+i];
            FragColor=color;
          }
        }
      return this;
    }
    QapTexMem*Draw(int x,int y,QapTexMem*Source,const QapColor&color){
      if(!Source||!Source->pBits)return this;
      int sW=Source->W;
      int sH=Source->H;
      int mX=min(W,x+sW);
      int mY=min(H,y+sH);
      QapColor*pT=this->pBits;
      QapColor*pS=Source->pBits;
      for(int j=max(0,y);j<mY;j++){
        for(int i=max(0,x);i<mX;i++){
          QapColor src=Source->pBits[(i-x)+(j-y)*sW]*color;
          QapColor&dest=pBits[j*W+i];
          dest=QapColor::Mix(dest,src,real(src.a)/255.0);
          //dest.a=;
        }
      }
      return this;
    }
    QapTexMem*Circle(const QapColor&color,int x,int y,int r,int hs){
      vec2i pos(x,y);
      int minr=r-hs;
      int maxr=r+hs;
      int r1=minr*minr;
      int r2=maxr*maxr;
      int i0=max(0,x-r-hs);
      int iz=min(W-1,x+r+hs);
      int j0=max(0,y-r-hs);
      int jz=min(H-1,y+r+hs);
      for(int j=j0;j<=jz;j++)
        for(int i=i0;i<=iz;i++)
        {
          int mm=abs(r-(vec2i(i,j)-pos).SqrMag());
          if(InDip(r1,mm,r2))
          {
            QapColor&FragColor=pBits[j*W+i];
            FragColor=color;
          }
        }
      return this;
    }
    QapTexMem*FillColomn(const int Colomn,const QapColor&Color){
      for(int i=0;i<H;i++)pBits[W*i+Colomn]=Color;
      return this;
    }
    QapTexMem*CalcAlphaToRGB_and_set_new_alpha(byte new_alpha=0xff){
      for(int i=0;i<W*H;i++){auto a=pBits[i].a;pBits[i]=QapColor(new_alpha,a,a,a);}
      return this;
    }
    QapTexMem*CalcAlphaToRGB_and_keep_alpha(){
      for(int i=0;i<W*H;i++){auto a=pBits[i].a;pBits[i]=QapColor(a,a,a,a);}
      return this;
    }
    QapTexMem*CalcAlpha(){
      for(int i=0;i<W*H;i++)pBits[i].a=pBits[i].GetLuminance();
      return this;
    }
    template<class FUNC>
    QapTexMem*ForEachPixel(FUNC&&func){
      for(int i=0;i<W*H;i++)func(pBits[i]);
      return this;
    }
    QapTexMem*CopyAlpha(QapTexMem*Alpha){
      if(Alpha->W!=W||Alpha->H!=H)return this;
      for(int i=0;i<W*H;i++)pBits[i].a=Alpha->pBits[i].a;
      return this;
    }
    QapTexMem*CalcAlpha(const QapColor&Color,int threshold=0){
      for(int i=0;i<W*H;i++)pBits[i].a=pBits[i]==Color?0:255;
      const QapColor&c=Color;
      int t=threshold;
      int t3=t*3;
      for(int i=0;i<W*H;i++){
        QapColor&p=pBits[i];
        #define F(r)int f##r=abs(int(p.r)-int(c.r));
        F(r);F(g);F(b);
        #undef F
        //int esqr=fr*fr+fg*fg+fb*fb;
        int sum=fr+fg+fb;
        p.a=sum>t3?255:t3?255*sum/t3:0;
      }
      return this;
    }
    QapTexMem*FillChannel(const QapColor&Source,DWORD BitMask){
      for(int i=0;i<W*H;i++){QapColor&C=pBits[i];C=(~BitMask&C)|(BitMask&Source);};
      return this;
    }
    QapTexMem*InvertRGB(){for(int i=0;i<W*H;i++){QapColor&C=pBits[i];C.r=~C.r;C.g=~C.g;C.b=~C.b;}return this;}
    Dip2i GetDipAlpha(){Dip2i Dip(256,-1);for(int i=0;i<W*H;i++)Dip.Take(pBits[i].a);return Dip;}
    Dip2i GetDipR(){Dip2i Dip(256,-1);for(int i=0;i<W*H;i++)Dip.Take(pBits[i].r);return Dip;}
    Dip2i GetDipG(){Dip2i Dip(256,-1);for(int i=0;i<W*H;i++)Dip.Take(pBits[i].g);return Dip;}
    Dip2i GetDipB(){Dip2i Dip(256,-1);for(int i=0;i<W*H;i++)Dip.Take(pBits[i].b);return Dip;}
    QapTexMem*NormRGB(){
      typedef Dip2i::Transform D2iXF;
      Dip2i Cur[4]={GetDipR(),GetDipG(),GetDipB(),GetDipAlpha()};
      Dip2i cur(256,-1);for(int i=0;i<4;i++){cur.Take(Cur[i].a);cur.Take(Cur[i].b);}
      Dip2i norm(0,255);
      D2iXF xf(cur,norm);
      D2iXF arr[4]={D2iXF(Cur[0],norm),D2iXF(Cur[1],norm),D2iXF(Cur[2],norm),D2iXF(Cur[3],norm)};
      for(int i=0;i<W*H;i++){
        QapColor&C=pBits[i];
        C.r=byte(xf*C.r);
        C.g=byte(xf*C.g);
        C.b=byte(xf*C.b);
      }
      return this;
    }
    QapTexMem*NormAlpha(){
      Dip2i Cur=GetDipAlpha();
      Dip2i Norm(0,255);
      Dip2i::Transform xf(Cur,Norm);
      for(int i=0;i<W*H;i++)
      {
        QapColor&C=pBits[i];
        C.a=byte(xf*C.a);
      }
      return this;
    }
    QapTexMem*InvertY(){
      QapColor*line=new QapColor[W];
      size_t Size=sizeof(QapColor)*W;
      for(int i=0;i<H/2;i++)
      {
        int a(W*(i)),b(W*(H-i-1));
        memcpy(line,&pBits[a],Size);
        memcpy(&pBits[a],&pBits[b],Size);
        memcpy(&pBits[b],line,Size);
      }
      delete[] line;
      return this;
    }
    QapTexMem*InvertX(){
      for(int i=0;i<H;i++)
      {
        QapColor*line=&pBits[i];
        for(int j=0;j<W/2;j++)
        {
          int a(i),b(H-i-1);
          std::swap(line[a],line[b]);
        }
      }
      return this;
    }
    static float gauss(float x,float sigma)
    {
      float x_sqr=x*x;
      float sigma_sqr=sigma*sigma;
      float sqrt_value=1.0/sqrt(2.0*M_PI*sigma_sqr);
      float exp_value=exp(-x_sqr/(2.0*sigma_sqr));
      return sqrt_value*exp_value;
    }
    static void PascalRow(IntArray&arr,int n)
    {
      arr.resize(n+1);
      for(int i=0;i<arr.size();i++)arr[i]=0;
      int*c=&arr[0];
      c[0]=1;
      for(int j=1;j<n;j++)for(int i=j;i>=1;i--)c[i]=c[i-1]+c[i];
    }
    private:
      inline vec4f tex2Df(vec4f*C,const vec2i&uv){return C[Clamp(uv.x,int(0),int(W-1))+W*Clamp(uv.y,int(0),int(H-1))];}
      inline vec4f tex2D(QapColor*C,const vec2i&uv){return vec4f(C[Clamp(uv.x,int(0),int(W-1))+W*Clamp(uv.y,int(0),int(H-1))]);}
      inline vec4i tex2Di(QapColor*C,const vec2i&uv){return vec4i(C[Clamp(uv.x,int(0),int(W-1))+W*Clamp(uv.y,int(0),int(H-1))]);}/*
      void DirBlur(QapColor*SysMem,float radius,const vec2i&texrad){
        IntArray garr;
        int r=int(radius*2)+1;
        if(!r)return;
        PascalRow(garr,r);
        int*g=&garr[r];
        for(int j=0;j<H;j++)
          for(int i=0;i<W;i++)
          {
            QapColor&FragColor=pBits[j*W+i];
            vec2i texcoord(i,j);
            int sum=0.0;
            int x=1;
            vec4i value(0,0,0,0);
            for(int x=1;x<=r;x++)
            {
              int currentScale=g[r-x-1];//gauss(x,sigma);
              sum+=2.0*currentScale;
              vec2i dudv=x*texrad;
              value+=(tex2Di(SysMem,texcoord-dudv)+tex2Di(SysMem,texcoord+dudv))*currentScale;
            }
            sum+=g[r];
            value+=tex2Di(SysMem,texcoord)*g[r];
            FragColor=(value*(255/sum)).GetColor();
          }
      }*/
      void DirBlur(vec4f*SysMem,vec4f*SysOut,float radius,const vec2i&texrad){
        float sigma=radius/3.0;
        vector<float> garr;
        for(int x=0;x<=radius;x++){
          garr.push_back(gauss(float(x),sigma));
        }
        float*g=&garr[0];
        for(int j=0;j<H;j++)
          for(int i=0;i<W;i++)
          {
            vec4f&FragColor=SysOut[j*W+i];
            vec2i texcoord(i,j);
            float sum=0.0;
            int x=1;
            vec4f value(0,0,0,0);
            for(int x=-radius;x<+radius-1;x++)if(x)
            {
              float currentScale=g[abs(x)];
              sum+=currentScale;
              vec2i dudv=x*texrad;
              value+=currentScale*tex2Df(SysMem,texcoord+dudv);
            }
            value+=(1.f-sum)*tex2Df(SysMem,texcoord);
            FragColor=value;
          }
      }
      //inline static QapColor&tex2D(QapColor*C,const vec2i&uv){return C[Clamp(uv.x,0,W-1)+W*Clamp(uv.y,0,H-1)];}
    public:
    QapTexMem*Blur(const float&radius)
    {
      static vec4f SysMem[1024*1024*4];
      vec4f*tmp=&SysMem[W*H];
      for(int i=0;i<W*H;i++)SysMem[i]=vec4f(pBits[i].b,pBits[i].g,pBits[i].r,pBits[i].a);
      //memcpy_s(SysMem,sizeof(SysMem),pBits,W*H*sizeof(QapColor));//copy image
      DirBlur(SysMem,tmp,radius,vec2i(1,0));
      DirBlur(tmp,SysMem,radius,vec2i(0,1));
      for(int i=0;i<W*H;i++)pBits[i]=QapColor(SysMem[i].a,SysMem[i].r,SysMem[i].g,SysMem[i].b);
      return this;
    }
  };
  class QapTex{
  public:
    IDirect3DTexture9*Tex;
    int W,H;
    string Name;
    int ID;
  public:
    QapTex(QapTexMem*TexMem,IDirect3DTexture9*Tex):Tex(Tex){W=TexMem->W;H=TexMem->H;Name=TexMem->Name;RegTex(this);};
    ~QapTex(){
      UnRegTex(this);
      Tex->Release();
      Tex=0;
    };
    //void Bind(int lvl=0){Sys.pDev->SetTexture(lvl,Tex);};
  };
  class IResource
  {
  public:
    virtual void OnLost()=0;
    virtual void OnReset()=0;
    static IResource* Reg(IResource *A);
    static IResource* UnReg(IResource *A);
  };
  class QapBitmapInfo{
  public:
    BITMAPINFO BI;
    BITMAPINFOHEADER&BH;
  public:
    QapBitmapInfo(int W,int H):BH(BI.bmiHeader){
      ZeroMemory(&BI,sizeof(BI));
      BH.biSize=sizeof(BI.bmiHeader);
      BH.biWidth=W;BH.biHeight=H;
      BH.biPlanes=1;BH.biBitCount=32;
      BH.biSizeImage=W*H*4;
    }
  };
  struct QapFont
  {
    QapTex*Tex;
    int W[256],H[256],Size;
    QapFont():Tex(NULL),Size(0){};
    QapFont(QapTex*Tex,int Size):Tex(Tex),Size(Size){};
    ~QapFont(){
      if(Tex)delete Tex;
    };
    void Transmit(QapFont&ref)
    {
      if(Tex)delete Tex;
      Tex=ref.Tex;ref.Tex=NULL;
      for(int i=0;i<256;i++)W[i]=ref.W[i];
      for(int i=0;i<256;i++)H[i]=ref.H[i];
      Size=ref.Size;
    }
    QapFont(QapFont&ref):Tex(NULL),Size(0){Transmit(ref);}
    QapFont&operator=(QapFont&ref){Transmit(ref);return *this;}
    QapTexMem*CreateFontMem(string Name,int Size,bool Bold,int TexSize)
    {
      QapColor*pix=new QapColor[TexSize*TexSize];
      this->Size=TexSize;
      {
        HDC DC=GetDC(Sys.hWnd);
        int W=Bold?FW_BOLD:FW_NORMAL;
        int H=-MulDiv(Size,GetDeviceCaps(DC,LOGPIXELSY),72);
        {
          HFONT FNT=CreateFontA(H,0,0,0,W,0,0,0,RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH,Name.c_str());
          {
            HDC MDC=CreateCompatibleDC(DC);
            {
              HBITMAP BMP=CreateCompatibleBitmap(DC,TexSize,TexSize);
              SelectObject(MDC,BMP);
              RECT Rect; SetRect(&Rect,0,0,TexSize,TexSize);
              FillRect(MDC,&Rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
              SelectObject(MDC,FNT);
              SetBkMode(MDC,TRANSPARENT);
              SetTextColor(MDC,0xFFFFFF);
              for(int i=0;i<256;i++)TextOutA(MDC,i%16*(TexSize/16),i/16*(TexSize/16),(LPCSTR)&i,1);
              {QapBitmapInfo QBI(TexSize,TexSize);GetDIBits(MDC,BMP,0,TexSize,pix,&QBI.BI,DIB_RGB_COLORS);}
              for(int i=0;i<TexSize*TexSize;i++){pix[i].a=pix[i].r;pix[i].r=255;pix[i].g=255;pix[i].b=255;}
              for(int i=0;i<256;i++){SIZE cs;GetTextExtentPoint32A(MDC,(LPCSTR)&i,1,&cs);this->W[i]=cs.cx;this->H[i]=cs.cy;}
              DeleteObject(BMP);
            }
            DeleteDC(MDC);
          }
          DeleteObject(FNT); 
        }
        ReleaseDC(Sys.hWnd,DC);
      }
      QapTexMem*pMem=new QapTexMem("Font_"+Name+"_"+IToS(TexSize),TexSize,TexSize,(QapColor*)pix);
      return pMem;
    }
  };
  const int MAX_TEXT_LENGTH=2048;
  extern QapColor Color;
}
/************************************************************************/
/* Переменные                                                           */
/************************************************************************/
extern QapSys Sys;
/************************************************************************/
/* Объявления                                                           */
/************************************************************************/
namespace QapDX
{
  void OrthoLH(D3DMATRIX *pOut,float w,float h,float zn,float zf);
  void Set2D(vec2d CamPos=vec2d(),real zoom=1,real ang=0);
  void Clear2d(const QapColor&Color=QapColor(255,0,0,0)); 
  void ResetStates();
  bool BeginScene();
  bool EndScene();
  bool Present();
  //bool BeginScene2D();bool EndScene2D();
  void DrawMouse(vec2d mp,real ls=1,real zoom=0.15f);
  void DrawQuad(float x,float y,float w, float h, float a);  
  void RenderLine(float aX,float aY,float bX, float bY);
  void DrawPointList(PointArray const&PA,real R);
  void DrawTexPointList(PointArray const&PA,real R);
  void DrawTexColorPointList(PointArray const&PA,real R,ColorArray const&ColorArr);
  void DrawConvex(PointArray &PA);
  void DrawPolyLine(PointArray const&PA,real ls=0.5,bool loop=false);
  void DrawLineList(PointArray const&PA,real ls=0.5);
  void DrawLine(vec2d a,vec2d b,float h=1);
  void DrawRib(vec2d a,vec2d b,float h=1);// - не используется
  void DrawCircle(vec2d o,real r,real ang,real tz,int seg=4);
  void DrawCircleEx(vec2d pos,real r,real ang,real ls,int seg=4);
  void DrawMesh(PointArray const&PA,IntArray const&IA);
  void DrawTexMesh(PointArray const&PA,IntArray const&IA,PointArray const&TA);
  void DrawTexColorMesh(PointArray const&PA,IntArray const&IA,PointArray const&TA,ColorArray const&CA);
  void RenderTrigon(vec2d &a,vec2d &b,vec2d &c,real w,real h);
  void RenderTrigonTex(vec2d *A,real w,real h);
  void Blend(BlendType Mode);
  void Alpha(AlphaMode Mode);
  void SetColor(QapColor Color);
  void PrintText(QapDX::QapFont Font,float X,float Y,string Text);
  void DrawText(QapDX::QapFont&Font,float X,float Y,string&Text);
  int GetQ3TextLength(const QapDX::QapFont&Font,const string &Text);
  string Q3TextToNormal(const string&Text);
  void UnloadTextures();
  bool BindTex(QapTex *Tex,int level=0);
  QapFont FontCreate(string Name, int Size, bool Bold, int TexSize);
  void FreeFont(QapDX::QapFont&Font);
  QapTexMem*LoadTexture(const string&FN);
  QapTexMem*GenBall(int TexSize);
  QapTexMem*GenQuad(int TexSize);
  QapTexMem*GenQuadSL(int TexSize);
  QapTexMem*GenLattice(int TexSize);
  QapTexMem*GenBigLogo(string Text,string FontName,int FontSize,bool Bold,int W,int H);
  QapTex*GenTextureMipMap(QapTexMem*&Tex,int MaxLevelCount=16);
  QapTexMem*BlurTexture(QapTexMem*Tex,int PassCount=1);
}
/************************************************************************/
/* Конец                                                                */
/************************************************************************/
