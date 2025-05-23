class TCounterInc{
#define PRO_VARIABLE()\
ADDVAR(int,Value,0)\
ADDVAR(int,Minimum,0)\
ADDVAR(int,Maximum,32)
//=====+>>>>>TCounterInc
#include "GenVar.inl"
//<<<<<+=====TCounterInc
public:
  TCounterInc(){DoReset();}
  TCounterInc(int Value,int Minimum,int Maximum){this->Value=Value;this->Minimum=Minimum;this->Maximum=Maximum;}
  void Start(){Value=Minimum;}
  void Stop(){Value=Maximum;}
  operator bool(){return Value<Maximum;}
  void operator++(){Value++;}
  void operator++(int){Value++;}
  int DipSize(){return Maximum-Minimum;}
  operator string(){return IToS(Value)+"/"+IToS(DipSize());}
};

class TCounterIncEx{
#define PRO_VARIABLE()\
ADDVAR(int,Value,0)\
ADDVAR(int,Minimum,0)\
ADDVAR(int,Maximum,32)\
ADDVAR(bool,Runned,false);
//=====+>>>>>TCounterIncEx
#include "GenVar.inl"
//<<<<<+=====TCounterIncEx
public:
  TCounterIncEx(){DoReset();}
  TCounterIncEx(int Value,int Minimum,int Maximum){DoReset();this->Value=Value;this->Minimum=Minimum;this->Maximum=Maximum;}
  void Start(){Value=Minimum;Runned=true;}
  void Stop(){Value=Minimum;Runned=false;}
  operator bool(){return Value<Maximum;}
  void operator++(){if(Runned)Value++;}
  void operator++(int){if(Runned)Value++;}
  int DipSize(){return Maximum-Minimum;}
  operator string(){return IToS(Value)+"/"+IToS(DipSize());}
};