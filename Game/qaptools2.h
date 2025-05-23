#pragma once
//-------------------------------------------//
//   {<| author : Adler3D(Kashin Ivan) |>}   //
//   {<| e-mail : Adler3D@gmail.com    |>}   //
//   {<| site   : Adler3d.narod.ru     |>}   //
//-------------------------------------------//
#include "QapType.h"
#include "QapAssert.h"
//-------------------------------------------//
class Extractor{
public:
  class Gripper{
  public:
    string before;
    string after;
    Gripper(const string&before,const string&after):before(before),after(after){}
    string grip(const string&source)const{return before+source+after;}
  };
public:
  int pos;
  string source;
  Extractor(const string&source):pos(0),source(source){}
  string extract(const Gripper&ref){
    string s=ScanParam(ref.before,source,ref.after,pos);
    if(pos>=0)pos+=s.size()+ref.after.size();
    return s;
  }
  template<class TYPE>
  void extract_all(TYPE&visitor,const Gripper&ref)
  {
    while(pos>=0){
      string s=extract(ref);
      if(pos<0)break;
      visitor.accept(s);
    }
  }
  operator bool(){return pos>=0;}
};
//-------------------------------------------//
//-------------------------------------------//
static string gen_dip(char from,char to){
  QapAssert(from<to);
  string out;
  out.reserve(to-from);
  bool flag=from!=to;
  for(auto c=from;flag;c++){
    flag=flag&&(c!=to);
    out.push_back(c);
  }
  return out;
}
//-------------------------------------------//
static string gen_dips(const string&rule){
  QapAssert(!(rule.size()%2));
  string out;
  for(int i=0;i<rule.size();i+=2){
    out+=gen_dip(rule[i+0],rule[i+1]);
  }
  return out;
}
//-------------------------------------------//
static string dip_inv(const string&dip){
  string out;
  char min=std::numeric_limits<char>::min();
  char max=std::numeric_limits<char>::max();
  bool flag=min!=max;
  for(auto c=min;flag;c++){
    flag=flag&&(c!=max);
    auto e=dip.find(CToS(c));
    if(e!=std::string::npos)continue;
    out.push_back(c);
  }
  return out;
}
//-------------------------------------------//
static vector<string> split(const string&s,const string&needle)
{
  vector<string> arr;
  if(s.empty())return arr;
  size_t p=0;
  for(;;){
    auto pos=s.find(needle,p);
    if(pos==std::string::npos){arr.push_back(s.substr(p));return arr;}
    arr.push_back(s.substr(p,pos-p));
    p=pos+needle.size();
  }
  return arr;
}
//-------------------------------------------//
static string join(const vector<string>&arr,const string&glue)
{
  string out;
  size_t c=0;
  size_t dc=glue.size();
  for(int i=0;i<arr.size();i++){if(i)c+=dc;c+=arr[i].size();}
  out.reserve(c);
  for(int i=0;i<arr.size();i++){if(i)out+=glue;out+=arr[i];}
  return out;
}
//-------------------------------------------//
inline string StrReplace(const string&input,const string&sub,const string&now)
{
  return join(split(input,sub),now);
  //old version is slow and wrong
  //string result=input;
  //for(unsigned index=0;index=result.find(sub,index),index!=string::npos;)
  //{
  //  result.replace(index,sub.length(),now);
  //  index+=now.length();
  //}
  //return std::move(result);
}
//-------------------------------------------//
//   {<|          04.08.2014           |>}   //
//-------------------------------------------//