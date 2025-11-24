#ifndef VARIABLECLASS_H
#define VARIABLECLASS_H


#include <string>
#include <iostream>
#include "typedescriptorclass.hpp"
#include "variablecontentclass.h"
//#include "varmanag.hpp"


class VariableClass
{
    VariableContextClass *MyContext;
    const std::string Name;
    VariableTypeDescriptorClass MyType;

public:
    VariableClass(const std::string &Name_, VariableTypeDescriptorClass MyType_) : MyContext(nullptr), Name(Name_), MyType(MyType_) {}
    virtual ~VariableClass() {}
    void SetContext(VariableContextClass *Context);
    const std::string GetName()  const {return Name;}
    virtual Variables::VariableContentClass      GetValue() const = 0;
    virtual void        SetValue(Variables::VariableContentClass v) = 0;
    virtual void        Print(std::ostream &s) = 0;
    virtual void        DrawNode(std::ostream &s, int MyNodeNumber) const;

    bool IsAssignable(Variables::VariableContentClass const &Content)
    {
        return(    (MyType == TypeDescriptorClass::Type::Dynamic)
                || (MyType == Content.getType())) ;

    }

    const VariableTypeDescriptorClass &Type() const {return MyType;}
    const ValueTypeDescriptorClass &ContaindedType() const {return GetContainedType();}
private:
    virtual const ValueTypeDescriptorClass &GetContainedType() const = 0;
};

class GlobalVariableClass : public VariableClass
{
    Variables::VariableContentClass Content;

public:
    GlobalVariableClass(const std::string &Name_, const VariableTypeDescriptorClass &Type_) : VariableClass(Name_, Type_), Content(Type_) {}
    virtual ~GlobalVariableClass() override {}
    virtual Variables::VariableContentClass GetValue() const override;
    virtual void        SetValue(Variables::VariableContentClass v) override;
    virtual void        Print(std::ostream &s) override;
  //  virtual void        DrawNode(std::ostream &s, int MyNodeNumber) const override;


private:
    virtual const ValueTypeDescriptorClass &GetContainedType() const override;
};

class LocalVariableClass : public VariableClass
{
    uint32_t Reference;
    std::shared_ptr<Variables::FunctionDefinitionClass> Parent;

public:
    LocalVariableClass(const std::string &Name_, const VariableTypeDescriptorClass &Type_, uint32_t Reference_, std::shared_ptr<Variables::FunctionDefinitionClass> Parent_) : VariableClass(Name_, Type_), Reference(Reference_), Parent(Parent_) {}
    virtual ~LocalVariableClass() override {}
    virtual Variables::VariableContentClass GetValue() const override;
    virtual void        SetValue(Variables::VariableContentClass v) override;
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
