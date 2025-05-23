#include "StdAfx.h"
#include "QapScene.h"

QapColor ColorFunc(float t,int id)
{
  float r=0,g=0,b=0,p=3.14f;
  if(id==1)t+=20;
  if(id==2)t+=15;
  if(id==3)t-=15;
  r=abs(cos(t*p/3.1f))*0.5+0.5; t+=0.3;
  g=abs(cos(t*p/5.2f))*0.7; t+=0.3;
  b=abs(cos(t*p/21.3f))*0.2+0.3; 
  return D3DCOLOR_XRGB(int(255*r),int(255*g),int(255*b));
}

void Scene::Render(){SceneVIP::DefRender();}
void Scene::Update(){SceneVIP::DefUpdate();}
void Scene::SystemRender(){Render();}
void Scene::SystemUpdate(){Update();}

namespace SceneVIP
{
  Scene *ActiveScene;
  QapDX::QapTex *TexBall=NULL,*TexQuad=NULL,*TexLatt=NULL,*TexQuadSL=NULL;
  QapDX::QapFont SysFont,NewFont;
  vector<Scene*> SceneList;
  vector<string> SceneNameList;
}

void SceneVIP::RegScene(Scene *Link,string SceneName)
{
  SceneNameList.push_back(SceneName);
  SceneList.push_back(Link);
  SetActiveScene(Link);
}

void SceneVIP::SetActiveScene(Scene *Link){ActiveScene=Link;}
void SceneVIP::Update(){if(ActiveScene){ActiveScene->Update();}else{DefUpdate();}} 
void SceneVIP::Render(){if(ActiveScene){ActiveScene->Render();}else{DefRender();}}

void SceneVIP::DefRender()
{

};
void SceneVIP::DefUpdate(){};

void SetFilter(DWORD value)
{
  Sys.pDev->SetSamplerState(0,D3DSAMP_MAGFILTER,value);
  Sys.pDev->SetSamplerState(0,D3DSAMP_MINFILTER,value);
}

void SceneVIP::Init()
{
  MACRO_ADD_LOG("Loading...",lml_EVENT); 
  SetFilter(D3DTEXF_LINEAR);
  SysFont=QapDX::FontCreate("FixedSys",8,false,512);
  //NewFont=QapDX::FontCreate("New Times Roman",8,true,512);
  #define GEN(NAME,SIZE)QapDX::GenTextureMipMaps(QapDX::NAME(SIZE));
  //TexBall=QapDX::GenTextureMipMap(QapDX::GenBall(512));
  //TexQuad=QapDX::GenTextureMipMap(QapDX::GenQuad(512));
  //TexLatt=QapDX::GenTextureMipMap(QapDX::GenLattice(64));
  //TexQuadSL=QapDX::GenTextureMipMap(QapDX::GenQuadSL(512));
  #undef GEN
  MACRO_ADD_LOG("Starting...",lml_EVENT);
  for(int i=0;i<SceneList.size();i++){SceneList[i]->SceneName=SceneNameList[i];SceneList[i]->Init();};
}

void SceneVIP::Free()
{
  MACRO_ADD_LOG("Unloading...",lml_EVENT); 
  for(int i=0;i<SceneList.size();i++){Scene* EX=SceneList[i];EX->Free();};SceneList.clear();
  #define Unload(TEX)if(TEX){delete TEX;TEX=0;};
  Unload(TexBall);
  Unload(TexQuad);
  Unload(TexLatt);
  Unload(TexQuadSL);
  #undef Unload
  FreeFont(SysFont);
  QapDX::UnloadTextures();
}

//Tex=QapDX::LoadTexture("D:\\Fail.tga");
//NewFont=QapDX::FontCreate("Comic Sans MS",12,false,512);
//D3DXCreateTextureFromFile(Sys.pDev,"D:\\Fail.tga",&Tex);
//D3DXSaveTextureToFile("D:\\Share\\Images\\Fail.tga",D3DXIFF_PNG,TexQuad,0);