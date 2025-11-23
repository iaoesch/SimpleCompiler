#include <iostream>
#include "typedescriptorclass.hpp"

TypeDescriptorClass CommonType(const TypeDescriptorClass &t1, const TypeDescriptorClass &t2)
{
    using Type = ValueTypeDescriptorClass::Type;
    
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
        case Type::Expression:
        case Type::Illegal:
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
                return ValueTypeDescriptorClass(Type::Undefined);
            }
        }
        break;
            
            
        case Type::Dynamic:  return ValueTypeDescriptorClass(Type::Undefined);
            //CommonType(*(std::get<DynamicDescriptorClass>(t1.Descriptor).CurrentType), *(std::get<DynamicDescriptorClass>(t2.Descriptor).CurrentType));
            
        }
    }
    if ((t1.MyType == Type::Integer) && (t2.MyType == Type::Float)) {
        return t2;
    }
    if ((t2.MyType == Type::Integer) && (t1.MyType == Type::Float)) {
        return t1;
    }
    return ValueTypeDescriptorClass(Type::Undefined);
}
#if 0
void ValueTypeDescriptorClass::ChangeDynamicType(const ValueTypeDescriptorClass &NewType)
{
    if (MyType != Type::Dynamic) {
        throw INTERNAL_ERROR_OBJECT("Cannot change type");
    }
    if (NewType.MyType == Type::Dynamic) {
        Descriptor = NewType.Descriptor;
    } else {
        *(std::get<DynamicDescriptorClass>(Descriptor).CurrentType) = NewType;
    }
}

const ValueTypeDescriptorClass &ValueTypeDescriptorClass::GetDynamicType() const
{
    return *(std::get<DynamicDescriptorClass>(Descriptor).CurrentType);
}

#endif

std::ostream &operator << (std::ostream &s, TypeDescriptorClass const&t)
{
    s << "type<";
    switch(t.MyType) {

    case ValueTypeDescriptorClass::Type::Undefined: s << "undef"; break;
    case ValueTypeDescriptorClass::Type::Integer:   s << "Integer"; break;
    case ValueTypeDescriptorClass::Type::Float:     s << "Float"; break;
    case ValueTypeDescriptorClass::Type::Bool:      s << "Bool"; break;
    case ValueTypeDescriptorClass::Type::String:    s << "String"; break;
    case ValueTypeDescriptorClass::Type::Stack:     s << "Stack"; break;
    case ValueTypeDescriptorClass::Type::List:      s << "List"; break;
    case ValueTypeDescriptorClass::Type::Array:     s << "Array"; break;
    case ValueTypeDescriptorClass::Type::Map:       s << "Map"; break;
    case ValueTypeDescriptorClass::Type::Function:  s << "Function"; break;
    case ValueTypeDescriptorClass::Type::Expression:s << "Expression"; break;
    case ValueTypeDescriptorClass::Type::Dynamic:   s << "Dynamic"; break;
    case ValueTypeDescriptorClass::Type::Illegal:   s << "illegal"; break;
        break;
    }
    s << ">";
    return s;
}


ArrayDescriptorClass::ArrayDescriptorClass(
    std::vector<int64_t> Dimensions, std::unique_ptr<VariableTypeDescriptorClass> BaseType)
    : Dimensions(std::move(Dimensions)), BaseType(std::move(BaseType)) {}


