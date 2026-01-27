#ifndef VARIABLECLASS_H
#define VARIABLECLASS_H


#include <string>
#include <iostream>
#include "typedescriptorclass.hpp"
#include "variablecontentclass.h"
//#include "varmanag.hpp"


class VariableClass
{
public:
    enum class StorageClass {None = 0, RW = 1, RO = 2, Code = 8, Local = 16, Global = 32, Static = 64, Class = 128};

    static void SetDefaultEnvironment(Environment &Env) {DefaultEnvironment = &Env;}

private:
    VariableContextManageClass *MyContext;
    const std::string Name;
    VariableTypeDescriptorClass MyType;
    const StorageClass Storage;
protected:
    bool  Initialized;
    static Environment *DefaultEnvironment;

protected:
    bool IsWriteable();

public:
    VariableClass(const std::string &Name_, VariableTypeDescriptorClass MyType_, StorageClass Storage_) : MyContext(nullptr), Name(Name_), MyType(MyType_), Storage(Storage_), Initialized(false) {}
    virtual ~VariableClass() {}
    void SetContext(VariableContextManageClass *Context);
    const std::string &GetName()  const {return Name;}
    virtual Variables::VariableContentClass const &GetValue() const = 0;
    virtual Variables::VariableContentClass const &GetInitialValue() const = 0;
    virtual Variables::VariableContentClass &GetWriteReferenceToValue() = 0;
    virtual void        SetValue(Variables::VariableContentClass v) = 0;
    virtual void        SetInitialValue(Variables::VariableContentClass v) = 0;
    virtual void        Print(std::ostream &s) = 0;
    virtual void        DrawNode(std::ostream &s, int MyNodeNumber) const;

    StorageClass GetStorageClass() const {return Storage;}

    bool IsAssignable(Variables::VariableContentClass const &Content)
    {
        return( (   (MyType == TypeDescriptorClass::Type::Dynamic)
                  || (MyType == Content.getType()))
                && IsWriteable()) ;

    }

    void PrepareForAssignment(Variables::VariableContentClass const &Content)
    {
        if(MyType == TypeDescriptorClass::Type::Undefined) {
           MyType = Content.getType() ;
        }
    }

    const VariableTypeDescriptorClass &Type() const {return MyType;}
    const ValueTypeDescriptorClass &ContaindedType() const {return GetContainedType();}
private:
    virtual const ValueTypeDescriptorClass &GetContainedType() const = 0;
};

inline VariableClass::StorageClass operator & (VariableClass::StorageClass e1, VariableClass::StorageClass e2)
{
    return VariableClass::StorageClass(int(e1) & int(e2));
}

inline VariableClass::StorageClass operator | (VariableClass::StorageClass e1, VariableClass::StorageClass e2)
{
    return VariableClass::StorageClass(int(e1) | int(e2));
}

inline bool IsStorageType (VariableClass::StorageClass e1, VariableClass::StorageClass e2)
{
    return (e1 & e2) == e2;
}

inline bool VariableClass::IsWriteable()
{
    return (Storage & StorageClass::RW) == StorageClass::RW;
}



class GlobalVariableClass : public VariableClass
{
    Variables::VariableContentClass Content;

public:
    GlobalVariableClass(const std::string &Name_, const VariableTypeDescriptorClass &Type_, StorageClass Storage_) : VariableClass(Name_, Type_, Storage_), Content(Variables::VariableContentClass::MakeEmpty(Type_)) {}
    virtual ~GlobalVariableClass() override {}
    virtual Variables::VariableContentClass const &GetValue() const override;
    virtual Variables::VariableContentClass const &GetInitialValue() const override;
    virtual Variables::VariableContentClass &GetWriteReferenceToValue() override;
    virtual void        SetValue(Variables::VariableContentClass v) override;
    virtual void        SetInitialValue(Variables::VariableContentClass v) override;
    virtual void        Print(std::ostream &s) override;
  //  virtual void        DrawNode(std::ostream &s, int MyNodeNumber) const override;


private:
    virtual const ValueTypeDescriptorClass &GetContainedType() const override;
};


class LocalVariableClass : public VariableClass
{
    uint32_t Reference;
    std::shared_ptr<Variables::FunctionDefinitionBaseClass> Parent;

public:
    LocalVariableClass(const std::string &Name_, const VariableTypeDescriptorClass &Type_, uint32_t Reference_, std::shared_ptr<Variables::FunctionDefinitionBaseClass> Parent_, StorageClass Storage_) : VariableClass(Name_, Type_, Storage_), Reference(Reference_), Parent(Parent_) {}
    virtual ~LocalVariableClass() override {}
    virtual Variables::VariableContentClass const &GetValue() const override;
    virtual Variables::VariableContentClass const &GetInitialValue() const override;
    virtual Variables::VariableContentClass &GetWriteReferenceToValue() override;
    virtual void        SetValue(Variables::VariableContentClass v) override;
    virtual void        SetInitialValue(Variables::VariableContentClass v) override;
    virtual void        Print(std::ostream &s) override;
  //  virtual void        DrawNode(std::ostream &s, int MyNodeNumber) const override;


private:
    virtual const ValueTypeDescriptorClass &GetContainedType() const override;
};

class LateBindingVariableClass : public VariableClass
{
    uint32_t Reference;
    static const uint32_t ThisOffset = 0;
    std::string ReferenceName;
    std::shared_ptr<Variables::FunctionDefinitionBaseClass> BoundMethod;

public:
    LateBindingVariableClass(const std::string &Name_, const VariableTypeDescriptorClass &Type_, uint32_t Reference, StorageClass Storage_) : VariableClass(Name_, Type_, Storage_), ReferenceName(Name_) {}
    virtual ~LateBindingVariableClass() override {}
    virtual Variables::VariableContentClass const &GetValue() const override;
    virtual Variables::VariableContentClass const &GetInitialValue() const override;
    virtual Variables::VariableContentClass &GetWriteReferenceToValue() override;
    virtual void        SetValue(Variables::VariableContentClass v) override;
    virtual void        SetInitialValue(Variables::VariableContentClass v) override;
    virtual void        Print(std::ostream &s) override;
    //  virtual void        DrawNode(std::ostream &s, int MyNodeNumber) const override;

    bool operator == (LateBindingVariableClass const& other) const {
        return Type() == other.Type();
    }

    bool BindToMethod(std::shared_ptr<Variables::FunctionDefinitionBaseClass> BoundMethod_);

private:
    virtual const ValueTypeDescriptorClass &GetContainedType() const override;
};

class StaticClassMemberVariableClass : public VariableClass
{
    uint32_t Reference;
    std::string ReferenceName;
    std::shared_ptr<Variables::ClassClass> MyClass;

public:
    StaticClassMemberVariableClass(const std::string &Name_, const VariableTypeDescriptorClass &Type_, StorageClass Storage_) : VariableClass(Name_, Type_, Storage_), ReferenceName(Name_) {}
    virtual ~StaticClassMemberVariableClass() override {}
    virtual Variables::VariableContentClass const &GetValue() const override;
    virtual Variables::VariableContentClass const &GetInitialValue() const override;
    virtual Variables::VariableContentClass &GetWriteReferenceToValue() override;
    virtual void        SetValue(Variables::VariableContentClass v) override;
    virtual void        SetInitialValue(Variables::VariableContentClass v) override;
    virtual void        Print(std::ostream &s) override;
    //  virtual void        DrawNode(std::ostream &s, int MyNodeNumber) const override;

    bool BindToMethod(std::shared_ptr<Variables::FunctionDefinitionBaseClass> BoundMethod_);

private:
    virtual const ValueTypeDescriptorClass &GetContainedType() const override;
};

class TemporaryVariableClass : public VariableClass
{
    Variables::VariableContentClass Content;

public:
    TemporaryVariableClass(const std::string &Name_, const VariableTypeDescriptorClass &Type_) : VariableClass(Name_, Type_, StorageClass::RW|StorageClass::Local), Content(Variables::VariableContentClass::MakeEmpty(Type_)) {}
    virtual ~TemporaryVariableClass() override {}
    virtual Variables::VariableContentClass const &GetValue() const override;
    virtual Variables::VariableContentClass const &GetInitialValue() const override;
    virtual Variables::VariableContentClass &GetWriteReferenceToValue() override;
    virtual void        SetValue(Variables::VariableContentClass v) override;
    virtual void        SetInitialValue(Variables::VariableContentClass v) override;
    virtual void        Print(std::ostream &s) override;
    //  virtual void        DrawNode(std::ostream &s, int MyNodeNumber) const override;


private:
    virtual const ValueTypeDescriptorClass &GetContainedType() const override;
};


class ProxyVariableClass : public VariableClass
{
    Variables::VariableContentClass &Content;

public:
    ProxyVariableClass(const std::string &Name_, const VariableTypeDescriptorClass &Type_, Variables::VariableContentClass &ReferedContent, StorageClass Storage_) : VariableClass(Name_, Type_, Storage_), Content(ReferedContent) {}
    virtual ~ProxyVariableClass() override {}
    virtual const Variables::VariableContentClass &GetValue() const override;
    virtual Variables::VariableContentClass const &GetInitialValue() const override;
    virtual Variables::VariableContentClass &GetWriteReferenceToValue() override;
    virtual void        SetValue(Variables::VariableContentClass v) override;
    virtual void        SetInitialValue(Variables::VariableContentClass v) override;
    virtual void        Print(std::ostream &s) override;
    //  virtual void        DrawNode(std::ostream &s, int MyNodeNumber) const override;


private:
    virtual const ValueTypeDescriptorClass &GetContainedType() const override;
};

#if 0
class DoubleVariableClass : public VariableClass {

    double Value;

    // VariableClass interface
public:
    DoubleVariableClass(const std::string &Name_, double Value);
    virtual Variables::VariableContentClass GetValue() const override;
    virtual void SetValue(Variables::VariableContentClass v) override;
};
#endif
#endif // VARIABLECLASS_H
