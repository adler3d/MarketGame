#pragma once
//-------------------------------------------//
//   {<| author : Adler3D(Kashin Ivan) |>}   //
//   {<| e-mail : Adler3D@gmail.com    |>}   //
//   {<| site   : Adler3d.narod.ru     |>}   //
//-------------------------------------------//
#include "QapType.h"
//-------------------------------------------//
template<typename TYPE>
class QapPool{
public:
  struct Rec
  {
    bool used;
    TYPE data;
    Rec():used(false){}
  };
  vector<Rec>Arr;
  int Size;
  int MaxSize;
public:
  QapPool(int MaxSize=0):Size(0),MaxSize(MaxSize){Arr.resize(MaxSize);}
  void NewInstance(TYPE*&pVar)
  {
    QapAssert(Size<MaxSize);
    for(int i=0;i<Arr.size();i++)
    {
      if(!Arr[i].used)
      {
        Arr[i].used=true;
        Size++;
        pVar=&Arr[i].data;
        return;
      }
    }
  }
  void FreeInstance(TYPE*&pVar){
    QapAssert(Size>0);
    int id=int((int)pVar-(int)&Arr[0].data)/sizeof(Arr[0]);
    for(int i=id;i<Arr.size();i++)
    {
      if(&Arr[i].data==pVar)
      {
        Arr[i].used=false;
        Size--;
        pVar=NULL;
        return;
      }
    }
    QapAssert(pVar=NULL);
  }
  template<typename TYPE>
  void ForEach(TYPE&Func)
  {
    int c=Size;
    for(int i=0;i<Arr.size();i++)
    {
      if(!c)break;
      Rec&it=Arr[i];
      if(it.used)
        Func(&it.data);
    }
  }
};
//-------------------------------------------//
//   {<|          28.06.2011           |>}   //
//-------------------------------------------//