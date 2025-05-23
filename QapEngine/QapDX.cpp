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
#include "QapDX.h"
#undef min
#undef max
#include <algorithm>
template<class A,class B>inline static auto max(const A&a,const B&b){return a>b?a:b;}
template<class A,class B>inline static auto min(const A&a,const B&b){return a<b?a:b;}
#include <assert.h>
/************************************************************************/
/* Переменные                                                           */
/************************************************************************/
namespace QapDX
{
  QapColor Color=D3DCOLOR_XRGB(255,255,255);
  vector<QapTexMem*> ListTexMem; int LTMCountID=0;
  vector<QapTex*> ListTex; int LTCountID=0;
  vector<QapDX::IResource*> ResList;
  //vector<QapTexRT*> ListTexRT; int LTRTCountID=0;
  void OrthoLH(D3DMATRIX *pOut,float W,float H,float zn,float zf);
  IResource* IResource::Reg(IResource *A){ResList.push_back(A); return A;};
  IResource* IResource::UnReg(IResource *A){for(int i=0;i<ResList.size();i++)if(ResList[i]==A){std::swap(ResList[i],ResList.back());ResList.pop_back();return A;};return NULL;};
}
using QapDX::QapTexMem;using QapDX::QapTex;
/************************************************************************/
/* Реализация                                                           */
/************************************************************************/
void QapDX::UnloadTextures()
{
  for(int i=ListTex.size();i;i--)delete ListTex.back();
  if(ListTex.size()!=0)MACRO_ADD_LOG("Internal error",lml_WARNING);
}
void QapDX::RegTexMem(QapTexMem* Tex)
{
  Tex->ID=LTMCountID; ListTexMem.push_back(Tex);LTMCountID++;
  //MACRO_ADD_LOG("Init #"+IToS(Tex->ID)+" \""+Tex->Name+"\"",lml_HINT);
};
void QapDX::UnRegTexMem(QapTexMem* Tex)
{
  for(int i=0;i<ListTexMem.size();i++)
    if(ListTexMem[i]==Tex)
    {
      //MACRO_ADD_LOG("Free #"+IToS(Tex->ID)+" \""+Tex->Name+"\"",lml_HINT);
      ListTexMem[i]=ListTexMem.back();ListTexMem.pop_back();
    };
};
void QapDX::RegTex(QapTex* Tex)
{
  Tex->ID=LTCountID;ListTex.push_back(Tex);LTCountID++;
  MACRO_ADD_LOG("Init #"+IToS(Tex->ID)+" \""+Tex->Name+"\"",lml_EVENT);
};
void QapDX::UnRegTex(QapTex* Tex){
  if(!Tex)MACRO_ADD_LOG("Invalid texture pointer",lml_WARNING);
  for(int i=0;i<QapDX::ListTex.size();i++)
    if(QapDX::ListTex[i]==Tex)
    {
      MACRO_ADD_LOG("Free #"+IToS(Tex->ID)+" \""+Tex->Name+"\"",lml_EVENT);
      QapDX::ListTex[i]=QapDX::ListTex.back();QapDX::ListTex.pop_back();
      return;
    };
  MACRO_ADD_LOG("Texture not found #"+IToS(Tex->ID)+" \""+Tex->Name+"\"",lml_ERROR);
};
bool QapDX::BindTex(QapTex *Tex,int level){
  if(!Tex||!Tex->Tex||Tex->Name.empty())goto fail;
  Sys.pDev->SetTexture(level,Tex->Tex);
  return true;
  fail:Sys.pDev->SetTexture(level,0);return false;
}
void QapDX::OrthoLH(D3DMATRIX *pOut,float w,float h,float zn,float zf)
{
  QapMat4 mat=QapMat4(
      2.0/w,0,0,0,
      0,2.0/h,0,0,
      0,0,1/(zf-zn),0,
      0,0,-zn/(zf-zn),1
    );
  //mat=Transpose(mat);
  (*pOut)=mat*Identity4();
}

void QapDX::Set2D(vec2d CamPos,real zoom,real ang)
{
  static QapMat4 matProj(Identity4());
  static QapMat4 matView(Identity4());
  static QapMat4 matWorld(Translate(-CamPos.x,-CamPos.y,0)*Scale(zoom,zoom,1.0)*RotateZ(ang));
  QapDX::OrthoLH(&matProj,float(Sys.SM.W),float(Sys.SM.H),-1.0,1.0);
  Sys.pDev->SetTransform(D3DTS_PROJECTION,&matProj); 
  Sys.pDev->SetTransform(D3DTS_VIEW,&matView);
  Sys.pDev->SetTransform(D3DTS_WORLD,&matWorld);
}

void QapDX::Clear2d(const QapColor&Color)
{
  Sys.pDev->Clear(0,NULL,D3DCLEAR_TARGET,Color,1.0f,0);
}

void QapDX::ResetStates()
{
  Sys.pDev->SetRenderState(D3DRS_LIGHTING, false);
  Sys.pDev->SetRenderState(D3DRS_ZENABLE, false);
  Sys.pDev->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
  Sys.pDev->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
  Sys.pDev->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
  //Sys.pDev->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
  //Sys.pDev->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
  Sys.pDev->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
  Sys.pDev->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
  Sys.pDev->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
}

namespace QapDX_Hide
{
  static bool Lost=false;
  void OnLost()
  {
    MACRO_ADD_LOG("Count = "+IToS(QapDX::ResList.size()),lml_EVENT);
    Lost=true;
    for(int i=0;i<QapDX::ResList.size();i++)QapDX::ResList[i]->OnLost();   
  }
  void OnReset()
  {
    MACRO_ADD_LOG("Count = "+IToS(QapDX::ResList.size()),lml_EVENT);
    Lost=false;
    for(int i=0;i<QapDX::ResList.size();i++)QapDX::ResList[i]->OnReset();
  }
};

bool QapDX::BeginScene()
{
  if(QapDX_Hide::Lost){EndScene();return false;};
  Sys.pDev->BeginScene(); QapDX::ResetStates();
  return true;
  //Sys.pDev->BeginStateBlock();IDirect3DStateBlock9 *SB;Sys.pDev->EndStateBlock(&SB);
}

bool QapDX::EndScene()
{
  if(QapDX_Hide::Lost){
    HRESULT hr=Sys.pDev->TestCooperativeLevel();
    if(hr!=D3DERR_DEVICENOTRESET)return false;
    D3DPRESENT_PARAMETERS pp=Sys.GetPresentParameters(); Sys.pDev->Reset(&pp);
    QapDX_Hide::OnReset();
     return false;
  };
  Sys.pDev->EndScene(); 
  return !QapDX_Hide::Lost;
}

bool QapDX::Present()
{
  if(!QapDX_Hide::Lost)
  {
    if(QapDX_Hide::Lost=D3DERR_DEVICELOST==Sys.pDev->Present(NULL,NULL,NULL,NULL))QapDX_Hide::OnLost();
  }
  return !QapDX_Hide::Lost;
}

/*
bool QapDX::BeginScene2D()
{
  if(QapDX_Hide::Lost){EndScene2D();return false;};
  QapDX::Set2D(); QapDX::Clear2d(); Sys.pDev->BeginScene(); QapDX::ResetStates();
  return true;
  //Sys.pDev->BeginStateBlock();IDirect3DStateBlock9 *SB;Sys.pDev->EndStateBlock(&SB);
}

bool QapDX::EndScene2D()
{
  if(QapDX_Hide::Lost){
    HRESULT hr=Sys.pDev->TestCooperativeLevel();
    if(hr!=D3DERR_DEVICENOTRESET)return false;
    D3DPRESENT_PARAMETERS pp=Sys.GetPresentParameters(); Sys.pDev->Reset(&pp);
    QapDX_Hide::Lost=false; return false;
  };
  Sys.pDev->EndScene(); 
  QapDX_Hide::Lost=D3DERR_DEVICELOST==Sys.pDev->Present(NULL,NULL,NULL,NULL);
  return !QapDX_Hide::Lost;
}*/

void QapDX::DrawMouse(vec2d mp,real ls,real zoom)
{
  Sys.pDev->SetTexture(0,0);
  vec2d a(90*zoom,30*zoom),b(30*zoom,90*zoom),c(60*zoom,40*zoom),d(40*zoom,60*zoom),u(60*zoom,60*zoom),e=c+u,f=d+u;
  #define F(A)A.y*=-1;
  F(a)F(b)F(c)F(d)F(u)F(e)F(f)
  #undef F
  #define CODE_COMMENT(PARAM)//PARAM
  CODE_COMMENT("Рисуем тело"){
    QapDX::SetColor(0xfff0e000);
    QapDX::RenderTrigon(mp,mp+a,mp+b,1,1);
    QapDX::RenderTrigon(mp+c,mp+d,mp+e,1,1);
    QapDX::RenderTrigon(mp+f,mp+d,mp+e,1,1);
  }
  CODE_COMMENT("Рисуем контур"){
    QapDX::SetColor(0xff404040);
    QapDX::DrawLine(mp,mp+a,ls);
    QapDX::DrawLine(mp,mp+b,ls);
    QapDX::DrawLine(mp+a,mp+c,ls);
    QapDX::DrawLine(mp+b,mp+d,ls);
    QapDX::DrawLine(mp+c,mp+e,ls);
    QapDX::DrawLine(mp+d,mp+f,ls);
    QapDX::DrawLine(mp+e,mp+f,ls);
  }
  #undef CODE_COMMENT
};
void QapDX::DrawQuad(float x,float y,float w, float h, float a)
{
  static IDirect3DVertexBuffer9 *VB=NULL;
  static IDirect3DIndexBuffer9 *IB=NULL;
  struct Ver{
    float x,y,z;
    QapColor color;
    float tu,tv;
  };
  static const DWORD FVF=D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_DIFFUSE;
  if(!VB)
  {
    Sys.pDev->CreateVertexBuffer(4*sizeof(Ver),
      D3DUSAGE_WRITEONLY,FVF,D3DPOOL_MANAGED,&VB,NULL);
    Sys.pDev->CreateIndexBuffer(6*sizeof(int),
      D3DUSAGE_WRITEONLY,D3DFMT_INDEX32,D3DPOOL_MANAGED,&IB,NULL);
    static int IM[]={1,0,3,3,2,1};
    void *mem;
    IB->Lock(0,sizeof(int),(void **)&mem,0);
    memcpy(mem,IM,sizeof(IM));
    IB->Unlock();
  }
  Ver vertices[]=
  {
    {-0.5f,-0.5f,0, Color, 0.0f,1.0f},//Так, вот тут надо текстурные координаты править по требованию атласа текстур.
    {+0.5f,-0.5f,0, Color, 1.0f,1.0f},//И собственно сами координаты вершин надо сразу определять, а не трансформированием это делать
    {+0.5f,+0.5f,0, Color, 1.0f,0.0f},
    {-0.5f,+0.5f,0, Color, 0.0f,0.0f},
  };
  {
    void *mem;// так спрашивается, нафига я вообще каждый раз перезаливал это в память? если иногда даже цвет не менял.
    VB->Lock(0, sizeof(vertices), (void **)&mem, 0);//короче фэйл и говнокод detected!
    memcpy(mem, vertices, sizeof(vertices));
    VB->Unlock();
  }
  Sys.pDev->SetFVF(FVF);
  Sys.pDev->SetStreamSource(0,VB,0,sizeof(Ver));
  Sys.pDev->SetIndices(IB);
  QapMat4 LOL,OLD;
  Sys.pDev->GetTransform(D3DTS_WORLD,&OLD);
  LOL=Scale(w,h,1)*RotateZ(a)*Translate(x,y,0)*OLD;//Блин, да я смотрю через одно место линии то рисовал...
  Sys.pDev->SetTransform(D3DTS_WORLD,&LOL);//это тоже делать не надо.
  Sys.pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,4,0,2);
  Sys.pDev->SetTransform(D3DTS_WORLD,&OLD);
}

void QapDX::DrawLine(vec2d a,vec2d b, float h)
{
  vec2d p=(a+b)*0.5f;
  float w=(a-b).Mag();
  float ang=(b-a).GetAng();
  Sys.pDev->SetTexture(0,0);
  QapDX::DrawQuad(p.x,p.y,w,h,ang);
}

void QapDX::DrawRib(vec2d a,vec2d b,float h)
{
  const real mag=10;
  vec2d p=(a+b)*0.5;
  vec2d n=(a-b).Ort();
  QapDX::DrawLine(a,b,h);
  QapDX::DrawLine(p,p+n.SetMag(mag),h);
}

void QapDX::RenderTrigon(vec2d &a,vec2d &b,vec2d &c,real w,real h)
{
  vec2d A[]={a,b,c,a,b,c};
  RenderTrigonTex(A,w,h);
}

void QapDX::RenderTrigonTex(vec2d *A,real w,real h)
{
  static IDirect3DVertexBuffer9 *VB=NULL;
  static IDirect3DIndexBuffer9 *IB=NULL;
  struct Ver{
    float x,y,z;
    QapColor color;
    float tu,tv;
  };
  static const DWORD FVF=D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_DIFFUSE;
  if(!VB)
  {
    Sys.pDev->CreateVertexBuffer(3*sizeof(Ver),
      D3DUSAGE_WRITEONLY,FVF,D3DPOOL_MANAGED,&VB,NULL);
    Sys.pDev->CreateIndexBuffer(3*sizeof(int),
      D3DUSAGE_WRITEONLY,D3DFMT_INDEX32,D3DPOOL_MANAGED,&IB,NULL);
    static int IM[]={0,1,2};
    void *mem;
    IB->Lock(0,sizeof(int),(void **)&mem,0);
    memcpy(mem,IM,sizeof(IM));
    IB->Unlock();
  }
  //real x0=-512,y0=-384;
  real x0=0,y0=0;
  #pragma warning( disable: 4838 )
  Ver vertices[]=
  {
    #define F(a,b){a.x,a.y, 0, Color, (b.x+x0)*w,(b.y+y0)*h}
    F(A[0],A[3]),F(A[1],A[4]),F(A[2],A[5]),
    #undef F
  };
  {
    void *mem;
    VB->Lock(0, sizeof(vertices), (void **)&mem, 0);
    memcpy(mem, vertices, sizeof(vertices));
    VB->Unlock();
  }
  Sys.pDev->SetFVF(FVF);
  Sys.pDev->SetStreamSource(0,VB,0,sizeof(Ver));
  Sys.pDev->SetIndices(IB);
  Sys.pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,3,0,1);
}

void QapDX::DrawCircle(vec2d pos,real r,real ang,real tz,int seg)
{
  static IDirect3DVertexBuffer9 *VB=NULL;
  static IDirect3DIndexBuffer9 *IB=NULL;
  struct Ver{
    float x,y,z;
    QapColor color;
    float tu,tv;
  };
  const int MAX_SEG=256;
  static const DWORD FVF=D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_DIFFUSE;
  if(seg==-1){if(VB){VB->Release();VB=0;};if(IB){IB->Release();IB=0;};};
  if(!VB)
  { 
    Sys.pDev->CreateVertexBuffer(MAX_SEG*sizeof(Ver),
      D3DUSAGE_WRITEONLY,FVF,D3DPOOL_MANAGED,&VB,NULL);
    Sys.pDev->CreateIndexBuffer(3*MAX_SEG*sizeof(int),
      D3DUSAGE_WRITEONLY,D3DFMT_INDEX32,D3DPOOL_MANAGED,&IB,NULL);
  }
  static int IM[MAX_SEG*3];
  real x0=0,y0=0;
  Ver vertices[MAX_SEG];
  {
    for(int i=0;i<seg;i++)
    {   
      if(i==seg-1){};
      IM[i*3+0]=seg;IM[i*3+1]=i;IM[i*3+2]=(i+1)%seg;
      vec2d vt=Vec2dEx((real)i/(real)seg*2.0*M_PI,r);
      vec2d v=Vec2dEx(ang+(real)i/(real)seg*2.0*M_PI,r);
      Ver V={pos.x+v.x,pos.y+v.y, 0, Color, (vt.x+x0)*tz,(vt.y+y0)*tz};
      vertices[i]=V;
    }
    //static QapColor d3dc[]={0x252525,0xFF0000,0x00FF00,0xFFFF00,
    //0x0000FF,0xFF00FF,0x00FFFF,0xFFFFFF,0xFFFFA8,0xFFA8FF};
    Ver V={pos.x,pos.y, 0, Color, x0*tz,y0*tz};
    vertices[seg]=V;
    void *mem;
    IB->Lock(0,sizeof(IM),(void**)&mem,0);//fix: replace int to IM
    memcpy(mem,IM,sizeof(IM));
    IB->Unlock();
    VB->Lock(0, sizeof(vertices), (void **)&mem, 0);
    memcpy(mem, vertices, sizeof(vertices));
    VB->Unlock();
  }
  Sys.pDev->SetFVF(FVF);
  Sys.pDev->SetStreamSource(0,VB,0,sizeof(Ver));
  Sys.pDev->SetIndices(IB);
  Sys.pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,3*seg,0,seg);
}
/*
void QapDX::DrawPolygon(vec2d* Arr,int count,real ls=1)
{
  for(int i=0;i<count;i++)QapDX::DrawLine(Arr[(i+1)%count],Arr[(i)%count],ls);
}*/

void QapDX::DrawMesh(PointArray const&PA,IntArray const&IA)
{
  if(IA.empty())return;
  static IDirect3DVertexBuffer9 *VB=NULL;
  static IDirect3DIndexBuffer9 *IB=NULL;
  struct Ver{
    float x,y,z;
    QapColor color;
  };
  const int MAX_SEG=1024*128;
  const DWORD FVF=D3DFVF_XYZ|D3DFVF_DIFFUSE;
  if(!VB)
  { 
    Sys.pDev->CreateVertexBuffer(MAX_SEG*sizeof(Ver),
      D3DUSAGE_WRITEONLY,FVF,D3DPOOL_MANAGED,&VB,NULL);
    Sys.pDev->CreateIndexBuffer(3*MAX_SEG*sizeof(int),
      D3DUSAGE_WRITEONLY,D3DFMT_INDEX32,D3DPOOL_MANAGED,&IB,NULL);
  }
  static Ver vertices[MAX_SEG]; //если убрать static, то стека не хватит всё упадёт :).
  {
    for(int i=0;i<PA.size();i++){Ver V={PA[i].x,PA[i].y,0,Color};vertices[i]=V;};
    void *mem;
    IB->Lock(0,sizeof(int)*IA.size(),(void**)&mem,0);
    memcpy(mem,&IA[0],sizeof(int)*IA.size());
    IB->Unlock();
    VB->Lock(0,sizeof(Ver)*PA.size(),(void **)&mem,0);
    memcpy(mem,vertices,sizeof(Ver)*PA.size());
    VB->Unlock();
  }
  Sys.pDev->SetFVF(FVF);
  Sys.pDev->SetStreamSource(0,VB,0,sizeof(Ver));
  Sys.pDev->SetIndices(IB);
  Sys.pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,PA.size(),0,IA.size()/3);
}

void QapDX::DrawTexMesh(PointArray const&PA,IntArray const&IA,PointArray const&TA)
{
  if(IA.empty())return;
  if(TA.size()!=PA.size())return;
  static IDirect3DVertexBuffer9 *VB=NULL;
  static IDirect3DIndexBuffer9 *IB=NULL;
  struct Ver{
    float x,y,z;
    QapColor color;
    float tu,tv;
  };
  const int MAX_SEG=1024*32;
  const DWORD FVF=D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_DIFFUSE;
  if(!VB)
  { 
    Sys.pDev->CreateVertexBuffer(MAX_SEG*sizeof(Ver),
      D3DUSAGE_WRITEONLY,FVF,D3DPOOL_MANAGED,&VB,NULL);
    Sys.pDev->CreateIndexBuffer(3*MAX_SEG*sizeof(int),
      D3DUSAGE_WRITEONLY,D3DFMT_INDEX32,D3DPOOL_MANAGED,&IB,NULL);
  }
  static Ver vertices[MAX_SEG];
  {
    for(int i=0;i<PA.size();i++){Ver V={PA[i].x,PA[i].y,0,Color,TA[i].x,TA[i].y};vertices[i]=V;};
    void *mem;
    IB->Lock(0,sizeof(int)*IA.size(),(void**)&mem,0);
    memcpy(mem,&IA[0],sizeof(int)*IA.size());
    IB->Unlock();
    VB->Lock(0,sizeof(Ver)*PA.size(),(void **)&mem,0);
    memcpy(mem,vertices,sizeof(Ver)*PA.size());// - лишнее копирование, можно сразу сюда записывать.
    VB->Unlock();
  }
  Sys.pDev->SetFVF(FVF);
  Sys.pDev->SetStreamSource(0,VB,0,sizeof(Ver));
  Sys.pDev->SetIndices(IB);
  Sys.pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,PA.size(),0,IA.size()/3);
}

void QapDX::DrawTexColorMesh(PointArray const&PA,IntArray const&IA,PointArray const&TA,ColorArray const&CA)
{
  if(IA.empty())return;
  if(PA.size()!=TA.size())return;
  if(PA.size()!=CA.size())return;
  static IDirect3DVertexBuffer9 *VB=NULL;
  static IDirect3DIndexBuffer9 *IB=NULL;
  struct Ver{
    float x,y,z;
    QapColor color;
    float tu,tv;
  };
  const int MAX_SEG=1024*32;
  const DWORD FVF=D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_DIFFUSE;
  if(!VB)
  { 
    Sys.pDev->CreateVertexBuffer(MAX_SEG*sizeof(Ver),
      D3DUSAGE_WRITEONLY,FVF,D3DPOOL_MANAGED,&VB,NULL);
    Sys.pDev->CreateIndexBuffer(3*MAX_SEG*sizeof(int),
      D3DUSAGE_WRITEONLY,D3DFMT_INDEX32,D3DPOOL_MANAGED,&IB,NULL);
  }
  if(true)
  {  
    void* mem;
    IB->Lock(0,sizeof(int)*IA.size(),(void**)&mem,0);
    memcpy(mem,&IA[0],sizeof(int)*IA.size());
    IB->Unlock();
  }
  if(true)
  { 
    Ver *vertices;
    VB->Lock(0,sizeof(Ver)*PA.size(),(void**)&vertices,0);
    for(int i=0;i<PA.size();i++){Ver V={PA[i].x,PA[i].y,0,CA[i],TA[i].x,TA[i].y};vertices[i]=V;};
    VB->Unlock();
  }
  Sys.pDev->SetFVF(FVF);
  Sys.pDev->SetStreamSource(0,VB,0,sizeof(Ver));
  Sys.pDev->SetIndices(IB);
  Sys.pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,PA.size(),0,IA.size()/3);
}

void QapDX::DrawPointList(PointArray const&PA,real R)
{
  if(PA.empty())return;
  int c=PA.size();
  static PointArray VA; static IntArray IA;
  VA.resize(c*4);IA.resize(c*6);
  vec2d A[4]={vec2d(+R,+R),vec2d(-R,-R),vec2d(+R,-R),vec2d(-R,+R)};
  for(int i=0;i<c;i++)
  {
    vec2d const&a=PA[i];
    VA[i*4+0]=a+A[0];
    VA[i*4+1]=a+A[1];
    VA[i*4+2]=a+A[2];
    VA[i*4+3]=a+A[3];
    IA[i*6+0]=i*4+0;
    IA[i*6+1]=i*4+1;
    IA[i*6+2]=i*4+2;
    IA[i*6+3]=i*4+0;
    IA[i*6+4]=i*4+1;
    IA[i*6+5]=i*4+3;
  }
  QapDX::DrawMesh(VA,IA);
}

void QapDX::DrawTexPointList(PointArray const&PA,real R)
{
  if(PA.empty())return;
  int c=PA.size();
  static PointArray VA; static IntArray IA; static PointArray TA;
  VA.resize(c*4);IA.resize(c*6);TA.resize(c*4);
  vec2d A[4]={vec2d(+R,+R),vec2d(-R,-R),vec2d(+R,-R),vec2d(-R,+R)};
  vec2d TexCoord[4]={vec2d(1,1),vec2d(0,0),vec2d(1,0),vec2d(0,1)};
  for(int i=0;i<c;i++)
  {
    vec2d const&a=PA[i];
    VA[i*4+0]=a+A[0];
    VA[i*4+1]=a+A[1];
    VA[i*4+2]=a+A[2];
    VA[i*4+3]=a+A[3];
    TA[i*4+0]=TexCoord[0];
    TA[i*4+1]=TexCoord[1];
    TA[i*4+2]=TexCoord[2];
    TA[i*4+3]=TexCoord[3];
    IA[i*6+0]=i*4+0;
    IA[i*6+1]=i*4+1;
    IA[i*6+2]=i*4+2;
    IA[i*6+3]=i*4+0;
    IA[i*6+4]=i*4+1;
    IA[i*6+5]=i*4+3;
  }
  QapDX::DrawTexMesh(VA,IA,TA);
}

void QapDX::DrawTexColorPointList(PointArray const&PA,real R,ColorArray const&ColorArr)
{
  if(PA.empty())return;
  if(ColorArr.size()!=PA.size())return;
  int c=PA.size();
  static PointArray VA; static IntArray IA; static PointArray TA; static ColorArray CA;
  VA.resize(c*4);IA.resize(c*6);TA.resize(c*4);CA.resize(c*4);
  vec2d A[4]={vec2d(+R,+R),vec2d(-R,-R),vec2d(+R,-R),vec2d(-R,+R)};
  vec2d TexCoord[4]={vec2d(1,1),vec2d(0,0),vec2d(1,0),vec2d(0,1)};
  for(int i=0;i<c;i++)
  {
    vec2d const&a=PA[i];
    VA[i*4+0]=a+A[0];
    VA[i*4+1]=a+A[1];
    VA[i*4+2]=a+A[2];
    VA[i*4+3]=a+A[3];
    TA[i*4+0]=TexCoord[0];
    TA[i*4+1]=TexCoord[1];
    TA[i*4+2]=TexCoord[2];
    TA[i*4+3]=TexCoord[3];
    CA[i*4+0]=ColorArr[i];
    CA[i*4+1]=ColorArr[i];
    CA[i*4+2]=ColorArr[i];
    CA[i*4+3]=ColorArr[i];
    IA[i*6+0]=i*4+0;
    IA[i*6+1]=i*4+1;
    IA[i*6+2]=i*4+2;
    IA[i*6+3]=i*4+0;
    IA[i*6+4]=i*4+1;
    IA[i*6+5]=i*4+3;
  }
  QapDX::DrawTexColorMesh(VA,IA,TA,CA);
}

void QapDX::DrawPolyLine(PointArray const&PA,real ls,bool loop)
{
  if(PA.empty())return;
  int Count=PA.size();
  int c=loop?Count:Count-1;
  static PointArray VA; static IntArray IA;
  VA.resize(c*4);IA.resize(c*6);
  for(int i=0;i<c;i++)
  {
    vec2d const&a=PA[(i+0)%Count];
    vec2d const&b=PA[(i+1)%Count];
    vec2d ab=(b-a).Ort().SetMag(ls);
    VA[i*4+0]=a+ab;
    VA[i*4+1]=b-ab;
    VA[i*4+2]=a-ab;
    VA[i*4+3]=b+ab;
    IA[i*6+0]=i*4+0;
    IA[i*6+1]=i*4+1;
    IA[i*6+2]=i*4+2;
    IA[i*6+3]=i*4+0;
    IA[i*6+4]=i*4+1;
    IA[i*6+5]=i*4+3;
  }
  QapDX::DrawMesh(VA,IA);
}

void QapDX::DrawLineList(PointArray const&PA,real ls)
{
  if(PA.empty())return;
  int c=PA.size();
  static PointArray VA; static IntArray IA;
  VA.resize(c*2);IA.resize(c*3);
  for(int i=0;i<c;i+=2)
  {
    vec2d const&a=PA[i+0];
    vec2d const&b=PA[i+1];
    vec2d ab=(b-a).Ort().SetMag(ls);
    VA[i*2+0]=a+ab;
    VA[i*2+1]=b-ab;
    VA[i*2+2]=a-ab;
    VA[i*2+3]=b+ab;
    IA[i*3+0]=i*2+0;
    IA[i*3+1]=i*2+1;
    IA[i*3+2]=i*2+2;
    IA[i*3+3]=i*2+0;
    IA[i*3+4]=i*2+1;
    IA[i*3+5]=i*2+3;
  }
  QapDX::DrawMesh(VA,IA);
}
void QapDX::DrawConvex(PointArray &PA)
{
  if(PA.empty())return;
  int c=PA.size(),n=0;
  static IntArray IA;IA.resize((c-2)*3);
  for(int i=2;i<c;i++)
  {
    IA[n++]=0;
    IA[n++]=i-1;
    IA[n++]=i-0;
  }
  QapDX::DrawMesh(PA,IA);
}
void QapDX::DrawCircleEx(vec2d pos,real r,real ang,real ls,int seg)
{
  const int SEG=256;
  static PointArray PA; PA.resize(seg);
  for(int i=0;i<seg;i++)
  {
    vec2d v=Vec2dEx((real)i/(real)seg*2.0*M_PI,r);
    PA[i]=pos+v;
  };
  DrawPolyLine(PA,ls,true);
}
void QapDX::SetColor(QapColor Color)
{
  QapDX::Color=Color;
}

void QapDX::Blend(BlendType Mode)
{
  Sys.pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, Mode!=BT_NONE);
  if(Mode==BT_NONE)return;
  if(Mode==BT_SUB)//DESTBLENDALPHA
  {
    Sys.pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    Sys.pDev->SetTextureStageState(0, D3DTSS_COLOROP,  D3DTOP_MODULATE);
    Sys.pDev->SetTextureStageState(0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
    Sys.pDev->SetTextureStageState(0, D3DTSS_COLORARG2,D3DTA_DIFFUSE);
    Sys.pDev->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_MODULATE);
    Sys.pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
    Sys.pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
    //Sys.pDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
    //Sys.pDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
    Sys.pDev->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
    Sys.pDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
    Sys.pDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
    return;
  }
  if(Mode==BT_ADD)
  {
    //Sys.pDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
    //Sys.pDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
    Sys.pDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
    Sys.pDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
    return;
  }
}

void QapDX::Alpha(AlphaMode Mode)
{
  Sys.pDev->SetRenderState(D3DRS_ALPHATESTENABLE, Mode!=BT_NONE);
  if(Mode==AM_NONE)return;
  if(Mode==AM_GEQUAL_ONE)
  {
    Sys.pDev->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
    Sys.pDev->SetRenderState(D3DRS_ALPHAREF,1);
    return;
  }
  if(Mode==AM_NEQUAL_MAX)
  {
    Sys.pDev->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_NOTEQUAL);
    Sys.pDev->SetRenderState(D3DRS_ALPHAREF,255);
    return;
  }
}

int QapDX::GetQ3TextLength(const QapDX::QapFont &Font,const string &Text)
{
  float xp=0; int i=0;
  while(i<(int)Text.size())
  {
    if(Text[i]!='^')
    {
      int I=(byte)Text[i];
      float cx=(float)Font.W[I];
      xp+=cx; i++; continue;
    };
    i++; if(i>(int)Text.size())continue;
    if((Text[i]>='0')&&(Text[i]<='9')){i++; continue;};
    if((Text[i]>='A')&&(Text[i]<='F')){i++; continue;};
  }
  return xp;
}

string QapDX::Q3TextToNormal(const string&Text)
{
  string s; int i=0;
  while(i<(int)Text.length())
  {
    if(Text[i]!='^'){s.push_back(Text[i++]);continue;}
    i++;if(i>(int)Text.length())continue;
    if((Text[i]>='0')&&(Text[i]<='9')){i++;continue;};
    if((Text[i]>='A')&&(Text[i]<='F')){i++;continue;};
  }
  return s;
}

void QapDX::PrintText(QapDX::QapFont Font,float X,float Y,string Text)
{
  return;
  Blend(BT_ADD);
  BindTex(Font.Tex,0);
  static IDirect3DVertexBuffer9 *VB=NULL;
  static IDirect3DIndexBuffer9 *IB=NULL;
  struct Ver{
    float x,y,z;
    QapColor c;
    float u,v;
  };
  static const DWORD FVF=D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_DIFFUSE;
  if(!VB)
  {
    Sys.pDev->CreateVertexBuffer(MAX_TEXT_LENGTH*4*sizeof(Ver),
      D3DUSAGE_WRITEONLY,FVF,D3DPOOL_MANAGED,&VB,NULL);
    Sys.pDev->CreateIndexBuffer(MAX_TEXT_LENGTH*6*sizeof(int),
      D3DUSAGE_WRITEONLY,D3DFMT_INDEX32,D3DPOOL_MANAGED,&IB,NULL);
    static int IM[MAX_TEXT_LENGTH*6]={0};
    for(int i=0;i<MAX_TEXT_LENGTH;i++)
    {
      IM[6*i+0]=4*i+0;IM[6*i+1]=4*i+1;
      IM[6*i+2]=4*i+2;IM[6*i+3]=4*i+2;
      IM[6*i+4]=4*i+3;IM[6*i+5]=4*i+0;
    };
    void *mem;
    IB->Lock(0,sizeof(IM),(void **)&mem,0);
    memcpy(mem,IM,sizeof(IM));
    IB->Unlock();
  }
  Ver *vertices;
  VB->Lock(0, sizeof(Ver)*Text.length(), (void **)&vertices, 0);
  {
    float xp=0;
    for(size_t i=0;i<Text.length();i++)
    {  
      int I=(byte)Text[i];
      float s=((float)(I%16))/16,t=((float)(I/16))/16;
      float cx=(float)Font.W[I],cy=(float)Font.H[I],ts=(float)Font.Size;
      Ver v[]=
      {
          {xp+0,0,0, Color, s,1-t-cy/ts},
          {xp+cx,0,0, Color, s+cx/ts,1-t-cy/ts},
          {xp+cx,cy,0, Color, s+cx/ts,1-t},
          {xp+0,cy,0, Color, s,1-t},
      };
      for(int j=0;j<4;j++)vertices[i*4+j]=v[j]; xp+=cx;
    };
  };
  VB->Unlock();
  Sys.pDev->SetFVF(FVF);
  Sys.pDev->SetStreamSource(0,VB,0,sizeof(Ver));
  Sys.pDev->SetIndices(IB);
  QapMat4 Trans,LOL,OLD;
  Sys.pDev->GetTransform(D3DTS_WORLD,&OLD);
  LOL=Translate(round(X)+0.5f,round(Y)+0.5f,0)*OLD;
  Sys.pDev->SetTransform(D3DTS_WORLD,&LOL);
  Sys.pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,Text.length()*4,0,Text.length()*2);
  Sys.pDev->SetTransform(D3DTS_WORLD,&OLD);
}

void QapDX::DrawText(QapDX::QapFont&Font,float X,float Y,string&Text)
{
  //return;
  static IDirect3DVertexBuffer9 *VB=NULL;
  static IDirect3DIndexBuffer9 *IB=NULL;
  struct Ver{
    float x,y,z;
    QapColor c;
    float u,v;
  };
  static QapColor CT[]={0xFF252525,0xFFFF0000,0xFF00FF00,0xFFFFFF00,
  0xFF0000FF,0xFFFF00FF,0xFF00FFFF,0xFFFFFFFF,
  0xFFFFFFA8,0xFFFFA8FF,
  0xFFFF8000,0xFF0080FF,0xFFA0A0A0,0xFF0080FF,0xFFF0F000,0xFF00F0F0,
  };
  static const DWORD FVF=D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_DIFFUSE;
  if(!VB)
  {
    Sys.pDev->CreateVertexBuffer(MAX_TEXT_LENGTH*4*sizeof(Ver),
      D3DUSAGE_WRITEONLY,FVF,D3DPOOL_MANAGED,&VB,NULL);
    Sys.pDev->CreateIndexBuffer(MAX_TEXT_LENGTH*6*sizeof(int),
      D3DUSAGE_WRITEONLY,D3DFMT_INDEX32,D3DPOOL_MANAGED,&IB,NULL);
    static int IM[MAX_TEXT_LENGTH*6]={0};
    for(int i=0;i<MAX_TEXT_LENGTH;i++)
    {
      IM[6*i+0]=4*i+0;IM[6*i+1]=4*i+1;
      IM[6*i+2]=4*i+2;IM[6*i+3]=4*i+2;
      IM[6*i+4]=4*i+3;IM[6*i+5]=4*i+0;
    };
    void *mem;
    IB->Lock(0,sizeof(IM),(void **)&mem,0);
    memcpy(mem,IM,sizeof(IM));
    IB->Unlock();
  }
  Ver *vertices; int QuadCount=0;
  VB->Lock(0, sizeof(Ver)*Text.length(), (void **)&vertices, 0);
  {
    float xp=0; int i=0;
    while(i<(int)Text.length())
    {
      if(Text[i]!='^')
      {
        int I=(byte)Text[i];
        float s=((float)(I%16))/16,t=((float)(I/16))/16;
        float cx=(float)Font.W[I],cy=(float)Font.H[I],ts=(float)Font.Size;
        Ver v[]={
          {xp+0,-cy,0, Color, s,1-t-cy/ts},
          {xp+cx,-cy,0, Color, s+cx/ts,1-t-cy/ts},
          {xp+cx,0,0, Color, s+cx/ts,1-t},
          {xp+0,0,0, Color, s,1-t},
        };
        for(int j=0;j<4;j++)vertices[QuadCount*4+j]=v[j]; xp+=cx; QuadCount++; i++; continue;
      };
      i++; if(i>(int)Text.length())continue;
      if((Text[i]>='0')&&(Text[i]<='9')){SetColor(CT[Text[i]-'0']); i++; continue;};
      if((Text[i]>='A')&&(Text[i]<='F')){SetColor(CT[Text[i]-'A'+10]); i++; continue;};
    }
  };
  VB->Unlock();
  Blend(BT_SUB); Alpha(AM_GEQUAL_ONE);
  BindTex(Font.Tex,0);
  Sys.pDev->SetFVF(FVF);
  Sys.pDev->SetStreamSource(0,VB,0,sizeof(Ver));
  Sys.pDev->SetIndices(IB);
  QapMat4 Trans,LOL,OLD;
  Sys.pDev->GetTransform(D3DTS_WORLD,&OLD);
  LOL=Translate(round(X)+0.5f,round(Y)+0.5f,0)*OLD;
  Sys.pDev->SetTransform(D3DTS_WORLD,&LOL);
  Sys.pDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,QuadCount*4,0,QuadCount*2);
  Sys.pDev->SetTransform(D3DTS_WORLD,&OLD);
  Alpha(AM_NONE);
}
#include "ImgLoader.h"
#include "Dynamic.h"
void QapDX::QapTexMem::SaveToFile(const string&FN)
{
  static TLoaderEnv env;
  TLoaderEnv::TextureMemory mem;mem.W=W;mem.H=H;mem.ptr=pBits;
  env.SaveTextureToFile_v2(mem,FN);
  int gg=1;
  /*
  Image Img;
  Img.create(FORMAT_RGBA8,W,H,1,1);
  QapColor *C=(QapColor*)Img.getPixels();
  for(int i=0;i<W*H;i++){QapColor &S=pBits[i];C[i]=QapColor(S.a,S.b,S.g,S.r);}
  string ext=GetFileExt(FN);for(int i=0;i<ext.size();i++)ext[i]=toupper(ext[i]);
  #define F(EXT)if(ext==(#EXT)){Img.save##EXT(FN.c_str());return;}
    F(PNG);F(TGA);F(BMP);
  #undef F
  */
}

QapDX::QapTexMem* QapDX::LoadTexture(const string&FN)
{
  static TLoaderEnv env;
  TLoaderEnv::TextureMemory mem;
  env.LoadTextureFromFile_v2(mem,FN);
  if(!mem.ptr){MACRO_ADD_LOG("Error Loading \""+FN+"\"",lml_WARNING);return NULL;}
  QapTexMem*pTex=new QapTexMem(FN);
  pTex->W=mem.W;
  pTex->H=mem.H;
  pTex->pBits=(QapColor*)mem.ptr;
  /*string dbg="";
  for(int y=0;y<pTex->H;y++){for(int x=0;x<pTex->W;x++){
    dbg=
  }dbg+="\n";}*/
  //pTex->Clear(QapColor(255,0,0,0));
  //pTex->Clear(0xffff00ff);
  return pTex;
}

QapTexMem*QapDX::BlurTexture(QapTexMem*Tex,int PassCount)//only D3DFMT_A8R8G8B8
{
  int &W=Tex->W;int &H=Tex->H;QapColor *&pBits=Tex->pBits;
  #define BlurLog(X,Y)//MACRO_ADD_LOG(X,Y)
  BlurLog("Blur \""+Tex->Name+"\" x"+IToS(PassCount),lml_HINT);
  struct QapARGB{BYTE B,G,R,A;};
  static QapARGB VoidMem[2048*2048*4];
  memcpy_s(VoidMem,sizeof(VoidMem),pBits,W*H*sizeof(QapARGB));//copy image
  static int BBM[9]={
    1,2,1, 
    2,4,2, 
    1,2,1};
  #define INIT_STATIC_VAR(TYPE,VALUE,INIT_CODE)static TYPE VALUE;{static bool _STATIC_SYS_FLAG=true;if(_STATIC_SYS_FLAG){INIT_CODE;_STATIC_SYS_FLAG=false;};};
    INIT_STATIC_VAR(int,MartixSum,for(int i=0;i<9;i++)MartixSum+=BBM[i];);
  #undef INIT_STATIC_VAR
  int PosRange[9]={
    -W-1,-W,-W+1,
    -1,0,+1,
    +W-1,+W,+W+1};
  for(int PassId=0;PassId<PassCount;PassId++)
  {
    QapARGB *PC=0;
    QapARGB *VM=0;
    for(int j=1;j<H-1;j++)
      for(int i=1;i<W-1;i++)
      {
        PC=(QapARGB*)pBits+j*W+i; VM=(QapARGB*)VoidMem+j*W+i;
        float AF[4]={0,0,0,0};
        for(int t=0;t<9;t++)
        {
          QapARGB T=*(VM+PosRange[t]);
          AF[0]+=T.R*BBM[t];
          AF[1]+=T.G*BBM[t];
          AF[2]+=T.B*BBM[t];
          AF[3]+=T.A*BBM[t];
        };    
        for(int i=0;i<4;i++)AF[i]/=MartixSum*255.0;
        QapColor PCC=D3DCOLOR_COLORVALUE(AF[0],AF[1],AF[2],AF[3]);
        *PC=*((QapARGB*)&PCC);
      }
    //PassId++;
    memcpy_s(VoidMem,sizeof(VoidMem),pBits,W*H*sizeof(QapARGB));
  }
  BlurLog("Blur \""+Tex->Name+"\" x"+IToS(PassCount),lml_HINT);
  #undef BlurLog
  return Tex;
}

QapTex*QapDX::GenTextureMipMap(QapTexMem*&Tex,int MaxLevelCount)//only D3DFMT_A8R8G8B8
{
  if(!Tex)return NULL;
  int &W=Tex->W;int &H=Tex->H;QapColor *&pBits=Tex->pBits;
  if(0){fail:MACRO_ADD_LOG("NPOT texture \""+Tex->Name+"\"("+IToS(W)+"x"+IToS(H)+")",lml_WARNING);return NULL;}
  int SW=1,SWC=0;for(int &i=SWC;SW<W;i++)SW*=2; if(SW>W)goto fail;
  int SH=1,SHC=0;for(int &i=SHC;SH<H;i++)SH*=2; if(SH>H)goto fail;
  int Levels=min(MaxLevelCount,min(SWC,SHC));
  IDirect3DTexture9 *tex;
  Sys.pDev->CreateTexture(SW,SH,Levels,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&tex,NULL);
  D3DLOCKED_RECT rect[16]; struct QapARGB{BYTE B,G,R,A;}; QapARGB* pBitsEx[16];
  for(int i=0;i<Levels;i++){tex->LockRect(i,&rect[i],NULL,0);pBitsEx[i]=(QapARGB*)rect[i].pBits;};
  {
    QapColor* pDestBits=(QapColor*)rect[0].pBits;
    memcpy_s(pDestBits,SW*SH*sizeof(QapColor),pBits,SW*SH*sizeof(QapColor));
    SW/=2; SH/=2;
  }
  for(int k=1;k<Levels;k++)
  {
    QapARGB *PC=(QapARGB*)rect[k].pBits;
    for(int j=0;j<SH;j++)
      for(int i=0;i<SW;i++)
      {
        #define F(X,Y)pBitsEx[k-1][X+2*Y*SW+2*i+4*j*SW]
          QapARGB A[4]={F(0,0),F(0,1),F(1,0),F(1,1)};
        #undef F
        float AF[4]={0,0,0,0};
        for(int t=0;t<4;t++)
        {
          AF[0]+=A[t].R;
          AF[1]+=A[t].G;
          AF[2]+=A[t].B;
          AF[3]+=A[t].A;
        };
        for(int t=0;t<4;t++)AF[t]*=0.25*(1.0/255.0);
        QapColor PCC=D3DCOLOR_COLORVALUE(AF[0],AF[1],AF[2],AF[3]);
        *PC=*((QapARGB*)&PCC);
        PC++;
      }
    SW/=2; SH/=2;
  }
  for(int i=0;i<Levels;i++)tex->UnlockRect(i);
  //MACRO_ADD_LOG("Loaded \""+Tex->Name+"\"",lml_HINT);
  QapTex *pTex=new QapTex(Tex,tex);
  delete Tex;Tex=NULL;
  return pTex;
}

QapTexMem* QapDX::GenBall(int TexSize)
{
  QapColor *pBits=new QapColor[TexSize*TexSize];
  QapColor *PC=pBits;
  for(int i=0;i<TexSize;i++)
    for(int j=0;j<TexSize;j++)
    {
      float x=i-TexSize/2+0.5f,y=j-TexSize/2+0.5f;
      float r=(float)TexSize/2;
      float m=((x*x)+(y*y))/(r*r);
      m=max(0,1-(m*m*m)); 
      float a=(m<0.5)?m*2:1;
      a=max(0,a);
      struct QapARGB{byte B,G,R,A;};
      QapColor T=0xFFFFFFFF;
      QapARGB *z=(QapARGB*)&T,Z=*z;
      *PC=D3DCOLOR_ARGB((byte)(a*Z.A),(byte)(m*Z.R),(byte)(m*Z.G),(byte)(m*Z.B));     
      PC++;
    }
  QapTexMem* pTex=new QapTexMem("GenBall_"+IToS(TexSize));
  pTex->W=TexSize;pTex->H=TexSize;pTex->pBits=pBits;
  return pTex;
}

QapTexMem* QapDX::GenLattice(int TexSize)
{
  QapColor *pBits=new QapColor[TexSize*TexSize];
  QapColor *PC=pBits;
  bool flag=false;
  for(int i=0;i<TexSize;i++)
    for(int j=0;j<TexSize;j++)
    {
      if(!((i+j)%8))flag=!flag;
      *PC=flag?0xFF808080:0xFFF0F0F0;     
      PC++;
    }
  QapTexMem* pTex=new QapTexMem("GenLattice_"+IToS(TexSize));
  pTex->W=TexSize;pTex->H=TexSize;pTex->pBits=pBits;
  return pTex;
}

QapTexMem* QapDX::GenQuad(int TexSize)
{
  QapColor *pBits=new QapColor[TexSize*TexSize];
  QapColor *PC=pBits;
  for(int i=0; i<TexSize; i++)
    for(int j=0; j<TexSize; j++)
    {
      float x=i-TexSize/2+0.5f,y=j-TexSize/2+0.5f;
      float r=(float)TexSize/2;
      float m=((x*x*x*x)+(y*y*y*y))/(r*r*r*r);
      m=max(0,1-(m*m*m)); 
      float a=(m<0.5)?m*2:1;
      a=max(0,a);
      *PC=D3DCOLOR_COLORVALUE(m,m,m,a);
      PC++;
    }
  QapTexMem* pTex=new QapTexMem("GenQuad_"+IToS(TexSize));
  pTex->W=TexSize;pTex->H=TexSize;pTex->pBits=pBits;
  return pTex;
}

QapTexMem* QapDX::GenQuadSL(int TexSize)
{
  QapColor *pBits=new QapColor[TexSize*TexSize];
  QapColor *PC=pBits;
  for(int i=0; i<TexSize; i++)
    for(int j=0; j<TexSize; j++)
    {
      float x=i-TexSize/2+0.5f,y=j-TexSize/2+0.5f;
      float r=(float)TexSize/2;
      float m=((x*x*x*x)+(y*y*y*y))/(r*r*r*r);
      m=max(0,1-(m*m*m)); 
      float a=(m<0.5)?m*2:1;
      a=max(0,a);
      *PC=D3DCOLOR_COLORVALUE(1,1,1,a);
      PC++;
    }
  QapTexMem* pTex=new QapTexMem("GenQuadSL_"+IToS(TexSize));
  pTex->W=TexSize;pTex->H=TexSize;pTex->pBits=pBits;
  return pTex;
}
QapDX::QapFont QapDX::FontCreate(string Name,int Size,bool Bold,int TexSize)
{
  HDC DC=GetDC(Sys.hWnd);
  int W=Bold?FW_BOLD:FW_NORMAL;
  int H=-MulDiv(Size,GetDeviceCaps(DC,LOGPIXELSY),72);
  HFONT FNT=CreateFontA(H,0,0,0,W,0,0,0,RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,DEFAULT_PITCH,Name.c_str());
  //sdf46sdf54sd6f54s6d5f46s5df465sd4f65sd4f65sd
  BITMAPINFO BI; BITMAPINFOHEADER &BH=BI.bmiHeader; ZeroMemory(&BI,sizeof(BI));
  BH.biSize=sizeof(BI.bmiHeader);
  BH.biWidth=TexSize; BH.biHeight=TexSize;
  BH.biPlanes=1; BH.biBitCount=32;
  BH.biSizeImage=TexSize*TexSize*4;
  //sdf46sdf54sd6f54s6d5f46s5df465sd4f65sd4f65sd
  //65a4sd654sa5dasdklflkgldfgjrioe90843jh33kldf
  HDC MDC=CreateCompatibleDC(DC);
  HBITMAP BMP=CreateCompatibleBitmap(DC,TexSize,TexSize);
  SelectObject(MDC,BMP);
  RECT Rect; SetRect(&Rect,0,0,TexSize,TexSize);
  FillRect(MDC,&Rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
  SelectObject(MDC,FNT);
  SetBkMode(MDC,TRANSPARENT);
  SetTextColor(MDC,0xFFFFFF);
  for(int i=0;i<256;i++)TextOutA(MDC,i%16*(TexSize/16),i/16*(TexSize/16),(LPCSTR)&i,1);
  //65a4sd654sa5dasdklflkgldfgjrioe90843jh33kldf
  //jksdhfui4y823oim09238c9023589c23981cz091573d
  union ARGB{COLORREF C; struct{byte B,G,R,A;};};
  ARGB* pix=new ARGB[TexSize*TexSize];
  GetDIBits(MDC,BMP,0,TexSize,pix,&BI,DIB_RGB_COLORS);
  for(int i=0;i<TexSize*TexSize;i++)
  {
    pix[i].A=pix[i].R; pix[i].R=255; pix[i].G=255; pix[i].B=255;
  }
  QapTexMem*pTex=new QapTexMem("Font_"+Name+"_"+IToS(TexSize),TexSize,TexSize,(QapColor*)pix);
  QapFont Q(GenTextureMipMap(pTex),TexSize); 
/*
  //jksdhfui4y823oim09238c9023589c23981cz091573d
  //jfhuiy2yskdjfn84orfo34f8sjkdhfgshvbnzbcuywfd
  IDirect3DTexture9 *tex;
  Sys.pDev->CreateTexture(TexSize,TexSize,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&tex,NULL);
  D3DLOCKED_RECT rect;
  tex->LockRect(0,&rect,NULL,0);
  memcpy(rect.pBits,pix,TexSize*TexSize*4);
  tex->UnlockRect(0);*/
  //jfhuiy2yskdjfn84orfo34f8sjkdhfgshvbnzbcuywfd
  //jkhdfsakjhdfjkehqwureyuy8382ujhdjaxhwdkandud
  SIZE cs;
  for(int i=0;i<256;i++)
  {
    GetTextExtentPoint32A(MDC,(LPCSTR)&i,1,&cs); 
    Q.W[i]=cs.cx; Q.H[i]=cs.cy;
  }
  //
  //jkhdfsakjhdfjkehqwureyuy8382ujhdjaxhwdkandud
  DeleteObject(FNT); DeleteObject(BMP); DeleteDC(MDC); ReleaseDC(Sys.hWnd,DC); return Q;
}
void QapDX::FreeFont(QapDX::QapFont &Font)
{
  if(Font.Tex){delete Font.Tex; Font.Tex=0;};
}
/************************************************************************/
/* Конец                                                                */
/************************************************************************/
