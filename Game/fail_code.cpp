        QapDX::QapTexMem*ApplyMode2(QapDX::QapTexMem*pMem)
  {
    Atlas.AddFrame(pMem);
    auto*tmp=pMem->CopyAlpha(pMem->Clone()->Clear());
    Atlas.AddFrame(tmp);
    tmp=AddBorder(pMem)->GenEdge(vec2i(1,1),0xffffffff);//->InvertRGB();
    Atlas.AddFrame(tmp);
    for(int i=0;i<4;i++){
      tmp=QapDX::BlurTexture(tmp,1);
      Atlas.AddFrame(tmp);
      tmp->NormRGB();
      Atlas.AddFrame(tmp);
    };
    tmp->CalcAlpha()->InvertRGB();
    Atlas.AddFrame(tmp);
    tmp=AddBorder(pMem)->Draw(0,0,tmp,0xffffff);
    Atlas.AddFrame(tmp);
    return tmp;
  }
    //if(MODE==2){ApplyMode2(QapDX::LoadTexture("GFX\\"FILE".png"))->SaveToFile("GFX\\"FILE"_out.png");};