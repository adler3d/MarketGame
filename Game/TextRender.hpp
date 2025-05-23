class TextRender{
public:
  typedef QapDX::QapFont QapFont;
  QapDev*RD;
  struct TextLine{
  public:
    string text;
    int x,y;
  public:
    TextLine(int x,int y,const string&text):x(x),y(y),text(text){}
  public:
    void DrawRaw(QapDev*RD,QapFont*Font,int dv){DrawQapText(RD,*Font,x+dv+0.5,y-dv+0.5,QapDX::Q3TextToNormal(text));}
    void DrawSys(QapDev*RD,QapFont*Font,int dv){DrawQapText(RD,*Font,x+dv+0.5,y-dv+0.5,text);}
  };
  vector<TextLine> LV;
  TextRender(QapDev*RD):RD(RD){}
public:
  int x,y,ident,bx;
  QapFont*NormFont;
  QapFont*BlurFont;
public:
  void BeginScope(int X,int Y,QapFont*NormFont,QapDX::QapFont*BlurFont){
    bx=X;x=X;y=Y;ident=24;this->NormFont=NormFont;this->BlurFont=BlurFont;
  }
  void BR(){y-=ident;x=bx;}
  void AddText(const string&text)
  {
    LV.push_back(TextLine(x,y,text));BR();
  }
  int text_len(const string&text){return QapDX::GetQ3TextLength(*NormFont,text);}
  void AddTextNext(const string&text)
  {
    LV.push_back(TextLine(x,y,text));x+=QapDX::GetQ3TextLength(*NormFont,text);
  }
  void EndScope(){
    RD->SetBlendMode(QapDX::BT_SUB);
    RD->SetAlphaMode(QapDX::AM_NONE);
    {
      RD->BindTex(0,BlurFont->Tex);
      RD->SetColor(0xff000000);
      RD->BeginBatch();
      for(int i=0;i<LV.size();i++)LV[i].DrawRaw(RD,BlurFont,1.0);
      RD->EndBatch();
    }
    {
      RD->BindTex(0,NormFont->Tex);
      RD->SetColor(0xffffffff);
      RD->BeginBatch();
      for(int i=0;i<LV.size();i++)LV[i].DrawSys(RD,NormFont,0.0);
      RD->EndBatch();
    }
    RD->SetAlphaMode(QapDX::AM_NONE);
  }
};