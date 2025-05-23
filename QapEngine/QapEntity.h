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
#include "QapTools.h"
#include "QapIO.h"
#include "QapInput.h"
/************************************************************************/
/* Переменные                                                           */
/************************************************************************/
extern QapSys Sys;
const int NULL_ID=-1;
namespace Qap
{
  class QapObj; class QapItem; class QapList; class QapFactory; class QapRTTI; class QapRender;class QapVisitor;
  template<class TYPE>class Factory;template<class TYPE>class QapPtr;
  typedef QapItem* PQapItem; typedef QapList* PQapList;
  typedef QapItem*(*TCreateItemFromID)(QapList* Owner,uint ID);
  typedef void(*RecFunc)(QapItem *EX,void* Data);
  template<class TYPE>inline TYPE* UberCast(QapItem *EX){return EX->IsDerivedFrom(TYPE::GetStaticClassFactory())?(TYPE*)EX:NULL;};
  //template<class TYPE>inline TYPE* UberCast(QapItem *EX){return dynamic_cast<TYPE*>(EX);}
  template<class TYPE>inline TYPE* UberInit(QapList *Owner){return (TYPE*)TYPE::GetStaticClassFactory()->Create(Owner);};
  template<class TYPE>inline void GrabEntity(QapItem *It,void *data){TYPE *EX=Qap::UberCast<TYPE>(It);if(EX)((PtrArray*)data)->push_back(EX);}
  extern TCreateItemFromID CreateItemFromID;
  extern vector<QapFactory*> FactoryList;
  extern vector<string> ScopeName;
  extern vector<vector<QapFactory*>> ScopeList;
}
/************************************************************************/
/* System procedure                                                     */
/************************************************************************/
namespace Qap
{
  QapItem* DefCreateItemFromID(QapList* Owner,uint ID);
  QapFactory* RegItemClass(QapFactory* RegFac);
  QapType* RegType(QapType const&Type,QapRTTI *RTTI);
  QapItem* CloneItem(QapItem *Source,QapList *Owner);
  QapList* CloneList(QapList *Source,QapList *Owner);
  void IncPostfix(string&Str,string Postfix);
  void MoveItem(QapItem *A,QapItem *B);
  void MoveToList(QapItem *A,QapList *B);
  void ReCalcLockCount(QapList *Root);
  void Recursive(QapList *A,RecFunc Func,void* Data);
  void ItemLockCount(QapItem *EX,void* Data);
  void ItemGrabLinks(QapItem *EX,void* Data);
  void GrabExternalLinks(QapList *Root,IntArray &IA);
  bool FindParent(QapItem *Item,QapList *Owner);
  bool GetFieldPtrByName(QapFactory *Factory,string &FieldName,void *&RetPtr);
  void QapPtrSaveToStr(string &str,void *p);
  void QapPtrGetCode(string &str,void *p);
  void DoSave(QapItem *EX,QapIO &F);
  void DoLoad(QapItem *EX,QapIO &F);
  string QToS(QapPtr<QapItem> &Link);
  template<typename TYPE>
  static void DefSavePtr(QapIO &IO,void *value)
  {
    QapPtr<TYPE> &tmp=*(QapPtr<TYPE>*)value;
    int ID=tmp.GetSID();
    IO.SavePlainData(&ID,sizeof(ID));
  }
  template<typename TYPE>
  static void DefLoadPtr(QapIO &IO,void *value)
  {
    QapPtr<TYPE> &tmp=*(QapPtr<TYPE>*)value;
    int ID;IO.LoadPlainData(&ID,sizeof(ID));
    tmp.SetSID(ID);
  }
  template<typename TYPE>
  QapType GetLinkType(string &Name)
  {
    return QapType(QapType::FT_SmartPointer,Name,DefSavePtr<TYPE>,DefLoadPtr<TYPE>,QapPtrSaveToStr,NULL,QapVIP::GrabPointer,QapPtrGetCode);
  }
}
/************************************************************************/
/* Пространство движка                                                  */
/************************************************************************/
namespace Qap
{
  class QapObject
  {   
  public:
    int RefCount;
    bool Deaded;
  public:
    QapObject():RefCount(0),Deaded(false){}
    ~QapObject(){};
    inline void AddRef(){RefCount++;}
    inline void Release(){RefCount--;}
    /*
    //inline 
      void AddRef()
    {
      static int FullAddRefCount=0;
      FullAddRefCount++;
      RefCount++;
      if(FullAddRefCount%16==0)MACRO_ADD_LOG("Get "+IToS(FullAddRefCount)+"!",lml_EVENT);
    };
    //inline 
      void Release()
    {
      static int FullReleaseCount=0;
      FullReleaseCount++;
      RefCount--;
      if(FullReleaseCount%16==0)MACRO_ADD_LOG("Get "+IToS(FullReleaseCount)+"!",lml_EVENT);
    };*/
    //inline bool IsLost(){return Deaded;}
  };
  /************************************************************************/
  /* Abstract Factory / Factory Method / Virtual Constructor              */
  /************************************************************************/
  class QapRTTI
  {   
  public:
    QapFactory *Factory;//ну и кто её реально использует? =)
    vector<QapType*> TypeList;
    vector<string> NameList;
    vector<int> OffsetList;
  public:
    void AddField(void const*ptr,QapType const&FieldType,string const&FieldName,void const*FieldPtr)
    {
      TypeList.push_back(RegType(FieldType,this));
      NameList.push_back(FieldName);
      OffsetList.push_back(int(FieldPtr)-int(ptr));
    }
  };
  class QapFactory
  {
  public:
    int FactoryID;
    string FactoryName;
    QapFactory *FactoryParent;
    QapRTTI Info;
  public:   
    virtual QapItem* Create(QapList* AParent)=0;
    virtual void Destroy(QapItem *Item)=0;
  public:
    bool IsDerivedFrom(QapFactory *Factory)
    {
      if(Factory==this){return true;}else{
        return FactoryParent?FactoryParent->IsDerivedFrom(Factory):false;
      };
    };
    //virtual string GetFactoryName()=0;
    //virtual uint GetFactoryID()=0;
    //virtual QapFactory* GetParentFactory()=0;
    QapType *GetFieldPtr(QapItem *Item,string &FieldName,void *&RetPtr)
    {
      for(int i=0;i<Info.NameList.size();i++)
        if(Info.NameList[i]==FieldName)
        {
          RetPtr=(void*)(Info.OffsetList[i]+int(Item));
          return Info.TypeList[i];
        }
      return NULL;
    }
    void GrabLinks(QapItem *Item,PtrArray &PA);
  };
  template<class TYPE>
  class Factory:public QapFactory{
  public:
    Factory(string Name)
    {
      {
        FactoryName=Name;
        FactoryParent=TYPE::ParentClass::GetStaticClassFactory();//Parent;
      }
      {
        Info.Factory=this;
        TYPE Entity;
        Entity.Factory=this;
        Entity.GetRTTI(Info);
      }
    }
  public:
    TYPE* Init(QapList *AParent){return (TYPE*)Create(AParent);}
    virtual QapItem* Create(QapList* AParent)
    {
      //MACRO_ADD_LOG("ParentCaption["+SToS(AParent?AParent->Caption:"undefined")+"]",lml_EVENT);
      TYPE *Item=new TYPE;
      Item->Factory=this;
      Item->Create(AParent);
      return Item;
    }
    virtual void Destroy(QapItem *Item)
    {
      //MACRO_ADD_LOG("Caption["+Item->Caption+"]",lml_EVENT);
      delete (TYPE*)Item;
    }
  public:
  };
  /************************************************************************/
  /* Smart Pointer                                                        */
  /************************************************************************/
  template<class TYPE>
  class QapPtr
  {
  public:
    TYPE* ptr;
    QapFactory* Factory;
    /*<4its>*/
    int GetSID(){return Connected()?ptr->SaveID:NULL_ID;};
    void SetSID(int ID){ptr=(TYPE*)ID;};
    void ReConnect(IntArray &A){ptr=(NULL_ID==(int)ptr)?0:(TYPE*)A[(int)ptr];if(ptr)Lock();};
    bool Connected(){if(ptr)if(ptr->Deaded){disconnect();return false;};return ptr;};
    void ForceSet(TYPE* a){ptr=a;};
    /*</4its>*/
  public:
    void Free()
    {
      if(ptr->RefCount)MACRO_ADD_LOG("invalid LockCount",lml_WARNING);
      //MACRO_ADD_LOG("ptr->Caption = \""+ptr->Caption+"\"",lml_EVENT);
      ptr->Clear(); QapItem::Destroy(ptr); ptr=0;
    };
    void Lock()
    {
      ptr->AddRef();
    }
    void UnLock()
    {
      if(!ptr->RefCount){MACRO_ADD_LOG("invalid LockCount",lml_WARNING);}else{ptr->Release();};
      if(!ptr->RefCount)Free();
    };
  protected:
    void connect(TYPE *a)
    {
      if(!a->IsDerivedFrom(Factory))return;ptr=a;a->AddRef();
    };
    void disconnect(){UnLock();ptr=0;};
  public:
    QapPtr():ptr(0){Factory=TYPE::GetStaticClassFactory();};
    QapPtr(TYPE const*a):ptr(0){Factory=TYPE::GetStaticClassFactory();this->operator =(a);};
    QapPtr(QapPtr const&a):ptr(0){Factory=TYPE::GetStaticClassFactory();this->operator =(a.ptr);}
    ~QapPtr(){if(Connected())disconnect();};
    TYPE* operator ->()const{return ptr;};
    TYPE*Get(){return Connected()?ptr:NULL;}
    QapPtr& operator =(TYPE const*a)
    {
      if(a==ptr)return *this;if(!a){disconnect();return *this;};if(ptr)disconnect();connect(const_cast<TYPE*>(a));return *this;
    };
    QapPtr& operator =(QapPtr const&a){return this->operator =(a.ptr);};
    bool operator !(){return !Connected();};
    operator bool (){return Connected();};
  public:
  };
  /************************************************************************/
  /* Базовый элемент                                                      */
  /************************************************************************/
  //#if(1)
  #ifdef USE_MACRO_BUILD
    #include "..\Macro\QapEntity.h"
  #else
  //=====>>>>>QapEntity
  class QapItem:public QapObject{
  public:
    //QapItem():QapObject(){};
    //QapItem(QapFactory *AFactory):QapObject(){Factory=AFactory;} - так и ниасилил :(
    QapFactory*Factory;
    QapList*Parent;
  public:
    #define DEF_PRO_CLASS_INFO(NAME,OWNER)NAME(QapItem)//OWNER(QapObj)
    #define DEF_PRO_METHOD(F)F(DoReset);
    #define DEF_PRO_VARIABLE()\
    ADDVAR(string,Caption,GetClassName()+"_Boss");\
    ADDVAR(bool,Visible,true);\
    ADDVAR(bool,Moved,true);\
    ADDVAR(int,SaveID,0);\
    ADDVAR(int,Tag,0);
    /*(<MacroCode>)*/
    #include "QapProClass.inl"//Generation ProClass Code
    /*(</MacroCode>)*/
  public:
    virtual void DoDraw();
    virtual void DoMove();
  public:
    virtual void Draw();
    virtual void Move();
    virtual void Dead();
    virtual void Clear();
    //virtual void Clone(QapItem *Source);
  public:
    //virtual void Render(QapRender*QR){ChildRender(QR);}
    //virtual void ChildRender(QapRender*QR){};
  public:
    virtual bool SaveToFile(string FN);
    virtual void SaveToStream(QapIO&F);
    virtual bool LoadFromFile(string FN);
    virtual void LoadFromStream(QapIO&F);
  public:
    virtual void RestoreLink(vector<int> &A);
    virtual void DoClear();
  public:
    virtual void Create(QapList*AParent);
    static void Destroy(QapItem*Self){Self->Factory->Destroy(Self);};
    virtual bool IsList(){return false;};
    virtual void GetSpecInfo(StringArray&SA){SA.push_back("RefCount="+IToS(RefCount));}
  public:
    string GetClassName(){return Factory->FactoryName;}
    uint GetClassID(){return Factory->FactoryID;}
    bool IsDerivedFrom(Qap::QapFactory*Factory){return this->Factory->IsDerivedFrom(Factory);}
    QapType*GetFieldPtr(string &FieldName,void*&RetPtr){return Factory->GetFieldPtr(this,FieldName,RetPtr);}
  public:
    //Смачный костыль =)
    class ParentClass{public:static QapFactory*GetStaticClassFactory(){return NULL;};};
  private:
    template<typename TYPE>
    inline TYPE weak_visit(QapVisitor*Visitor){Visitor->OnAccept(this);};
  public:
    virtual void Visit(QapVisitor*Visitor){weak_visit<void>(Visitor);};
  public:
    bool GetTweakInfo(StringArray&SA,PtrArray&PA){GetTweakList(SA,PA);return !SA.empty();}
    virtual void GetTweakList(StringArray&SA,PtrArray&PA){}
  };
  /************************************************************************/
  /* Базовая коллекция / Composite Object                                 */
  /************************************************************************/
  class QapList:public QapItem{    
  public:
    vector<QapItem*> Items;
    vector<QapItem*> DeadList;
  private:
    virtual void Add(QapItem* Item);
    friend QapItem;
  public:
    #define DEF_PRO_MANUAL_LINK
    #define DEF_PRO_CLASS_INFO(NAME,OWNER)NAME(QapList)OWNER(QapItem)
    #define DEF_PRO_VARIABLE()\
    ADDVAR(int,AddCount,0);\
    ADDVAR(int,ListBits,0);
    /*(<MacroCode>)*/
    #include "QapProClass.inl"//Generation ProClass Code
    /*(</MacroCode>)*/
  public:
    virtual void Draw();
    virtual void Move();
    virtual void Clear();
    virtual void Clean();
  public:
    //virtual void ChildRender(QapRender *QR){for(int i=0;i<Count();i++)if(Items[i]->Visible)Items[i]->Render(QR);}
  public:
    virtual void SaveToStream(QapIO &F);
    virtual void LoadFromStream(QapIO &F);
  public:
    virtual void GrabID(IntArray &A);
    virtual void RestoreLink(IntArray &A);
    virtual void DoClear();
  public:
    //INFO
    virtual bool IsList(){return true;};
    virtual void GetSpecInfo(StringArray &SA){
      ParentClass::GetSpecInfo(SA);
      SA.push_back("Count="+IToS(Count()));
    };
    int Count(){return Items.size();};
  public:
    virtual void Visit(QapVisitor*Visitor);
  };

  class QapVisitor:public QapItem{
  public:
    #define DEF_PRO_CLASS_INFO(NAME,OWNER)NAME(QapVisitor)OWNER(QapItem)
    #define DEF_PRO_METHOD(F)F(Break);F(Continue);
    #define DEF_PRO_VARIABLE()\
    ADDVAR(int,BreakCount,0);\
    ADDVAR(int,ContinueCount,0);
    /*(<MacroCode>)*/
    #include "QapProClass.inl"//Generation ProClass Code
    /*(</MacroCode>)*/
  public:
    void Break(){BreakCount++;}
    void Continue(){ContinueCount++;}
    virtual void Launch(QapItem*EX){BreakCount=0;ContinueCount=0;EX->Visit(this);};
    virtual void OnHover(QapList*EX){};
    virtual void OnAccept(QapItem*EX){};
    virtual void OnLeave(QapList*EX){};
  };

  //<<<<<=====QapEntity
  #endif
};
/************************************************************************/
/* сохранялка для vector<QapPtr<QapItem>>                               */
/************************************************************************/
namespace QapVIP
{
  typedef Qap::QapPtr<Qap::QapItem> TQapPtr;
  const QapType Var_TQapPtr=Qap::GetLinkType<Qap::QapItem>(SToS("TQapPtr"));
}
namespace QapVIP
{
  typedef vector<Qap::QapPtr<Qap::QapItem>> TQapPtrArr;
  const QapType Var_TQapPtrArr=GetVectorType<TQapPtr>(SToS("TQapPtr"),Var_TQapPtr);
  //typedef vector<vector<Qap::QapPtr<Qap::QapItem>>> TQapPtrVec;
  //const QapType Var_TQapPtrVec=GetVectorType<TQapPtrArr>(SToS("TQapPtrArr"),Var_TQapPtrArr);
}
/************************************************************************/
/* Конец                                                                */
/************************************************************************/