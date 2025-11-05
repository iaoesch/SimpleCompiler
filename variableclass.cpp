#include "variableclass.h"
#include "Errclass.hpp"




void VariableClass::SetContext(VariableContextClass *Context)
{
    if (MyContext != nullptr) {
        throw ERROR_OBJECT("Changing context of variable is not allowed");
    }
    MyContext = Context;
}


double DoubleVariableClass::GetValue() const
{
    return Value;
}

void DoubleVariableClass::SetValue(double v)
{
    Value = v;
}

DoubleVariableClass::DoubleVariableClass(const std::string &Name_, double Value) : VariableClass(Name_),
    Value(Value)
{

}
