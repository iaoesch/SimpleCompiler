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

Variables::VariableContentClass VariableClass::GetValue() const
{

}

void VariableClass::SetValue(Variables::VariableContentClass v)
{
    Content = v;
}

void VariableClass::Print(std::ostream &s)
{
    s << Content;
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
        s << "(";
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

TypeDescriptorClass ArrayClass::GetTypeDescriptor() const
{
   // ArrayDescriptorClass Arraydescriptor;

  // TypeDescriptor Descriptor =
}

}


TypeDescriptorClass CommonType(const TypeDescriptorClass &t1, const TypeDescriptorClass &t2)
{
    using Type = TypeDescriptorClass::Type;

    // List dominates everything
    if (t1.MyType == Type::List) {
        return t1;
    }
    if (t2.MyType == Type::List) {
        return t2;
    }

    if (t1.MyType == t2.MyType) {
        switch(t1.MyType) {

        case Type::Undefined:
        case Type::Integer:
        case Type::Float:
        case Type::Bool:
        case Type::String:
        case Type::List:
        case Type::Map:
        case Type::Function: return t1;
        case Type::Stack:   {
            auto tc = CommonType(*(std::get<StackDescriptorClass>(t1.Descriptor).BaseType), *(std::get<StackDescriptorClass>(t2.Descriptor).BaseType));
            if (tc.MyType == Type::Undefined) {
                return tc;
            } else {
                // Use common basetype
                auto rt = t1;
                rt.MyType = tc.MyType;
                return rt;
            }
        }
            break;

        case Type::Array:   {auto tc = CommonType(*(std::get<ArrayDescriptorClass>(t1.Descriptor).BaseType), *(std::get<ArrayDescriptorClass>(t2.Descriptor).BaseType));
            if (tc.MyType == Type::Undefined) {
                return tc;
            } else if (std::get<ArrayDescriptorClass>(t1.Descriptor).Dimensions == std::get<ArrayDescriptorClass>(t2.Descriptor).Dimensions) {
                // Use common basetype
                auto rt = t1;
                rt.MyType = tc.MyType;
                return rt;
            } else {
                // dimensions differ
                return TypeDescriptorClass(Type::Undefined);
            }
        }
        break;


        case Type::Dynamic:  return CommonType(*(std::get<DynamicDescriptorClass>(t1.Descriptor).CurrentType), *(std::get<DynamicDescriptorClass>(t2.Descriptor).CurrentType));

        }
    }
    if ((t1.MyType == Type::Integer) && (t2.MyType == Type::Float)) {
        return t2;
    }
    if ((t2.MyType == Type::Integer) && (t1.MyType == Type::Float)) {
        return t1;
    }
    return TypeDescriptorClass(Type::Undefined);
}
