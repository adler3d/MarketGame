#include "stdafx.h"
#include "QapEntity.h"
//#include <typeinfo>
/************************************************************************/
/* Объявления                                                           */
/************************************************************************/
namespace Qap
{
  vector<QapFactory*> FactoryList;
  vector<string> ScopeName;
  vector<vector<QapFactory*>> ScopeList;
  vector<QapType> TypeList;
/************************************************************************/
/* Реализация                                                           */
/************************************************************************/
  /************************************************************************/
  /* System procedure                                                     */
  /************************************************************************/
  QapItem* DefCreateItemFromID(QapList* Owner,uint ID)
  {
    if((ID<0)||ID>=FactoryList.size()){MACRO_ADD_LOG("Invalid ClassID - "+IToS(ID),lml_ERROR);return NULL;}
    return FactoryList[ID]->Create(Owner);
  }
  TCreateItemFromID CreateItemFromID=DefCreateItemFromID;
  QapFactory* RegItemClass(QapFactory* RegFac)
  {
    FactoryList.push_back(RegFac); 
    RegFac->FactoryID=FactoryList.size()-1;
    return RegFac;
  }
  QapType* RegType(QapType const&Type,QapRTTI *RTTI)
  {
    const int MAX_TYPE_COUNT=1024;
    QapType *sub=NULL;
    if(Type.SubType)
      sub=RegType(*Type.SubType,RTTI);
    if(TypeList.empty()||TypeList.size()==MAX_TYPE_COUNT)
    {
      if(!TypeList.empty())MACRO_ADD_LOG("Epic fail...",lml_EVENT);
      //А теперь внимание! Это лол.
      TypeList.reserve(MAX_TYPE_COUNT);//я надеюсь, что переполнения не произойдёт.
    }
    for(int i=0;i<TypeList.size();i++)
      if(Type.Name==TypeList[i].Name)
      {
        return &TypeList[i];
      }
    //MACRO_ADD_LOG("'"+Type.Name+"' - Registered",lml_EVENT);
    TypeList.push_back(Type);
    int id=TypeList.size()-1;
    QapType &ret=TypeList[id];
    ret.ID=id;
    ret.SubType=sub;
    return &ret;
  }
  //===========================================//
  void IncPostfix(string&Str,string Postfix)
  {
    int e=0;string num=ScanParam("_"+Postfix+"(",Str,")",e);
    if(!num.empty())
    {
      int q=0;string s=ScanParam("\n","\n"+Str,"_"+Postfix+"(",q);
      Str=s+"_"+Postfix+"("+IToS(SToI(num)+1)+")";return;
    }
    Str+="_"+Postfix+"(0)";
  }
  void AddCaptionPostfix(QapItem *EX,string Postfix){IncPostfix(EX->Caption,Postfix);}
  bool FindParent(QapItem *Item,QapList *Owner)
  {
    if(!Item)return false;
    if(Item==Owner)return true;
    return FindParent(Item->Parent,Owner);
  }
  void Recursive(QapList *A,RecFunc Func,void* Data)
  {
    Func(A,Data);
    for(int i=0;i<A->Count();i++)
    {
      QapItem *it=A->Items[i];    
      if(it->IsList()){Recursive((QapList*)it,Func,Data);continue;}
      Func(it,Data);
    }
  }
  void ItemGrabLinks(QapItem *EX,void* Data)
  {
    PtrArray *LA=(PtrArray*)Data;
    PtrArray tmp;
    EX->Factory->GrabLinks(EX,tmp);
    for(int i=0;i<tmp.size();i++)
    {
      QapPtr<QapItem> *EX=(QapPtr<QapItem>*)tmp[i];
      if(*EX)LA->push_back(EX->ptr);
    }
  }
  void GrabExternalLinks(QapList *Root,IntArray &IA)
  {
    if(!Root)return;
    vector<QapItem*> LA;
    Qap::Recursive((QapList*)Root,&Qap::ItemGrabLinks,&LA);
    for(int i=0;i<LA.size();i++)LA[i]->SaveID=NULL_ID;
    for(int i=0;i<LA.size();i++){
      QapItem *EX=LA[i];
      if((EX->SaveID==NULL_ID)&&!Qap::FindParent(EX,Root)){EX->SaveID=IA.size();IA.push_back((int)EX);}
    }
  }
  void ItemKillUndead(QapItem *EX,void* Data)// Этот костыль ещё кто-то использует?
  {
    if(!EX->Deaded)return;
    EX->Deaded=false;
    EX->Dead();
  };
  QapItem* CloneItem(QapItem *Source,QapList *Owner)
  {
    if(Source==Owner)return 0;
    if(!Source)return 0;
    if(Source->IsList()){return CloneList((QapList*)Source,Owner);}
    IntArray IA,OA; vector<void*> LA;
    ItemGrabLinks(Source,&LA);
    for(int i=0;i<LA.size();i++)((QapItem*)LA[i])->SaveID=NULL_ID;
    for(int i=0;i<LA.size();i++)
    {
      QapItem *EX=(QapItem*)LA[i];
      if(EX->SaveID!=NULL_ID){
        EX->Tag++;
        EX->Tag--;
      };
      if(EX->SaveID==NULL_ID){EX->SaveID=IA.size();IA.push_back((int)EX);};
    }
    OA=IA;LA.clear();
    {
      CrutchIO f; QapIO Q; Q.data=&f;
      QapItem *Res=(QapItem*)Source->Factory->Create(Owner);
      Res->SaveID=OA.size();OA.push_back((int)Res);
      Source->SaveToStream(Q);
      f.pos=0;
      Res->LoadFromStream(Q);
      Res->SaveID=IA.size();IA.push_back((int)Res);
      Res->RestoreLink(IA);
      AddCaptionPostfix(Res,"Clon");
      return Res;
    }
  }
  QapList* CloneList(QapList *Source,QapList *Owner)
  {
    if(!Source)return 0;
    Source->Clean();
    IntArray IA,OA; vector<void*> LA;
    Recursive(Source,ItemGrabLinks,&LA);
    for(int i=0;i<LA.size();i++)((QapItem*)LA[i])->SaveID=NULL_ID;
    for(int i=0;i<LA.size();i++)
    {
      QapItem *EX=(QapItem*)LA[i];
      if(EX->SaveID!=NULL_ID){
        EX->Tag++;
        EX->Tag--;
      };
      if(EX->SaveID==NULL_ID)if(!FindParent(EX,Source)){EX->SaveID=IA.size();IA.push_back((int)EX);};
    }
    OA=IA;LA.clear();
    {
      CrutchIO f; QapIO Q; Q.data=&f;
      QapList *Res=(QapList*)Source->Factory->Create(Owner);
      Source->GrabID(OA);
      Source->SaveToStream(Q);
      f.pos=0;
      Res->LoadFromStream(Q);
      Res->GrabID(IA);
      Res->RestoreLink(IA);
      AddCaptionPostfix(Res,"Clon");
      return Res;
    }  
  }
  void ReCalcLockCount(QapList *Root)
  {
    MACRO_ADD_LOG("deprecated",lml_WARNING);
  }
  void MoveItem(QapItem *A,QapItem *B)
  {
    if((A==B)||!A||!B||!A->Parent||!B->Parent)return;
    if(A->IsList())if(FindParent(B,(QapList*)A))return;
    QapList *P=A->Parent;A->Parent=B->Parent;
    for(int i=P->Count()-1;i>=0;i--)
      {if(A!=P->Items[i])continue;{for(int k=i+1;k<P->Items.size();k++)P->Items[k-1]=P->Items[k];}P->Items.pop_back();break;}
    P=A->Parent;QapItem *oA=A,*oB;int j=-1;
    for(int i=0;i<P->Count();i++){if(P->Items[i]==B){j=i;break;}}
    if(j<0)return;P->Items.push_back(0);
    for(int i=j;i<P->Count();i++){oB=P->Items[i];P->Items[i]=oA;oA=oB;};
  }  
  void MoveToList(QapItem *A,QapList *B)
  {
    if(A==B)return;if(!A||!B)return;if(!A->Parent||!B->Parent)return;
    if(A->IsList())if(FindParent(B,(QapList*)A))return;
    QapList *P=A->Parent;A->Parent=B;
    for(int i=P->Count()-1;i>=0;i--)
      {if(A!=P->Items[i])continue;{for(int k=i+1;k<P->Items.size();k++)P->Items[k-1]=P->Items[k];}P->Items.pop_back();break;}
    P=A->Parent;QapItem *oA=A,*oB;P->Items.push_back(0);
    for(int i=0;i<P->Count();i++){oB=P->Items[i];P->Items[i]=oA;oA=oB;};
  }
  string QToS(QapPtr<QapItem> &Link){return Link?Link->Caption+"{"+Link->GetClassName()+"}":"NULL";}
  string QapPtrToStr(QapPtr<QapItem> &Link){return Link?"/*"+Link->Caption+"*/"+IToH(Link.GetSID()):"NULL_ID";}
  void QapPtrSaveToStr(string &str,void *p){str+=QToS(*(Qap::QapPtr<QapItem>*)p);}
  void QapPtrGetCode(string &str,void *p){str+=QapPtrToStr(*(Qap::QapPtr<QapItem>*)p);}
  /************************************************************************/
  /* QapRTTI                                                              */
  /************************************************************************/
  void DoSave(QapItem *EX,QapIO &F)
  {
    QapFactory *P=EX->Factory;
    vector<QapType*> &TL=P->Info.TypeList;
    vector<int> &OL=P->Info.OffsetList;
    for(int i=0;i<TL.size();i++)
    {
      if(TL[i]->Type!=QapType::FT_Unknown)
      {
        int p=OL[i]+int(EX);
        TL[i]->Save(F,(void*)p);
      }
    }
  }
  void DoLoad(QapItem *EX,QapIO &F)
  {
    QapFactory *P=EX->Factory;
    vector<QapType*> &TL=P->Info.TypeList;
    vector<int> &OL=P->Info.OffsetList;
    for(int i=0;i<TL.size();i++)
    {
      QapType*T=TL[i];
      if(T->Type!=QapType::FT_Unknown)
      {
        int p=OL[i]+int(EX);
        T->Load(F,(void*)p);
      }
    }
  }
  /************************************************************************/
  /* QapFactory                                                           */
  /************************************************************************/
  void QapFactory::GrabLinks(QapItem *Item,PtrArray &PA)
  {
    for(int i=0;i<Info.TypeList.size();i++)
    {
      void *p=(void*)(int(Item)+Info.OffsetList[i]);
      QapType *A=Info.TypeList[i];
      if(A->GrabFunc)
        A->GrabFunc(PA,p,A);
    }
  };
  /************************************************************************/
  /* QapItem                                                              */
  /************************************************************************/
  void QapItem::Create(QapList* AParent)
  {
    Parent=AParent; DoReset();
    if(!Parent)return;
    Caption=GetClassName()+"_"+IToS(Parent->AddCount);
    Parent->Add(this);
  }
  void QapItem::DoDraw(){/*Render Code*/}
  void QapItem::DoMove(){/*Update Code*/}
  void QapItem::Draw()
  {
    if(Visible)DoDraw();
  }
  void QapItem::Move()
  {
    if(Moved)DoMove();
  }
  void QapItem::Dead()
  {
    if((Deaded)||(!Parent))return;
    Parent->DeadList.push_back(this); Deaded=true;
  }
  void QapItem::Clear()
  {
    DoClear();
  }
  bool QapItem::SaveToFile(string FN)
  {
    CrutchIO f; 
    QapIO Q; Q.data=&f; SaveToStream(Q);
    f.SaveFile(FN);
    return true;
    //fstream f(FN.c_str(),std::ios_base::out);
    //if(!f)return false;
    //QapIO Q; Q.data=&f; SaveToStream(Q);
    //f.close();
    //return true;
  }
  void QapItem::SaveToStream(QapIO &F)
  {
    //DoSave(F);return;
    Qap::DoSave(this,F);
  }
  bool QapItem::LoadFromFile(string FN)
  {
    CrutchIO f;
    f.LoadFile(FN);
    if(f.mem.empty())return false;
    QapIO Q; Q.data=&f; LoadFromStream(Q);
    return true;
    //fstream f(FN.c_str(),std::ios_base::in);
    //if(!f)return false;
    //QapIO Q; Q.data=&f; LoadFromStream(Q);
    //f.close();
    //return true;
  }
  void QapItem::LoadFromStream(QapIO &F)
  {
    //DoLoad(F);return;
    DoLoad(this,F);
  }
  void QapItem::RestoreLink(vector<int> &A)
  {
    PtrArray PA;
    Factory->GrabLinks(this,PA);
    for(int i=0;i<PA.size();i++)
    {
      QapPtr<QapItem> *p=(QapPtr<QapItem>*)PA[i];
      p->ReConnect(A);
      //if(p->Connected())p->ptr->Caption+="["+this->Caption+"]";
    }
  };
  void QapItem::DoClear()
  {
    PtrArray PA;
    Factory->GrabLinks(this,PA);
    for(int i=0;i<PA.size();i++)
    {
      QapPtr<QapItem> *p=(QapPtr<QapItem>*)PA[i];
      *p=NULL;
    }
    //Вообще-то неплохо было бы тут ещё и векторы почистить... да.
  }
  /************************************************************************/
  /* QapList                                                              */
  /************************************************************************/
  void QapList::Add(QapItem* Item)
  {
    QapPtr<QapItem> QP; QP=Item; QP.ForceSet(0);
    Items.push_back(Item); AddCount++;
  }
  void QapList::Draw()
  {
    if(!Visible)return; DoDraw();
    for(int i=0;i<Count();i++)Items[i]->Draw();
  }
  void QapList::Move()
  {
    if(!Moved)return; DoMove();
    for(int i=0;i<Count();i++)Items[i]->Move();
  }
  void QapList::Clean()
  {
    if(DeadList.empty())return;
    QapPtr<QapItem> QP;
    for(int i=DeadList.size()-1;i>=0;i--)
    {
      QapItem* Culprit=DeadList[i];
      for(int j=Count()-1;j>=0;j--)
      {
        if(Culprit!=Items[j])continue;
        {for(int k=j+1;k<Items.size();k++)Items[k-1]=Items[k];}Items.pop_back();break;
        //Items[j]=Items[Items.size()-1];Items.pop_back(); break;
      }
      Culprit->Clear();
      QP.ForceSet(Culprit); QP=0; DeadList.pop_back();
    };
  }
  void QapList::DoClear()
  {
    ParentClass::DoClear();
    AddCount=0;
  };
  void QapList::Clear()
  {
    DoClear();
    for(uint i=0;i<Count();i++)Items[i]->Dead();
    Clean();
  }
  void QapList::SaveToStream(QapIO &F)
  {
    QapItem::SaveToStream(F);
    int ID,c=Count();QapVIP::Var_int.Save(F,&c);
    for(int i=0;i<c;i++)
    { 
      ID=Items[i]->GetClassID();QapVIP::Var_int.Save(F,&ID);
      QapIO tmp;CrutchIO IO;tmp.data=&IO;//IO.mem.reserve(1024*4);
      Items[i]->SaveToStream(tmp);
      QapVIP::Var_string.Save(F,&IO.mem);
    }
  }
  void QapList::LoadFromStream(QapIO &F)
  {
    Clear();
    QapItem::LoadFromStream(F);
    uint ID,c; QapVIP::Var_int.Load(F,&c);
    for(uint i=0;i<c;i++)
    {
      QapVIP::Var_int.Load(F,&ID);QapItem *EX=CreateItemFromID(this,ID);
      QapIO tmp;CrutchIO IO;tmp.data=&IO;
      QapVIP::Var_string.Load(F,&IO.mem);
      if(EX)EX->LoadFromStream(tmp);
      if(!EX){
        EX=UberInit<QapItem>(this);
        EX->Dead();
      };
    }
    AddCount-=c;//однако костыль...
  }
  void QapList::GrabID(IntArray &A)
  {
    this->SaveID=A.size();A.push_back((int)this);
    for(uint i=0;i<Count();i++)
    {
      QapItem *it=Items[i];
      if(it->IsList()){((QapList*)it)->GrabID(A);}else{it->SaveID=A.size();A.push_back((int)it);}
    }
  }
  void QapList::RestoreLink(IntArray &A)
  {
    ParentClass::RestoreLink(A);
    for(int i=0;i<Count();i++)Items[i]->RestoreLink(A);
  }
  void QapList::Visit(QapVisitor*Visitor)
  {
    Visitor->OnHover(this);
    for(int i=0;i<Count();i++){
      if(Visitor->BreakCount){
        break;
      }
      Items[i]->Visit(Visitor);
      i+=Visitor->ContinueCount?Visitor->ContinueCount--:0;
    }
    Visitor->OnLeave(this);
    Visitor->ContinueCount=0;
    if(Visitor->BreakCount)Visitor->BreakCount--;
  }
}
/************************************************************************/
/* Конец                                                                */
/************************************************************************/