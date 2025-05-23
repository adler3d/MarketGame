#pragma once
//-------------------------------------------//
//   {<| author : Adler3D(Kashin Ivan) |>}   //
//   {<| e-mail : Adler3D@gmail.com    |>}   //
//   {<| site   : Adler3d.narod.ru     |>}   //
//-------------------------------------------//
#include "QapType.h"
//-------------------------------------------//
const real Pi=3.14159265;
const real Pi2=Pi*2;
const real PiD2=Pi/2;
const real PiD4=Pi/4;
//-------------------------------------------//
template<typename TYPE>inline TYPE Lerp(const TYPE&A,const TYPE&B,const real&v){return A+(B-A)*v;}
template<class TYPE>inline TYPE Clamp(const TYPE&v,const TYPE&a,const TYPE&b){return max(a,min(v, b));}
//-------------------------------------------//
struct vec2d
{
public:
  real x,y;
  vec2d():x(0),y(0){}
  vec2d(const real&x,const real&y):x(x),y(y){}
  vec2d(const vec2d&v):x(v.x),y(v.y){}
  vec2d&operator=(const vec2d&v){x=v.x;y=v.y;return *this;}
  vec2d operator+()const{return *this;}
  vec2d operator-()const{return vec2d(-x,-y);}
  vec2d&operator+=(const vec2d&v){x+=v.x;y +=v.y;return *this;}
  vec2d&operator-=(const vec2d&v){x-=v.x; y-=v.y;return *this;}
  vec2d&operator*=(const real&f){x*=f;y*=f;return *this;}
  vec2d&operator/=(const real&f){x/=f;y/=f;return *this;}
public:
  vec2d Rot(const vec2d&OX)const{real M=OX.Mag();return vec2d(((x*+OX.x)+(y*OX.y))/M,((x*-OX.y)+(y*OX.x))/M);}
  vec2d UnRot(const vec2d&OX)const{real M=OX.Mag();if(M==0.0f){return vec2d(0,0);};return vec2d(((x*OX.x)+(y*-OX.y))/M,((x*OX.y)+(y*+OX.x))/M);}
  vec2d Ort()const{return vec2d(-y,x);}
  vec2d Norm()const{if((x==0)&&(y==0)){return vec2d(0,0);}return vec2d(x/this->Mag(),y/this->Mag());}
  vec2d SetMag(const real&val)const{return this->Norm()*val;}
  vec2d Mul(const vec2d&v)const{return vec2d(x*v.x,y*v.y);}
  vec2d Div(const vec2d&v)const{return vec2d(v.x!=0?x/v.x:x,v.y!=0?y/v.y:y);}
  real GetAng()const{return atan2(y,x);}
  real Mag()const{return sqrt(x*x+y*y);}
public:
  friend vec2d operator+(const vec2d&u,const vec2d&v){return vec2d(u.x+v.x,u.y+v.y);}
  friend vec2d operator-(const vec2d&u,const vec2d&v){return vec2d(u.x-v.x,u.y-v.y);}
  friend vec2d operator*(const vec2d&u,const real&v){return vec2d(u.x*v,u.y*v);}
  friend vec2d operator*(const real&u,const vec2d&v){return vec2d(u*v.x,u*v.y);}
  friend bool operator==(const vec2d&u,const vec2d&v){return (u.x==v.x)||(u.y==v.y);}
  friend bool operator!=(const vec2d&u,const vec2d&v){return (u.x!=v.x)||(u.y!=v.y);}
public:
  //friend inline static vec2d Vec2dEx(const real&ang,const real&mag){return vec2d(cos(ang)*mag,sin(ang)*mag);}
public:
  friend inline static real dot(const vec2d&a,const vec2d&b){return a.x*b.x+a.y*b.y;}
  friend inline static real cross(const vec2d&a,const vec2d&b){return a.x*b.y-a.y*b.x;}
public:
  #ifdef BOX2D_H
    operator b2Vec2()const{return b2Vec2(x,y);}
    vec2d(const b2Vec2& v):x(v.x),y(v.y){}
  #endif
};
//-------------------------------------------//
struct QapColor
{
public:
  byte b,g,r,a;
  QapColor():b(255),g(255),r(255),a(255){}
  QapColor(byte A,byte R,byte G,byte B):a(A),r(R),g(G),b(B){}
  QapColor(byte R,byte G,byte B):a(255),r(R),g(G),b(B){}
  QapColor(const QapColor& v):a(v.a),r(v.r),g(v.g),b(v.b){}
public:
  QapColor&operator=(const QapColor&v){a=v.a;r=v.r;g=v.g;b=v.b;return *this;}
  QapColor operator+()const{return *this;}
public:
  QapColor&operator*=(const QapColor&v){
  #define F(r){r=Clamp(int(r)*int(v.r)/int(255),int(0),int(255));}
    F(a)F(r)F(g)F(b)
    #undef F
    return *this;
  }
  QapColor&operator+=(const QapColor&v){
  #define F(r){r=Clamp(int(r)+int(v.r),int(0),int(255));}
    F(a)F(r)F(g)F(b)
    #undef F
    return *this;
  }
  QapColor&operator-=(const QapColor&v){
  #define F(r){r=Clamp(int(r)-int(v.r),int(0),int(255));}
    F(a)F(r)F(g)F(b)
    #undef F
    return *this;
  }
  QapColor operator*(const QapColor&v)const{
    #define F(r)int(int(r)*int(v.r))/int(255)
    return QapColor(F(a),F(r),F(g),F(b));
    #undef F
  }
  QapColor operator+(const QapColor&v)const{
    #define F(r)Clamp(int(r)+int(v.r),int(0),int(255))
    return QapColor(F(a),F(r),F(g),F(b));
    #undef F
  }
  QapColor operator-(const QapColor&v)const{
    #define F(r)Clamp(int(r)-int(v.r),int(0),int(255))
    return QapColor(F(a),F(r),F(g),F(b));
    #undef F
  }
public:
  #define F(r){r=byte(Clamp(real(r)*f,0.0,255.0));}
    QapColor&operator*=(real f){F(b);F(g);F(r);return *this;}
    QapColor&operator/=(real r){real f=1.0/r;F(b);F(g);F(r);return *this;}
  #undef F
public:
  #ifdef D3DCOLOR_DEFINED
    QapColor(const D3DCOLOR& v){*((D3DCOLOR*)(void*)this)=v;}
    operator D3DCOLOR&()const{return *(DWORD*)this;};
  #endif
public:
  #ifdef BOX2D_H
    QapColor(const b2Color& v){a=255;r=int(v.r*255.0);g=int(v.g*255.0);b=int(v.b*255.0);}
  #endif
public:
  byte GetLuminance(){return int(int(r)+int(g)+int(b))/int(3);}
  inline static QapColor Mix(const QapColor&A,const QapColor&B,const real&t){
    real ct=Clamp(t,0.0,1.0);
    real tA=1.0-ct;
    real tB=ct;
    QapColor O;
    #define F(r){O.r=byte(Clamp(real(A.r)*tA+real(B.r)*tB,0.0,255.0));}
    F(b)F(g)F(r)F(a)
    #undef F
    return O;
  }
};
//-------------------------------------------//
  union vec4i{
  public:
    struct{int x,y,z,w;};
    struct{int b,g,r,a;};
    vec4i(int b,int g,int r,int a):b(b),g(g),r(r),a(a){}
    vec4i(const QapColor&ref):b(ref.b),g(ref.g),r(ref.r),a(ref.a){}
    vec4i&operator+=(const vec4i&v){b+=v.b;g+=v.g;r+=v.r;a+=v.a;return *this;}
    vec4i operator*(const int&v){return vec4i(x*v,y*v,z*v,w*v);}
    vec4i operator/(const int&v){return vec4i(x/v,y/v,z/v,w/v);}
    vec4i operator+(const vec4i&v){return vec4i(x+v.x,y+v.y,z+v.z,w+v.w);}
    #define F(r)Clamp(int(r),int(0),int(255))
    QapColor GetColor(){return QapColor(F(a),F(r),F(g),F(b));}
    #undef F
  };
  union vec3f{
  public:
    //struct{float b,g,r;};
    struct{float x,y,z;};
    vec3f(){}
    vec3f(float x,float y,float z):x(x),y(y),z(z){}
  };
  struct vec4f{
  public:
    float b,g,r,a;
    //struct{float x,y,z,w;};
    vec4f(){}
    vec4f(float b,float g,float r,float a):b(b),g(g),r(r),a(a){}
    vec4f(const QapColor&ref):b(ref.b/255.f),g(ref.g/255.f),r(ref.r/255.f),a(ref.a/255.f){}
    vec4f&operator+=(const vec4f&v){b+=v.b;g+=v.g;r+=v.r;a+=v.a;return *this;}
    vec4f&operator*=(const float&k){b*=k;g*=k;r*=k;a*=k;return *this;}
    friend vec4f operator*(const float&u,const vec4f&v){return vec4f(u*v.b,u*v.g,u*v.r,u*v.a);}
    friend vec4f operator+(const vec4f&u,const vec4f&v){return vec4f(u.b+v.b,u.g+v.g,u.r+v.r,u.a+v.a);}
    #define F(r)Clamp(int(r*255),int(0),int(255))
    QapColor GetColor(){return QapColor(F(a),F(r),F(g),F(b));}
    #undef F
  };
  class vec2f
  {
  public:
  public:
    typedef vec2f SelfClass;
  public:
    float x;
    float y;
  public:
  public:
    vec2f():x(0),y(0) {}
    vec2f(const vec2d&v)
    {
      x=v.x;
      y=v.y;
    }
    vec2f(float x,float y):x(x),y(y) {};
    void set_zero()
    {
      x=0.0f;
      y=0.0f;
    }
  public:
    friend vec2f operator*(const vec2f&u,const float&v)
    {
      return vec2f(u.x*v,u.y*v);
    }
    friend vec2f operator*(const float&u,const vec2f&v)
    {
      return vec2f(u*v.x,u*v.y);
    }
    friend vec2f operator+(const vec2f&u,const vec2f&v)
    {
      return vec2f(u.x+v.x,u.y+v.y);
    }
    friend vec2f operator-(const vec2f&u,const vec2f&v)
    {
      return vec2f(u.x-v.x,u.y-v.y);
    }
    friend void operator*=(vec2f&ref,float r){ref.x*=r;ref.y*=r;}
  public:
    void operator+=(const vec2d&v)
    {
      x+=v.x;
      y+=v.y;
    }
    void operator-=(const vec2d&v)
    {
      x-=v.x;
      y-=v.y;
    }
  public:
    friend vec2f operator*(float u,const vec2f&v)
    {
      return vec2f(u*v.x,u*v.y);
    }
    operator vec2d()const
    {
      return vec2d(x,y);
    }
  public:
    friend bool operator==(const vec2f&u,const vec2f&v)
    {
      return (u.x==v.x)&&(u.y==v.y);
    }
    friend bool operator!=(const vec2f&u,const vec2f&v)
    {
      return (u.x!=v.x)||(u.y!=v.y);
    }

    vec2f operator-()const{
      return vec2f(-x,-y);
    }
  };
  inline static real dot(const vec2f&a,const vec2f&b){return a.x*b.x+a.y*b.y;}
  inline static real cross(const vec2f&a,const vec2f&b){return a.x*b.y-a.y*b.x;}
  struct vec2i{
    int x,y;
    vec2i(int x,int y):x(x),y(y){};
    explicit vec2i(const vec2d&p){x=int(p.x);y=int(p.y);}
    operator vec2d(){return vec2d(x,y);}
    friend vec2i operator*(int u,const vec2i&v){return vec2i(u*v.x,u*v.y);}
    friend vec2i operator+(const vec2i&u,const vec2i&v){return vec2i(u.x+v.x,u.y+v.y);}
    friend vec2i operator-(const vec2i&u,const vec2i&v){return vec2i(u.x-v.x,u.y-v.y);}
    int SqrMag(){return x*x+y*y;}
    float Mag(){return sqrt(float(x*x+y*y));}
  };
  struct Dip2i{
  public:
    int a,b;
    Dip2i(int a,int b):a(a),b(b){};
    void Take(int x){a=std::min(a,x);b=std::max(b,x);}
    Dip2i Norm()const{return Dip2i(std::min(a,b),std::max(a,b));}
    int Mag()const{return b-a;}
  public:
    struct Transform{
      float x,s;
      Transform(float x,float s):x(x),s(s){}
      Transform(const Dip2i&from,const Dip2i&to){s=float(to.Norm().Mag())/float(from.Norm().Mag());x=float(to.a-from.a);}
      float operator*(int v){return x+v*s;}
    };
  };
//-------------------------------------------//
const vec2d vec2d_zero(0,0);
//-------------------------------------------//
typedef vector<QapColor> ColorArray;
typedef ColorArray* PColorArray;
typedef vector<vec2d> PointArray;
typedef PointArray* PPointArray;
typedef vector<int> IntArray;
typedef IntArray* PIntArray;
//-------------------------------------------//
union TLine{
  struct{vec2d A,B;};
  struct{real ax,ay,bx,by;};
public:
  TLine(){}
  TLine(const vec2d&A,const vec2d&B):A(A),B(B){}
  TLine(const real&ax,const real&ay,const real&bx,const real&by):ax(ax),ay(ay),bx(bx),by(by){}
};
//-------------------------------------------//
inline vec2d Vec2dEx(const real&ang,const real&mag){return vec2d(cos(ang)*mag,sin(ang)*mag);}
inline int round(const real&val){return int(val>=0?val+0.5:val-0.5);}//{return int(val);}
//-------------------------------------------//
#ifdef BOX2D_H
  ///для матрицы X=b2MulXf(A,B) свойственно следующее b2Mul(A,b2Mul(B,p))==b2Mul(X,p) где p - b2Vec2
  inline b2Transform b2MulXf(b2Transform const&A,b2Transform const&B){return b2Transform(b2Mul(B,A.position),b2Mul(B.R,A.R));}
  inline vec2d UnMul(b2Transform&A,vec2d&V){b2Vec2 &m=A.R.col1;real r=1.0/(m.x*m.x+m.y*m.y);return r*vec2d(b2MulT(A,V));}
  inline b2Mat22 MulMat22(const b2Mat22&A,float f){return b2Mat22(vec2d(A.col1)*f,vec2d(A.col2)*f);}
  inline real b2GetZoom(b2Transform&xf){return vec2d(xf.R.col1).Mag();};
  inline b2Transform MakeZoomTransform(const vec2d&Zoom){b2Transform tmp(b2Vec2_zero,b2Mat22(b2Vec2(Zoom.x,0.f),b2Vec2(0.f,Zoom.y)));return tmp;}
#endif
//-------------------------------------------//
//   {<|          03.06.2011           |>}   //
//-------------------------------------------//