//#include "QapProClass.cpp"
//multy pass source file
//==Declaration Field==//
#define ADDVAR(TYPE,NAME,NOUSED)TYPE NAME;
public: DEF_PRO_VARIABLE();
#undef ADDVAR
//==Declaration DoSave==//
protected:
virtual void DoSave(QapIO F){
  #define ADDVAR(TYPE,NAME,NOUSED) F.Write_##TYPE(NAME);
  DEF_PRO_VARIABLE();
  #undef ADDVAR
  #ifdef DEF_PRO_OWNER
  DEF_PRO_OWNER::DoSave(F);
  #endif
}
//==Declaration DoLoad==//
protected:
virtual void DoLoad(QapIO F){
  #define ADDVAR(TYPE,NAME,NOUSED) F.Read_##TYPE(NAME);
  DEF_PRO_VARIABLE();
  #undef ADDVAR
  #ifdef DEF_PRO_OWNER
  DEF_PRO_OWNER::DoLoad(F);
  #endif 
}
//==Declaration DoReset==//
protected:
virtual void DoReset(){
  #define ADDVAR(NOUSED,NAME,VALUE) NAME=VALUE;
  DEF_PRO_VARIABLE();
  #undef ADDVAR
  #ifdef DEF_PRO_OWNER
  DEF_PRO_OWNER::DoReset();
  #endif 
}
//==Declaration GetClassID==//
public:
  virtual uint GetClassID(){return QapVID::ClassID;};
#undef DEF_PRO_OWNER
#undef DEF_PRO_VARIABLE