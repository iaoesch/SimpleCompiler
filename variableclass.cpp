#include "variableclass.h"
#include "Errclass.hpp"
#include "compact.h"




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

void FunctionDefinitionClass::Print(std::ostream &s) const
{
    {
        bool first = true;
        for(auto &r: Parameters) {
            if (first) {
                first = false;
            } else {
                std::cout << ", ";
            }
            std::cout << r->GetName();
        }
        s << ")" << std::endl;
        for(auto &r: Statements) {
            r->Print(s);
        }
    }

}

TypeDescriptorClass const &VariableContentClass::getType() const
{
    return Type;
}

void VariableContentClass::setType(const TypeDescriptorClass &newType)
{
    Type = newType;
}

}
