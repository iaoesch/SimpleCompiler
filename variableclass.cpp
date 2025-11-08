#include "variableclass.h"
#include "Errclass.hpp"




void VariableClass::SetContext(VariableContextClass *Context)
{
    if (MyContext != nullptr) {
        throw ERROR_OBJECT("Changing context of variable is not allowed");
    }
    MyContext = Context;
}

const TypeDescriptorClass &VariableClass::GetType() const
{
    return Content.getType();
}


Variables::VariableContentClass DoubleVariableClass::GetValue() const
{
    Variables::VariableContentClass C = TypeDescriptorClass(TypeDescriptorClass::Type::Float);
    return C;
}

void DoubleVariableClass::SetValue(Variables::VariableContentClass v)
{
    Value = v.GetValue<double>();
}

DoubleVariableClass::DoubleVariableClass(const std::string &Name_, double Value) : VariableClass(Name_, TypeDescriptorClass(TypeDescriptorClass::Type::Float)),
    Value(Value)
{

}


namespace Variables {
FunctionDefinitionClass::FunctionDefinitionClass(const std::list<std::shared_ptr<VariableClass> > &Parameters, const std::list<std::shared_ptr<StatementClass> > &Statements) : Parameters(Parameters),
    Statements(Statements)
{}

TypeDescriptorClass const &VariableContentClass::getType() const
{
    return Type;
}

void VariableContentClass::setType(const TypeDescriptorClass &newType)
{
    Type = newType;
}

}
