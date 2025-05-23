//`#include "QapProClass.inl"
//`multy pass source file
#ifdef DEF_PRO_VARIABLE
//`Настройка макросов окружения
  #define DEF_PRO_MACRO_BLANK(A)
  #define DEF_PRO_STATE_EXIT
  #ifndef ADDVAR
#define ADDVAR \
DEF_PRO_MEMBER(DEF_PRO_MEMBER_ADDVAR)\
DEF_PRO_GETRTTI(DEF_PRO_GETRTTI_ADDVAR)\
DEF_PRO_DORESET(DEF_PRO_DORESET_ADDVAR)
  #endif //ADDVAR//
  #ifndef ADDLINK
#define ADDLINK \
DEF_PRO_MEMBER(DEF_PRO_MEMBER_ADDLINK)\
DEF_PRO_GETRTTI(DEF_PRO_GETRTTI_ADDLINK)\
DEF_PRO_DORESET(DEF_PRO_DORESET_ADDLINK)
  #endif //ADDLINK//
  #ifndef ADDVEC
#define ADDVEC \
DEF_PRO_MEMBER(DEF_PRO_MEMBER_ADDVEC)\
DEF_PRO_GETRTTI(DEF_PRO_GETRTTI_ADDVEC)\
DEF_PRO_DORESET(DEF_PRO_DORESET_ADDVEC)
  #endif //ADDVEC//
  #ifndef ADDSYS
#define ADDSYS \
DEF_PRO_MEMBER(DEF_PRO_MEMBER_ADDSYS)\
DEF_PRO_GETRTTI(DEF_PRO_GETRTTI_ADDSYS)\
DEF_PRO_DORESET(DEF_PRO_DORESET_ADDSYS)
  #endif //ADDSYS//
//`Настройка макросов окружения
  #define DEF_PRO_MEMBER(CODE)
  #define DEF_PRO_GETRTTI(CODE)
  #define DEF_PRO_DORESET(CODE)
//`Сценарий 1a: Используеться для автоматизации работы с полями сущности
  //==Declaration SelfClass and ParentClass==//
  #define DEF_PRO_SELF(SELF)public:typedef SELF SelfClass;
  #define DEF_PRO_OWNER(OWNER)public:typedef OWNER ParentClass;
  DEF_PRO_CLASS_INFO(DEF_PRO_SELF,DEF_PRO_OWNER);
  #undef DEF_PRO_OWNER
  #undef DEF_PRO_SELF
  //==Declaration Members==//
  public:
  #undef DEF_PRO_MEMBER
  #define DEF_PRO_MEMBER(CODE)CODE
    #define DEF_PRO_MEMBER_ADDVAR(TYPE,NAME,NOUSED)TYPE NAME;
    #define DEF_PRO_MEMBER_ADDLINK(TYPE,NAME,NOUSED)Qap::QapPtr<TYPE> NAME;
    #define DEF_PRO_MEMBER_ADDVEC(TYPE,NAME,NOUSED)std::vector<TYPE> NAME;
    #define DEF_PRO_MEMBER_ADDSYS(TYPE,NAME,NOUSED)TYPE NAME;
    DEF_PRO_VARIABLE();
    #undef DEF_PRO_MEMBER_ADDSYS
    #undef DEF_PRO_MEMBER_ADDVEC
    #undef DEF_PRO_MEMBER_ADDLINK
    #undef DEF_PRO_MEMBER_ADDVAR
  #undef DEF_PRO_MEMBER
  #define DEF_PRO_MEMBER(CODE)
  //==Declaration GetRTTI==//
  public:
  virtual void GetRTTI(QapRTTI &Info){
    {
      #define DEF_PRO_OWNER(NOUSED)ParentClass::GetRTTI(Info)
      DEF_PRO_CLASS_INFO(DEF_PRO_MACRO_BLANK,DEF_PRO_OWNER);
      #undef DEF_PRO_OWNER
    }
    #undef DEF_PRO_GETRTTI
    #define DEF_PRO_GETRTTI(CODE)CODE
      #define DEF_PRO_GETRTTI_ADDVAR(TYPE,NAME,NOUSED){Info.AddField(this,QapVIP::Var_##TYPE,#NAME,&this->NAME);};
      #define DEF_PRO_GETRTTI_ADDLINK(TYPE,NAME,NOUSED){Info.AddField(this,Qap::GetLinkType<TYPE>(SToS(#TYPE)),#NAME,&this->NAME);};
      #define DEF_PRO_GETRTTI_ADDVEC(TYPE,NAME,NOUSED){Info.AddField(this,QapVIP::GetVectorType<TYPE>(SToS(#TYPE),QapVIP::Var_##TYPE),#NAME,&this->NAME);};
      #define DEF_PRO_GETRTTI_ADDSYS(TYPE,NAME,NOUSED)
      DEF_PRO_VARIABLE();
      #undef DEF_PRO_GETRTTI_ADDSYS
      #undef DEF_PRO_GETRTTI_ADDVEC
      #undef DEF_PRO_GETRTTI_ADDLINK
      #undef DEF_PRO_GETRTTI_ADDVAR
    #undef DEF_PRO_GETRTTI
    #define DEF_PRO_GETRTTI(CODE)
  }
  //==Declaration GetMethodInfo==//
  #ifdef DEF_PRO_METHOD
  //`Сценарий 1d: используеться для объявления методов
    virtual void GetMethodInfo(StringArray &SA,PtrArray &PA)
    {
      #define DEF_PRO_OWNER(NOUSED)ParentClass::GetMethodInfo(SA,PA)
      DEF_PRO_CLASS_INFO(DEF_PRO_MACRO_BLANK,DEF_PRO_OWNER);
      #undef DEF_PRO_OWNER 
      #define AddMethod(NAME)static void NAME(SelfClass *EX){EX->NAME();};
        class SysHideMethodList{
        public:
          DEF_PRO_METHOD(AddMethod);
        };
      #undef AddMethod
      #define AddMethod(NAME){void* ptr=(void*)&SysHideMethodList::NAME;PA.push_back(ptr);SA.push_back(#NAME);};
        DEF_PRO_METHOD(AddMethod);
      #undef AddMethod
    }
  #endif //DEF_PRO_METHOD
  #undef DEF_PRO_METHOD
  //==Declaration DoReset==//
  #ifndef DEF_PRO_MANUAL_RESET
  //`Сценарий 1c: используеться для инициализации классса
    protected:
    virtual void DoReset(){
      {
        #define DEF_PRO_OWNER(NOUSED)ParentClass::DoReset()
        DEF_PRO_CLASS_INFO(DEF_PRO_MACRO_BLANK,DEF_PRO_OWNER);
        #undef DEF_PRO_OWNER
      }
      #undef DEF_PRO_DORESET
      #define DEF_PRO_DORESET(CODE)CODE
        #define DEF_PRO_DORESET_ADDVAR(NOUSED,NAME,VALUE)(this->NAME)=(VALUE);
        #define DEF_PRO_DORESET_ADDLINK(NOUSED,NAME,VALUE)(this->NAME)=(0);
        #define DEF_PRO_DORESET_ADDVEC(NOUSED,NAME,VALUE)(this->NAME)=(VALUE);
        #define DEF_PRO_DORESET_ADDSYS(NOUSED,NAME,VALUE)(this->NAME)=(VALUE);
        DEF_PRO_VARIABLE();
        #undef DEF_PRO_DORESET_ADDSYS
        #undef DEF_PRO_DORESET_ADDVEC
        #undef DEF_PRO_DORESET_ADDLINK
        #undef DEF_PRO_DORESET_ADDVAR
      #undef DEF_PRO_DORESET
      #define DEF_PRO_DORESET(CODE)
    }
  #endif //DEF_PRO_MANUAL_RESET//
  #undef DEF_PRO_MANUAL_RESET
  //==Declaration GetStaticClassFactory==/
  public: 
    static Qap::QapFactory* GetStaticClassFactory();
    //`error C2523 - несовпадение тегов деструктор чё за?
    #define DEF_PRO_CLASSNAME(NAME)NAME()
    //`DEF_PRO_CLASS_INFO(DEF_PRO_CLASSNAME,DEF_PRO_MACRO_BLANK){MACRO_ADD_LOG("ClassName["+GetClassName()+"]",lml_EVENT);};
    #undef DEF_PRO_CLASSNAME
    #define DEF_PRO_CLASSNAME(NAME)~NAME()
    //`DEF_PRO_CLASS_INFO(DEF_PRO_CLASSNAME,DEF_PRO_MACRO_BLANK){MACRO_ADD_LOG("Caption("+Caption+")",lml_EVENT);};
    #undef DEF_PRO_CLASSNAME
  public:
  //==Declaration Tweak==/
  #include "QapProTweak.inl"
//`убиваем макросы окружения
  #undef DEF_PRO_MEMBER
  #undef DEF_PRO_GETRTTI
  #undef DEF_PRO_DORESET
//`убиваем возмжно системные макросы
  #undef ADDSYS
  #undef ADDVEC
  #undef ADDLINK
  #undef ADDVAR
//`убиваем макросы определённые пользователем
  #undef DEF_PRO_MACRO_BLANK
  #undef DEF_PRO_CLASS_INFO
  #undef DEF_PRO_VARIABLE
#endif //DEF_PRO_VARIABLE//


#ifdef DEF_PRO_REGCLASS
//`Сценарий 2: Используеться в main.cpp для объявления всех фабрик сущностей.
  #define DEF_PRO_STATE_EXIT 
  #define ADDSCOPEX(SCOPE)ADDSCOPE_XUY("StackOverFlow");
  #define ADDSCOPE0(SCOPE)ADDSCOPE_BEF(SCOPE);DEF_SCOPE_##SCOPE(ADDSCOPEX,ADDCLASS);ADDSCOPE_AFT(SCOPE);
  #define ADDSCOPE1(SCOPE)ADDSCOPE_BEF(SCOPE);DEF_SCOPE_##SCOPE(ADDSCOPE0,ADDCLASS);ADDSCOPE_AFT(SCOPE);
  #define ADDSCOPE2(SCOPE)ADDSCOPE_BEF(SCOPE);DEF_SCOPE_##SCOPE(ADDSCOPE1,ADDCLASS);ADDSCOPE_AFT(SCOPE);
  #define ADDSCOPE3(SCOPE)ADDSCOPE_BEF(SCOPE);DEF_SCOPE_##SCOPE(ADDSCOPE2,ADDCLASS);ADDSCOPE_AFT(SCOPE);
  #define ADDSCOPE4(SCOPE)ADDSCOPE_BEF(SCOPE);DEF_SCOPE_##SCOPE(ADDSCOPE3,ADDCLASS);ADDSCOPE_AFT(SCOPE);
  #define ADDSCOPE5(SCOPE)ADDSCOPE_BEF(SCOPE);DEF_SCOPE_##SCOPE(ADDSCOPE4,ADDCLASS);ADDSCOPE_AFT(SCOPE);
  #define ADDSCOPE6(SCOPE)ADDSCOPE_BEF(SCOPE);DEF_SCOPE_##SCOPE(ADDSCOPE5,ADDCLASS);ADDSCOPE_AFT(SCOPE);
  #define ADDSCOPE7(SCOPE)ADDSCOPE_BEF(SCOPE);DEF_SCOPE_##SCOPE(ADDSCOPE6,ADDCLASS);ADDSCOPE_AFT(SCOPE);
  #define ADDSCOPE_STD(SCOPE)ADDSCOPE7(SCOPE)

  #ifdef DEF_PRO_LOL_STATE_XUY
    Fuck_You+Fuck_You=Fuck_You!// ТРЕВОГА("майндфак!!!);"
  #else
    #define DEF_PRO_LOL_STATE_XUY
  #endif

  #define ADDSCOPE_BEF(SCOPE)
  #define ADDSCOPE_AFT(SCOPE)
  #define ADDCLASS(TYPE)Qap::Factory<TYPE> *T##TYPE; 
    namespace Qap
    {
      DEF_PRO_REGCLASS(ADDSCOPE_STD,ADDCLASS);
    };
  #undef ADDCLASS
  #undef ADDSCOPE_AFT
  #undef ADDSCOPE_BEF
  //==***==//
  #define ADDSCOPE_BEF(SCOPE)
  #define ADDSCOPE_AFT(SCOPE)
  #define ADDCLASS(TYPE)\
    Qap::QapFactory*TYPE::GetStaticClassFactory(){return Qap::T##TYPE;};;\
    //string TYPE::GetClassName(){return #TYPE;};;\
    //string TYPE::GetStaticClassName(){return #TYPE;};;\
    //uint TYPE::GetClassID(){return QapVIP::TYPE##_ClassID;};;\
    //uint TYPE::GetStaticClassID(){return QapVIP::TYPE##_ClassID;};;\
    //Qap::QapFactory* TYPE::GetClassFactory(){return Qap::T##TYPE;};;
  //==***==//
    DEF_PRO_REGCLASS(ADDSCOPE_STD,ADDCLASS);
  #undef ADDCLASS
  #undef ADDSCOPE_AFT
  #undef ADDSCOPE_BEF
  //==|||==//
  void RegEntityClass()
  { 
    #define ADDSCOPE_BEF(SCOPE)
    #define ADDSCOPE_AFT(SCOPE)
    #define ADDCLASS(TYPE){static Qap::Factory<TYPE> StaticFactory(#TYPE);Qap::RegItemClass(&StaticFactory);Qap::T##TYPE=&StaticFactory;};
      DEF_PRO_REGCLASS(ADDSCOPE_STD,ADDCLASS);
    #undef ADDCLASS
    #undef ADDSCOPE_AFT
    #undef ADDSCOPE_BEF

    #define ADDSCOPE_BEF(SCOPE){;;Qap::ScopeList.push_back(FL);Qap::ScopeName.push_back(#SCOPE);
    #define ADDSCOPE_AFT(SCOPE)Qap::ScopeList[ScopeID++]=FL;FL.clear();;};
   // #define ADDSCOPE(SCOPE){;;Qap::ScopeList.push_back(FL);Qap::ScopeName.push_back(#SCOPE);;ADDSCOPE_STD(SCOPE);;Qap::ScopeList[ScopeID++]=FL;FL.clear();;};;
    #define ADDCLASS(TYPE)FL.push_back(Qap::T##TYPE);
      vector<Qap::QapFactory*> FL;int ScopeID=0;
      DEF_PRO_REGCLASS(ADDSCOPE_STD,ADDCLASS);
    #undef ADDCLASS
    #undef ADDSCOPE_AFT
    #undef ADDSCOPE_BEF
  }
  #undef ADDSCOPE_STD
  #undef ADDSCOPE7
  #undef ADDSCOPE6
  #undef ADDSCOPE5
  #undef ADDSCOPE4
  #undef ADDSCOPE3
  #undef ADDSCOPE2
  #undef ADDSCOPE1
  #undef ADDSCOPE0
  #undef ADDSCOPEX
  #undef DEF_PRO_REGCLASS
#endif //DEF_PRO_REGCLASS//

#ifdef DEF_PRO_DECLARE_SCOPE
//`Сценарий 3: Используеться для предобъявления скопа классов сущностей в header'е
  #define ADDSCOPEX(SCOPE)ADDSCOPE_XUY("StackOverFlow");
  #define ADDSCOPE0(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPEX,ADDCLASS);
  #define ADDSCOPE1(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPE0,ADDCLASS);
  #define ADDSCOPE2(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPE1,ADDCLASS);
  #define ADDSCOPE3(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPE2,ADDCLASS);
  #define ADDSCOPE4(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPE3,ADDCLASS);
  #define ADDSCOPE5(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPE4,ADDCLASS);
  #define ADDSCOPE6(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPE5,ADDCLASS);
  #define ADDSCOPE7(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPE6,ADDCLASS);
  #define ADDCLASS(F)class F;
  DEF_PRO_DECLARE_SCOPE(ADDSCOPE7,ADDCLASS);
  #undef ADDCLASS
  #undef ADDSCOPE7
  #undef ADDSCOPE6
  #undef ADDSCOPE5
  #undef ADDSCOPE4
  #undef ADDSCOPE3
  #undef ADDSCOPE2
  #undef ADDSCOPE1
  #undef ADDSCOPE0
  #undef ADDSCOPEX
  #define DEF_PRO_STATE_EXIT 
  #undef DEF_PRO_DECLARE_SCOPE
#endif //DEF_PRO_DECLARE_SCOPE//

#ifdef DEF_PRO_STATE_EXIT
  #undef DEF_PRO_STATE_EXIT
#else
//`Сценарий 4: Используеться для предобъявления всех классов сущностей в header'е
  #define ADDSCOPEX(SCOPE)ADDSCOPE_XUY("StackOverFlow");
  #define ADDSCOPE0(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPEX,ADDCLASS);
  #define ADDSCOPE1(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPE0,ADDCLASS);
  #define ADDSCOPE2(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPE1,ADDCLASS);
  #define ADDSCOPE3(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPE2,ADDCLASS);
  #define ADDSCOPE4(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPE3,ADDCLASS);
  #define ADDSCOPE5(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPE4,ADDCLASS);
  #define ADDSCOPE6(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPE5,ADDCLASS);
  #define ADDSCOPE7(SCOPE)DEF_SCOPE_##SCOPE(ADDSCOPE6,ADDCLASS);
  #ifdef DEF_PRO_REGCLASS_EXTERN
    #define ADDCLASS(TYPE)extern Qap::Factory<TYPE> *T##TYPE;
    namespace Qap{DEF_PRO_REGCLASS_EXTERN(ADDSCOPE2,ADDCLASS);};
    #undef ADDCLASS
  #endif //DEF_PRO_REGCLASS_EXTERN//
  #undef ADDSCOPE7
  #undef ADDSCOPE6
  #undef ADDSCOPE5
  #undef ADDSCOPE4
  #undef ADDSCOPE3
  #undef ADDSCOPE2
  #undef ADDSCOPE1
  #undef ADDSCOPE0
  #undef ADDSCOPEX
  #undef DEF_PRO_STATE_EXIT
#endif //DEF_PRO_STATE_EXIT//