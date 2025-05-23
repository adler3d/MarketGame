template<class TYPE>
class TWeakCopy{
public:
  TYPE&Ref;
  TWeakCopy(TYPE&Ref):Ref(Ref){}
public:
  operator TYPE(){return *Ref;}
};

union TQuad;
/*
union TRect{
  struct{vec2d a,b;};
  struct{real ax,ay,bx,by;};
public:
  TRect(){}
  TRect(const vec2d&a,const vec2d&b):a(a),b(b){}
  TRect(const real&ax,const real&ay,const real&bx,const real&by):ax(ax),ay(ay),bx(bx),by(by){}
public:
  template<typename TYPE>operator TWeakCopy<TYPE>(){return TWeakCopy<TYPE>(TQuad(0.5*(a+b),b-a));}
public:
   TRect Norm(){return TRect(min(ax,bx),min(ay,by),max(ax,bx),max(ay,by));}
};

union TQuad{
  struct{vec2d pos,size;};
  struct{real x,y,w,h;};
public:
  TQuad(){}
  TQuad(const vec2d&pos,const vec2d&size):pos(pos),size(size){}
  TQuad(const real&x,const real&y,const real&w,const real&h):x(x),y(y),w(w),h(h){}
public:
  operator TRect(){return TRect(x-w,y-h,x+w,y+h);}
public:
  TQuad Norm(){return TQuad(x,y,abs(w),abs(h));}
  vec2d RndPoint(){return vec2d(x+RndReal(-w*0.5,+w*0.5),y+RndReal(-h*0.5,+h*0.5));}
};

inline vec2d ClampCircleToQuad(const vec2d&p,const real&r,const TQuad&q){return q.pos+vec2d(Clamp(p.x-q.x,-q.w*0.5+r,q.w*0.5-r),Clamp(p.y-q.y,-q.h*0.5+r,q.h*0.5-r));}
inline PointArray MakeQuadAABB(const vec2d&Min,const vec2d&Max){
  PointArray PA;PA.resize(4);
  PA[0]=vec2d(Min.x,Min.y);PA[1]=vec2d(Min.x,Max.y);
  PA[2]=vec2d(Max.x,Max.y);PA[3]=vec2d(Max.x,Min.y);
  return PA;
}*/
inline bool CD_Rect2Point(vec2d A,vec2d B,vec2d P)
{
  vec2d &p=P;vec2d a(min(A.x,B.x),min(A.y,B.y)),b(max(A.x,B.x),max(A.y,B.y));
  return InDip(a.x,p.x,b.x)&&InDip(a.y,p.y,b.y);
}
inline bool CD_Circle2Point(const vec2d&p,const vec2d&o,const real&r){return (p-o).Mag()<r;};
inline bool CD_Circle2Segment(const vec2d&p,const vec2d&a,const vec2d&b,const real&r)
{
  vec2d o=b-a;
  vec2d t=p-a;
  t=t.Rot(o);
  real om=o.Mag();
  return CD_Circle2Point(p,a,r)||CD_Circle2Point(p,b,r)||(InDip(real(0),t.x,om)&&abs(t.y)<r);
}
inline bool CD_Segment2Segment(const vec2d&a,const vec2d&b,const vec2d&c,const vec2d&d,vec2d&o)
{
  vec2d A[5]={a,b-a,c-a,d-a,o};
  real m=A[1].Mag();
  A[2]=A[2].Rot(A[1]);
  A[3]=A[3].Rot(A[1]);
  if(Sign(A[2].y)==(Sign(A[3].y)))return false;
  A[4].x=A[2].x-(A[2].x-A[3].x)*A[2].y/(A[2].y-A[3].y); A[4].y=0;
  if(!InDip(real(0),A[4].x,m))return false;
  o=A[4].UnRot(A[1])+a;
  return true;
}
inline void DrawQapText(QapDev*RD,QapDX::QapFont&Font,float X,float Y,const string&Text)
{
  static QapColor CT[]={
    0xFF252525,0xFFFF0000,0xFF00FF00,0xFFFFFF00,
    0xFF0000FF,0xFFFF00FF,0xFF00FFFF,0xFFFFFFFF,
    0xFFFFFFA8,0xFFFFA8FF,
    0xFFFF8000,0xFF0080FF,0xFFA0A0A0,0xFF808080,0xFFF0F000,0xFF00F0F0,
  };
  bool _4it=!RD->IsBatching();
  if(_4it)RD->BeginBatch();
  int QuadCount=0;
  int VPos=RD->GetVPos();
  {
    float xp=0; int i=0;
    while(i<(int)Text.length())
    {
      if(Text[i]!='^')
      {
        int I=(byte)Text[i];
        float s=((float)(I%16))/16,t=((float)(I/16))/16;
        float cx=(float)Font.W[I],cy=(float)Font.H[I],ts=(float)Font.Size;
        #define F(var,x,y,z,color,u,v)int var=RD->AddVertex(QapDev::Ver(X+x,Y+y,color,u,v));
          F(A,xp+0,-cy,0,RD->GetColor(),s,1-t-cy/ts);
          F(B,xp+cx,-cy,0,RD->GetColor(),s+cx/ts,1-t-cy/ts);
          F(C,xp+cx,0,0,RD->GetColor(),s+cx/ts,1-t);
          F(D,xp+0,0,0,RD->GetColor(),s,1-t);
        #undef F
        RD->AddTris(A,B,C);
        RD->AddTris(C,D,A);
        xp+=cx; QuadCount++; i++; continue;
      };
      i++; if(i>(int)Text.length())continue;
      if((Text[i]>='0')&&(Text[i]<='9')){RD->SetColor(CT[Text[i]-'0']); i++; continue;};
      if((Text[i]>='A')&&(Text[i]<='F')){RD->SetColor(CT[Text[i]-'A'+10]); i++; continue;};
    }
  };
  if(_4it)RD->EndBatch();
}