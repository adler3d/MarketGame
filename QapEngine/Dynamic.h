#include "lodepng.cpp"

struct TLoaderEnv:ImgLoader::IEnv
{
  static ImgLoader::string SToS(const std::string&in)
  {
    ImgLoader::string tmp;
    if(in.empty()){tmp.ptr=0;tmp.size=0;return tmp;}
    tmp.ptr=&in[0];
    tmp.size=in.size();
    return tmp;
  }
  typedef ImgLoader::TextureMemory TextureMemory;
  //using ImgLoader::TextureMemory;
  void InitMemory(TextureMemory&out,int W,int H)
  {
    if(W*H<=0)return;
    assert(!out.ptr);
    assert(!out.W);
    assert(!out.H);
    out.ptr=new int[W*H];
    out.W=W;
    out.H=H;
    int*p=(int*)out.ptr;
    for(int i=0;i<W*H;i++)p[i]=0;
  }
  void FreeMemory(TextureMemory&in)
  {
    int*ptr=(int*)in.ptr;
    delete[] ptr;
    in.ptr=0;
    in.W=0;
    in.H=0;
  }
  //DynamicContext DC;
  //ImgLoader*pLoader;
  //TLoaderEnv()
  //{
  //  DC.init();
  //  pLoader=DC.GetImgLoader();
  //}
  //TLoaderEnv(string&path)
  //{
  //  DC.path=path;
  //  DC.init();
  //  pLoader=DC.GetImgLoader();
  //}
  //~TLoaderEnv()
  //{
  //  pLoader=nullptr;
  //  DC.free();
  //}
  //static string GetFileExt(const std::string&fn)
  //{
  //  //std::string fn=FileName;
  //  auto x=fn.rfind('.');
  //  if(x==std::string::npos)return "";
  //  return fn.substr(x+1);
  //}
  //void LoadTextureFromFile(TextureMemory&Mem,const std::string&FileName)
  //{
  //  if(!pLoader)return;
  //  std::string fn=FileName;
  //  pLoader->LoadTextureFromFile(*this,Mem,SToS(fn));
  //}
  //void SaveTextureToFile(TextureMemory&Mem,const std::string&FileName)
  //{
  //  if(!pLoader)return;
  //  if(!KeepExt(GetFileExt(FileName)))return;
  //  pLoader->SaveTextureToFile(*this,Mem,SToS(FileName));
  //}
  //bool KeepExt(const std::string&ext)
  //{
  //  return pLoader->KeepExt(SToS(ext));
  //}
  //v2
  struct bgra{unsigned char b,g,r,a;};
  bool LoadTextureFromFile_v2(TextureMemory&mem,const std::string&fn)
  {
    return load_tex(mem,fn.c_str());
  }
  bool load_tex(TextureMemory&mem,const char*fn)
  {
    std::vector<unsigned char> image;
    unsigned cx=0,cy=0;
    decodeOneStep(fn,image,cx,cy);
    if(!cx||!cy)return false;
    InitMemory(mem,cx,cy);auto*arr=(bgra*)mem.ptr;
    for(unsigned y=0;y<cy;y++)
    for(unsigned x=0;x<cx;x++)
    {
      auto&out=arr[x+cx*y];
      auto&inp=*(bgra*)(void*)&image[4*cx*y+4*x+0];
      out=inp;
      std::swap(out.r,out.b);
    }
    return true;
  }
  static void decodeOneStep(const char*fn,std::vector<unsigned char>&image,unsigned&width,unsigned&height)
  {
    unsigned error=lodepng::decode(image,width,height,fn);
    //if(error)QapDebugMsg("encoder error "+std::to_string(error)+": "+lodepng_error_text(error));
  }
  void SaveTextureToFile_v2(TextureMemory&mem,const string&fn){
    assert(mem.ptr);
    save_tex(mem.W,mem.H,(bgra*)mem.ptr,fn.c_str());
  }
  //void save_tex(int cx,int cy,const vector<QapColor>&arr,const char*fn)
  void save_tex(int cx,int cy,bgra*arr,const char*fn)
  {
    std::vector<unsigned char> image;
    image.resize(cx*cy*4);
    for(unsigned y=0;y<cy;y++)
    for(unsigned x=0;x<cx;x++)
    {
      auto c=arr[x+cx*y];
      auto&out=*(bgra*)(void*)&image[4*cx*y+4*x+0];
      out=c;
      std::swap(out.r,out.b);
    }
    encodeOneStep(fn,image,cx,cy);
  }
  static void encodeOneStep(const char*fn,std::vector<unsigned char>&image,unsigned width,unsigned height)
  {
    unsigned error=lodepng::encode(fn,image,width,height);
    //if(error)QapDebugMsg("encoder error "+std::to_string(error)+": "+lodepng_error_text(error));
  }
};