#ifdef PRO_VARIABLE
  public:
  #define ADDVAR(TYPE,NAME,VALUE)TYPE NAME;
    PRO_VARIABLE();
  #undef ADDVAR
  public:
  void DoReset()
  {
    #define ADDVAR(TYPE,NAME,VALUE)NAME=VALUE;
      PRO_VARIABLE();
    #undef ADDVAR
  }
  #undef PRO_VARIABLE
#endif //PRO_VARIABLE