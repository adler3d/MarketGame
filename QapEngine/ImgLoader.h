// Interface. App_Side <=> DLL_Side

struct ImgLoader{
  struct string
  {
    const char*ptr;
    int size;
  };
  struct TextureMemory
  {
    void*ptr;
    int W;
    int H;
    TextureMemory(){ptr=0;W=0;H=0;}
  };
  struct IEnv
  {
    virtual void InitMemory(TextureMemory&out,int W,int H)=0;
    virtual void FreeMemory(TextureMemory&in)=0;
  };
  virtual void LoadTextureFromFile(IEnv&Env,TextureMemory&Mem,string FileName)=0;
  virtual void SaveTextureToFile(IEnv&Env,TextureMemory&Mem,string FileName)=0;
  virtual bool KeepExt(string ext)=0;
};