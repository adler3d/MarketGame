#pragma once
//-------------------------------------------//
//   {<| author : Adler3D(Kashin Ivan) |>}   //
//   {<| e-mail : Adler3D@gmail.com    |>}   //
//   {<| site   : Adler3d.narod.ru     |>}   //
//-------------------------------------------//
#include "QapSys.h"
#include "QapVec.h"
#include "QapMatrix.h"
#include "QapDX.h"
//-------------------------------------------//

class QapMat22
{
public:
  vec2f col1;
  vec2f col2;
public:
  QapMat22():col1(1,0),col2(0,1) {}
  QapMat22(const vec2f&c1,const vec2f&c2)
  {
    col1=c1;
    col2=c2;
  }
  QapMat22(float a11,float a12,float a21,float a22)
  {
    col1.x=a11;
    col1.y=a21;
    col2.x=a12;
    col2.y=a22;
  }
  explicit QapMat22(float ang)
  {
    float c=cosf(ang);
    float s=sinf(ang);
    col1.x=c;
    col2.x=-s;
    col1.y=s;
    col2.y=+c;
  }
  void set(const vec2f&c1,const vec2f&c2)
  {
    col1=c1;
    col2=c2;
  }
  void set(float ang)
  {
    float c=cosf(ang);
    float s=sinf(ang);
    col1.x=c;
    col2.x=-s;
    col1.y=s;
    col2.y=+c;
  }
  void set_ident()
  {
    col1.x = 1.0f;
    col2.x = 0.0f;
    col1.y = 0.0f;
    col2.y = 1.0f;
  }
  void set_zero()
  {
    col1.x = 0.0f;
    col2.x = 0.0f;
    col1.y = 0.0f;
    col2.y = 0.0f;
  }
  float GetAngle()const
  {
    return atan2(col1.y, col1.x);
  }
  void mul(real r){col1*=r;col2*=r;}
};
class transform2f
{
public:
  vec2f p;
  QapMat22 r;
public:
  transform2f() {}
  transform2f(const vec2f&p,const QapMat22&r):p(p),r(r) {}
  explicit transform2f(const vec2f&p):p(p) {}
  void set_ident()
  {
    p.set_zero();
    r.set_ident();
  }
  void set(const vec2d&p,float ang)
  {
    this->p=p;
    this->r.set(ang);
  }
  float getAng()const
  {
    return atan2(r.col1.y,r.col1.x);
  }
public:
  friend vec2f operator*(const transform2f&T,const vec2f&v)
  {
    float x=(+T.r.col1.x*v.x+T.r.col2.x*v.y)+T.p.x;
    float y=(+T.r.col1.y*v.x+T.r.col2.y*v.y)+T.p.y;
    return vec2f(x,y);
  }
};
  typedef transform2f b2Transform;
  
inline transform2f MakeZoomTransform(const vec2d&zoom)
{
  transform2f tmp(vec2f(0,0),QapMat22(vec2f(zoom.x,0.f),vec2f(0.f,zoom.y)));
  return tmp;
}
class QapDev
{
public:
  class TDynamicResource:public QapDX::IResource
  {
  public:
    TDynamicResource():Owner(NULL),flag(false){QapDX::IResource::Reg(this);};
    ~TDynamicResource(){QapDX::IResource::UnReg(this);};
  public:
    QapDev *Owner;
    bool flag;
    void OnLost(){flag=Owner->VB;if(Owner)Owner->Free();}
    void OnReset(){if(Owner&&flag)Owner->ReInit();}
  };
  TDynamicResource DynRes;
public:
  #define ADDVAR(TYPE,NAME,VALUE)TYPE NAME;
    ADDVAR(QapColor,color,0);
  #undef ADDVAR
public:
  struct Ver
  {
    float x,y,z;
    QapColor color;
    float tu,tv;
    Ver():x(0),y(0),z(0),tu(0),tv(0) {}
    Ver(float x,float y,const QapColor&color,float u=0.0,float v=0.0):x(x),y(y),z(0),color(color),tu(u),tv(v) {}
    Ver(const vec2f&pos,const QapColor&color,float u=0.0,float v=0.0):x(pos.x),y(pos.y),z(0),color(color),tu(u),tv(v) {}
    Ver(const vec2f&pos,const QapColor&color,const vec2f&texcoord):x(pos.x),y(pos.y),z(0),color(color),tu(texcoord.x),tv(texcoord.y) {}
    vec3f&get_vec3f()const{
      return *(vec3f*)&x;
    }
    vec2f&get_pos()
    {
      return *(vec2f*)&x;
    }
    vec2f&get_pos()const
    {
      return *(vec2f*)&x;
    }
    vec2f&get_tpos()
    {
      return *(vec2f*)&tu;
    }
    vec2f&get_tpos()const
    {
      return *(vec2f*)&tu;
    }
  };
  struct BatchScope{
    QapDev&RD;
    bool flag=false;
    BatchScope(QapDev&RD):RD(RD){flag=!RD.Batching;if(flag)RD.BeginBatch();}
    ~BatchScope(){if(flag)RD.EndBatch();}
  };
public:
  IDirect3DVertexBuffer9 *VB;
  IDirect3DIndexBuffer9 *IB;
  Ver *VBA;
  int *IBA;
  int VPos;
  int IPos;
  int MaxVPos;
  int MaxIPos;
  int DIPs;
  int Verts;
  int Tris;
  QapDX::BlendType BlendMode;
  QapDX::AlphaMode AlphaMode;
  bool Batching;
  bool Textured;
  b2Transform xf,txf;
  static const DWORD FVF=D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
public:
  QapDev():color(0xFFFFFFFF),VB(NULL),IB(NULL),VBA(NULL),IBA(NULL),VPos(0),IPos(0),MaxVPos(0),MaxIPos(0),Batching(false),BlendMode(QapDX::BT_SUB),AlphaMode(QapDX::AM_NONE){DynRes.Owner=this;}
  ~QapDev(){Free();}
public:
  void ReInit(){Init(MaxVPos,MaxIPos);}
  void Init(int VCount,int ICount)
  {
    if(!VB)
    {
      MaxVPos=VCount; MaxIPos=ICount;
      Sys.pDev->CreateVertexBuffer(VCount*sizeof(Ver),D3DUSAGE_DYNAMIC,FVF,D3DPOOL_DEFAULT,&VB,NULL);
      Sys.pDev->CreateIndexBuffer(ICount*sizeof(int),D3DUSAGE_DYNAMIC,D3DFMT_INDEX32,D3DPOOL_DEFAULT,&IB,NULL);
      VBA=0; IBA=0; VPos=0; IPos=0; DIPs=0; Verts=0; Tris=0;
      xf.set_ident();
      txf.set_ident();
    };
  }
  void Free(){
    if(VB)VB->Release();VB=NULL;if(IB)IB->Release();IB=NULL;VPos=0;IPos=0;Batching=false;BlendMode=QapDX::BT_SUB;AlphaMode=QapDX::AM_NONE;
  };
public:
  void QapDev::BeginBatch()
  {
    Batching=true;Textured=true;
    VBA=0;IBA=0;VPos=0;IPos=0;//Test it
    if(!IB||!VB)return;
    IB->Lock(0,sizeof(int)*MaxIPos,(void **)&IBA,0);
    VB->Lock(0,sizeof(Ver)*MaxVPos,(void **)&VBA,0);
  };
  void EndBatch()
  {
    Batching=false;
    if(!IB||!VB)return;
    IB->Unlock(); VB->Unlock();  
    DIP();
    VBA=0;IBA=0;
  }
  void DIP()
  {
    Sys.pDev->SetFVF(FVF);
    Sys.pDev->SetStreamSource(0,VB,0,sizeof(Ver));
    Sys.pDev->SetIndices(IB);
    Sys.pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,VPos,0,IPos/3);
    DIPs++;Verts+=VPos;Tris+=IPos/3;
  }
  bool IsBatching(){return Batching;}
  int GetIPos(){return IPos;}
  int GetVPos(){return VPos;}
  int GetDIPs(){return DIPs;}
  int GetVerts(){return Verts;}
  int GetTris(){return Tris;}
  const QapColor&GetColor(){return color;}
  void NextFrame(){DIPs=0;Verts=0;Tris=0;SetBlendMode(BlendMode);SetAlphaMode(AlphaMode);}
public:
  void HackMode(bool Textured){this->Textured=Textured;}
  virtual void BindTex(int Stage,QapDX::QapTex*Tex){Sys.pDev->SetTexture(Stage,Tex?Tex->Tex:NULL);txf.set_ident();}
public:
  inline Ver&AddVertexRaw(){return VBA[VPos++];}
  inline int AddVertex(const Ver&Source)
  {
    Ver&Dest=VBA[VPos];
    Dest=Source;
    if(1){auto&v=Dest.get_pos();v=xf*v;}
    if(Textured){auto&v=Dest.get_tpos();v=txf*v;}
    return VPos++;
  }
  inline void AddTris(int A,int B,int C)
  {
    IBA[IPos++]=A;
    IBA[IPos++]=B;
    IBA[IPos++]=C;
  };
public:
  inline void SetColor(const QapColor&C){color=C;QapDX::SetColor(C);}
  inline void SetTransform(b2Transform const&val){xf=val;}
  inline b2Transform GetTransform(){return xf;}
  inline void SetTextureTransform(b2Transform const&val){txf=val;}
  inline b2Transform GetTextureTransform(){return txf;}
  inline real GetZoom(){return vec2d(xf.r.col1).Mag();}
public:
  QapDX::BlendType GetBlendMode(){return BlendMode;}
  QapDX::AlphaMode GetAlphaMode(){return AlphaMode;}
  void SetBlendMode(QapDX::BlendType Mode){QapDX::Blend(BlendMode=Mode);}
  void SetAlphaMode(QapDX::AlphaMode Mode){QapDX::Alpha(AlphaMode=Mode);}
public:
  inline int AddVertex(float x,float y,const QapColor&c,float u,float v)
  {
    QapDev::Ver tmp;
    tmp.x=x;
    tmp.y=y;
    tmp.z=0;
    tmp.color=c;
    tmp.tu=u;
    tmp.tv=v;
    auto id=AddVertex(tmp);
    return id;
  }
  inline int AddVertex(const vec2f&pos,const QapColor&c,float u,float v)
  {
    QapDev::Ver tmp;
    tmp.get_pos()=pos;
    tmp.z=0;
    tmp.color=c;
    tmp.tu=u;
    tmp.tv=v;
    auto id=AddVertex(tmp);
    return id;
  }
  inline int AddVertex(const vec2f&pos,const QapColor&c,const vec2f&tpos)
  {
    QapDev::Ver tmp;
    tmp.get_pos()=pos;
    tmp.z=0;
    tmp.color=c;
    tmp.get_tpos()=tpos;
    auto id=AddVertex(tmp);
    return id;
  }
public:
  void DrawCircleOld(const vec2d&pos,real r,real ang,real ls,int seg)
  {
    static PointArray PA;
    PA.resize(seg);
    for(int i=0;i<seg;i++)
    {
      vec2d v=Vec2dEx((real)i/(real)seg*2.0*Pi,r);
      PA[i]=pos+v;
    };
    DrawPolyLine(PA,ls,true);
  }
  void DrawCircle(const vec2d&pos,real r,real ang,real ls,int seg)
  {
    DrawCircleEx(pos,r-ls*0.5,r+ls*0.5,seg,ang);
  }
  void DrawCircleEx(const vec2d&pos,real r0,real r1,int seg,real ang)
  {
    int n=seg;
    if(n<=0)return;
    BatchScope Scope(*this);
    static vector<int> VID;
    VID.resize(n*2);
    for(int i=0;i<n;i++)
    {
      vec2d v=Vec2dEx(ang+Pi2*(real(i)/real(n)),1);
      VID[0+i]=AddVertex(pos+v*r0,color,0,0);
      VID[n+i]=AddVertex(pos+v*r1,color,0,0);
    }
    for(int i=0;i<n;i++)
    {
      int a=VID[0+(i+0)%n];
      int b=VID[0+(i+1)%n];
      int c=VID[n+(i+0)%n];
      int d=VID[n+(i+1)%n];
      AddTris(a,b,d);
      AddTris(d,c,a);
    }
  }
public:
  void DrawQuad(float x,float y,float w, float h)
  {  
    BatchScope Scope(*this);
    {
      #define F(X,Y,U,V){AddVertex(Ver(x+(X)*w,y+(Y)*h,color,U,V));}
      vec2d O(x,y);
      int n=VPos;
      F(-0.5f,-0.5f,0.0f,1.0f);
      F(+0.5f,-0.5f,1.0f,1.0f);
      F(+0.5f,+0.5f,1.0f,0.0f);
      F(-0.5f,+0.5f,0.0f,0.0f); 
      AddTris(n+1,n+0,n+3);
      AddTris(n+3,n+2,n+1);
      #undef F
    }
  }
  void DrawQuad(float x,float y,float w, float h, float a)
  {
    BatchScope Scope(*this);
    {
      #define F(X,Y,U,V){AddVertex(Ver(O+vec2d(X*w,Y*h).UnRot(OZ),color,U,V));}
      vec2d OZ=Vec2dEx(a,1.0);
      vec2d O(x,y);
      int n=VPos;
      F(-0.5f,-0.5f,0.0f,1.0f);
      F(+0.5f,-0.5f,1.0f,1.0f);
      F(+0.5f,+0.5f,1.0f,0.0f);
      F(-0.5f,+0.5f,0.0f,0.0f); 
      AddTris(n+1,n+0,n+3);
      AddTris(n+3,n+2,n+1);
      #undef F
    }
  }
  void DrawTrigon(const vec2d&A,const vec2d&B,const vec2d&C)
  {
    BatchScope Scope(*this);
    {
      AddTris(
        AddVertex(Ver(A,color)),
        AddVertex(Ver(B,color)),
        AddVertex(Ver(C,color))
      );
    }
  }
  void DrawConvex(const vector<vec2d>&Points)
  {
    BatchScope Scope(*this);
    {
      if(Points.empty())return;
      int c=Points.size();
      int n=VPos;
      for(int i=0;i<c;i++)AddVertex(Ver(Points[i],color,0.5,0.5));
      for(int i=2;i<c;i++)AddTris(n,n+i-1,n+i-0);
    }
  }
public:
  template<typename TYPE>
  void DrawPolyLine(const vector<TYPE>&PA,const real&LineSize,const bool&Loop)
  {
    if(PA.empty())return;
    BatchScope Scope(*this);
    {
      int Count=PA.size();
      int c=Loop?Count:Count-1;
      for(int i=0;i<c;i++)
      {
        TYPE const&a=PA[(i+0)%Count];
        TYPE const&b=PA[(i+1)%Count];
        TYPE n=vec2d(b-a).Ort().SetMag(LineSize);
        int A[4]={
        #define F(pos)AddVertex(Ver(pos,color,0.5f,0.5f))
          F(a+n),
          F(b-n),
          F(a-n),
          F(b+n),
        #undef F
        };
        AddTris(A[0],A[1],A[2]);
        AddTris(A[0],A[1],A[3]);
      }
    }
  }
  template<typename TYPE>
  void DrawLineList(const vector<TYPE>&PA,const real&LineSize)
  {
    if(PA.empty())return;
    BatchScope Scope(*this);
    {
      int Count=PA.size();
      for(int i=0;i<Count;i+=2)
      {
        TYPE const&a=PA[i+0];
        TYPE const&b=PA[i+1];
        TYPE n=vec2d(b-a).Ort().SetMag(LineSize);
        int A[4]={
        #define F(A)AddVertex(Ver(A,color,0.5,0.5))
          F(a+n),
          F(b-n),
          F(a-n),
          F(b+n),
        #undef F
        };
        AddTris(A[0],A[1],A[2]);
        AddTris(A[0],A[1],A[3]);
      }
    }
  }
  template<typename TYPE>
  void DrawMesh(const vector<TYPE>&VA,const vector<int>&IA)
  {
    if(VA.empty())return;
    BatchScope Scope(*this);
    {
      int base=GetVPos();
      static IntArray VID;VID.resize(VA.size());
      for(int i=0;i<VA.size();i++)VID[i]=AddVertex(MakeVer(VA[i],color,p.x,p.y));
      for(int i=0;i<IA.size();i+=3)AddTris(VID[IA[i+0]],VID[IA[i+1]],VID[IA[i+2]]);
    }
  }
};
//-------------------------------------------//
//   {<|          01.07.2011           |>}   //
//-------------------------------------------//