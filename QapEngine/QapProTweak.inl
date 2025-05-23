#ifdef DEF_PRO_TWEAK
  #define ADDTWEAK(NAME,VALUE)bool NAME;
    struct TSingleState{DEF_PRO_TWEAK(ADDTWEAK)};
  #undef ADDTWEAK
  #define ADDTWEAK(NAME,VALUE)sys.NAME=VALUE;
    static TSingleState&SysGetState(){static TSingleState sys;static bool guard=false;if(!guard){guard=true;DEF_PRO_TWEAK(ADDTWEAK);}return sys;}
  #undef ADDTWEAK
  virtual void GetTweakList(StringArray&SA,PtrArray&PA){
    ParentClass::GetTweakList(SA,PA);
    #define ADDTWEAK(NAME,VALUE)static void NAME(){SysGetState().NAME=!SysGetState().NAME;};
    class SysHideTweak{public:DEF_PRO_TWEAK(ADDTWEAK);};
    #undef ADDTWEAK
    #define ADDTWEAK(NAME,VALUE)SA.push_back("^3"#NAME" ^2: ^3"+BToS(SysGetState().NAME));PA.push_back(&SysHideTweak::NAME);
    DEF_PRO_TWEAK(ADDTWEAK);
    #undef ADDTWEAK
  }
  #undef DEF_PRO_TWEAK
#endif //DEF_PRO_TWEAK//