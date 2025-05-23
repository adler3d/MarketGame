/*------------------------------------------*/
/*   {<|author : Adler3D(Kashin Ivan) |>   }*/
/*   {<|e-mail : Adler3D@gmail.com    |>   }*/
/*   {<|site   : Adler3d.narod.ru     |>   }*/
/*------------------------------------------*/
#pragma once
/************************************************************************/
/* Подключаем всякое разное                                             */
/************************************************************************/
#include "QapType.h"
#include "QapSys.h"
#include "QapVec.h"
#include "QapTools.h"
/************************************************************************/
/* define                                                               */
/************************************************************************/
//////////////////////////////////////////////////////////////////////////
//#define DummyTypeReg(TYPE)                                               \
//  virtual void Write_##TYPE(TYPE &value){SavePlainData(&value,sizeof(TYPE));}    \
//  virtual void Read_##TYPE(TYPE &value){LoadPlainData(&value,sizeof(TYPE));}     \
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* Классы                                                               */
/************************************************************************/
template<typename TYPE>inline vector<TYPE> Make_vector(int Count,...){
  vector<TYPE> Arr;Arr.resize(Count);
  va_list vl;va_start(vl,Count);for(int i=0;i<Count;i++)Arr[i]=va_arg(vl,TYPE);
  return Arr;
}

#define DEF_PRO_REGTYPE_EXTERN(F);F(int);F(vec2d);F(QapColor);F(real);
  #define ADDVAR(TYPE)inline vector<TYPE> Make_vector_##TYPE(int Count,...)\
  {vector<TYPE> Arr;Arr.resize(Count);va_list vl;va_start(vl,Count);for(int i=0;i<Count;i++)Arr[i]=va_arg(vl,TYPE);return Arr;}
    DEF_PRO_REGTYPE_EXTERN(ADDVAR);
  #undef ADDVAR
#undef DEF_PRO_REGTYPE_EXTERN

#include "NakedIO.h"
class QapIO
{
public:
  virtual void SavePlainData(void *value,uint count);
  virtual void LoadPlainData(void *value,uint count);
public:
   CrutchIO *data;
};
class QapGoodIO:public QapIO
{
public:
  void clear(){mem.clear();pos=0;}
public:
  CrutchIO IO;
  int &pos;
  string &mem;
  QapGoodIO():pos(IO.pos),mem(IO.mem){data=&IO;}
}; 
/*template<typename TYPE>
class prop{
public:
  prop<TYPE>(TYPE &v):content(V){};
  operator (TYPE)prop<TYPE>(TYPE &v):content(V){};
public:
  TYPE &content;
};*/
/************************************************************************/
/* Abstract Type                                                        */
/************************************************************************/
struct QapType
{
public:
  typedef void(*TFunc)(QapIO &IO,void *value);
  typedef void(*TStrFunc)(string &str,void *p);
  //typedef void(*TGrabProcCallBack)(PtrArray &PA,void *p,void *UserData);
  typedef void(*TGrabFunc)(PtrArray &PA,void *p,QapType *A);
  enum FieldType{FT_Unknown,FT_PlainData,FT_SmartPointer,FT_Vector,FT_Struct};
public:
  template<typename TYPE>static void DefSave(QapIO &IO,void *value){IO.SavePlainData(value,sizeof(TYPE));}
  template<typename TYPE>static void DefLoad(QapIO &IO,void *value){IO.LoadPlainData(value,sizeof(TYPE));}
public:
  QapType(FieldType type,string name,TFunc save=NULL,TFunc load=NULL,TStrFunc str=NULL,QapType *sub=NULL,TGrabFunc grab=NULL,TStrFunc code=NULL)
  {
    ID=-1;//уникальный индификатор зарегестрированного типа.
    Type=type;//класс типа данных.
    Name=name;//уникальное имя типа данных 
    Save=save;//функция сохрнаяющая в бинарную строку
    Load=load;//функция загружающая из бинарной строки
    StrFunc=str;//функция перевадащая 
    SubType=sub;//указатель на фложенный тип.
    GrabFunc=grab;//функция собирающая умные ссылки.
    GetCode=code;//функция сохраняющая в С++ код.
  };
  QapType()
  {
    ID=-1;
    Type=FT_Unknown;
    Name="Unknown";
    Save=NULL;
    Load=NULL;
    StrFunc=NULL;
    SubType=NULL;
    GrabFunc=NULL;
    GetCode=NULL;
  };
public:
  int ID;
  FieldType Type;
  string Name;
  TFunc Save,Load;
  TStrFunc StrFunc;
  QapType *SubType;
  TGrabFunc GrabFunc;
  TStrFunc GetCode;
  /*
  TProc Restore;
  TProc Free;*/
};
/************************************************************************/
/* Something To String                                                  */
/************************************************************************/
inline string VToS(vec2d Value){return FToS(Value.x)+" "+FToS(Value.y);}
inline string RealToCode(real value){std::stringstream ss;ss<<value;return ss.str();}
inline string Vec2dToCode(vec2d Value){return "vec2d("+RealToCode(Value.x)+","+RealToCode(Value.y)+")";}
inline string CharToString(char c){return "'"+CToS(c)+"'";}
/************************************************************************/
/* Holy Shit                                                            */
/************************************************************************/
#ifdef USE_MACRO_BUILD
  #include "..\Macro\HolyShit.h"
#else
//=====>>>>>HolyShit
/*(<MacroCode>)*/  
namespace QapVIP
{
  #define DEF_PRO_REGTYPE_EXTERN(F)\
    F(int,IToS,IToS);\
    F(uint,IToS,IToS);\
    F(real,FToS,RealToCode);\
    F(bool,BToS,BToS);\
    F(vec2d,VToS,Vec2dToCode);\
    F(string,SToS,SToS);\
    F(QapColor,IToH,IToH);\
    F(char,CToS,CharToString);
  #define RegType(TYPE,FUNC,CODE)\
  inline void SaveToStr_##TYPE(string &str,void *p){str+=FUNC(*(TYPE*)p);}\
  inline void GetCode_##TYPE(string &str,void *p){str+=CODE(*(TYPE*)p);}
//=====+>>>>>QapIO_RegType
    DEF_PRO_REGTYPE_EXTERN(RegType);
//<<<<<+=====QapIO_RegType
  #undef RegType
  #undef DEF_PRO_REGTYPE_EXTERN
  template<typename TYPE>inline void SaveToStrVector(string &str,void *p){str="{..."+IToS(((vector<TYPE>*)p)->size())+"...}";}
  inline void GetCodeString(string &str,void *p){str+="\""+(*(string*)p)+"\"";}
  inline void LoadString(QapIO &F,void *p)
  {
    string &s=*(string*)p;
    int tmp=0;F.LoadPlainData(&tmp,sizeof(tmp));s.resize(tmp);
    if(tmp)F.LoadPlainData(&s[0],tmp);
  };
  inline void SaveString(QapIO &F,void *p)
  {
    string &s=*(string*)p;
    int tmp=s.size();F.SavePlainData(&tmp,sizeof(tmp));
    if(tmp)F.SavePlainData(&s[0],tmp);
  };
  inline void GrabPointer(PtrArray &PA,void *p,QapType *A)
  {
    if(!A)return;
    PA.push_back(p);
  }
  template<typename TYPE>
  inline void GrabVector(PtrArray &PA,void *p,QapType *A)
  {
    if(!A||!A->SubType||!A->SubType->GrabFunc)return;
    vector<TYPE> &v=*(vector<TYPE>*)p;
    for(int i=0;i<v.size();i++)
    {
      A->SubType->GrabFunc(PA,&v[i],A->SubType);
    }
  }
  template<typename TYPE>
  void LoadVector(QapIO &F,void *p)
  {
    vector<TYPE> &v=*(vector<TYPE>*)p;
    int tmp=0;F.LoadPlainData(&tmp,sizeof(tmp));v.resize(tmp);
    if(tmp>0)F.LoadPlainData(&v[0],tmp*sizeof(TYPE));
  }
  template<typename TYPE>
  void SaveVector(QapIO &F,void *p)
  {
    vector<TYPE> &v=*(vector<TYPE>*)p;
    int tmp=v.size();F.SavePlainData(&tmp,sizeof(tmp));
    if(tmp>0)F.SavePlainData(&v[0],tmp*sizeof(TYPE));
  }
  template<typename TYPE>
  void LoadSoftVector(QapIO &F,void *p)
  {
    vector<TYPE::TSubType> &v=*(vector<TYPE::TSubType>*)p;
    int tmp=0;F.LoadPlainData(&tmp,sizeof(tmp));v.resize(tmp);
    QapType::TFunc Func=TYPE::GetSubLoadFunc();
    for(int i=0;i<tmp;i++){Func(F,&v[i]);}
  }
  template<typename TYPE>
  void SaveSoftVector(QapIO &F,void *p)
  {
    vector<TYPE::TSubType> &v=*(vector<TYPE::TSubType>*)p;
    int tmp=v.size();F.SavePlainData(&tmp,sizeof(tmp));
    QapType::TFunc Func=TYPE::GetSubSaveFunc();
    for(int i=0;i<tmp;i++){Func(F,&v[i]);}
  }
  template<typename TYPE>
  void GetCodeVector(string &str,void *p)
  {
    vector<TYPE::TSubType> &v=*(vector<TYPE::TSubType>*)p;
    int tmp=v.size();
    QapType::TStrFunc Func=TYPE::GetSubCodeFunc();
    string s;for(int i=0;i<v.size();i++){s+=",";Func(s,&v[i]);}
    str+="Make_vector_"+TYPE::GetSubName()+"("+IToS(tmp)+s+")";
  }
  template<typename TYPE>
  inline QapType GetVectorType(string &SubName,QapType const&SubType)
  {
    //static bool runed=false;
    class TUberCall{
    public:
      typedef TYPE TSubType;
      static void Hack(QapType *GetValue,QapType const*SetValue=NULL)
      {
        static QapType s_SubType;
        if(SetValue)s_SubType=*SetValue;
        if(GetValue)*GetValue=s_SubType;
      }
      static QapType::TFunc GetSubLoadFunc(){QapType tmp;Hack(&tmp);return tmp.Load;}
      static QapType::TFunc GetSubSaveFunc(){QapType tmp;Hack(&tmp);return tmp.Save;}
      static QapType::TStrFunc GetSubCodeFunc(){QapType tmp;Hack(&tmp);return tmp.GetCode;}
      static string GetSubName(){QapType tmp;Hack(&tmp);return tmp.Name;}
    };
    static QapType sub=SubType;
    TUberCall::Hack(NULL,&sub);
    return QapType(QapType::FT_Vector,"vector<"+SubName+">",SaveSoftVector<TUberCall>,LoadSoftVector<TUberCall>,SaveToStrVector<TYPE>,&sub,GrabVector<TYPE>,&GetCodeVector<TUberCall>);
  }
  const QapType Var_string(QapType::FT_PlainData,"string",SaveString,LoadString,QapVIP::SaveToStr_string,NULL,NULL,GetCodeString);
  //const QapType Var_PointArray(QapType::FT_Vector,"PointArray",SaveVector<vec2d>,LoadVector<vec2d>,SaveToStrVector<vec2d>);
  //const QapType Var_IntArray(QapType::FT_Vector,"IntArray",SaveVector<int>,LoadVector<int>,SaveToStrVector<int>);
}
#define DEF_PRO_REGTYPE_EXTERN(F)F(bool);F(int);F(uint);F(real);F(char);F(vec2d);F(QapColor);
namespace QapVIP
{
  #define ADDVAR(TYPE)const QapType Var_##TYPE(QapType::FT_PlainData,#TYPE,QapType::DefSave<TYPE>,QapType::DefLoad<TYPE>,QapVIP::SaveToStr_##TYPE,NULL,NULL,GetCode_##TYPE);
    DEF_PRO_REGTYPE_EXTERN(ADDVAR);
  #undef ADDVAR
}
#undef DEF_PRO_REGTYPE_EXTERN
/*(</MacroCode>)*/
//<<<<<=====HolyShit
#endif
/************************************************************************/
/* Объявления                                                           */
/************************************************************************/
/************************************************************************/
/* Конец                                                                */
/************************************************************************/
