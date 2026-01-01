#include <iostream>
#include <sstream>
#include "typedescriptorclass.hpp"

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

    // Handle case of just one referencetype
    if (t1.MyType != t2.MyType) {

        // helper lambda to avoid codeduplication, just used below...
        auto CommonTypeFirstDereferenced = [](const TypeDescriptorClass &t1, const TypeDescriptorClass &t2) -> TypeDescriptorClass
        {
            const TypeDescriptorClass &rt = *std::get<ReferenceDescriptorClass>(t1.Descriptor).ReferedType;

            // allow just one level of dereferencing
            if (rt.MyType == Type::Reference) {
                return TypeDescriptorClass(Type::Undefined);
            } else {
                // examine dereferenced first type an second type
                return CommonType(rt, t2);
            }
        };

        if (t1.MyType == Type::Reference) {
            return CommonTypeFirstDereferenced(t1, t2);
        }

        if (t2.MyType == Type::Reference) {
            return CommonTypeFirstDereferenced(t2, t1);
        }
        // here we have no referenced types
    }
    // here we have either no referenced types or both types references

    

    if (t1.MyType == t2.MyType) {
        switch(t1.MyType) {
            
        case Type::Undefined:
        case Type::Integer:
        case Type::Float:
        case Type::Bool:
        case Type::String:
        case Type::Type:
        case Type::List:
        case Type::Map:
        case Type::Expression:
        case Type::Internal:
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
        case Type::Reference:   {
            // Follow chain of references,
            TypeDescriptorClass *tt1 = &*std::get<ReferenceDescriptorClass>(t1.Descriptor).ReferedType;
            TypeDescriptorClass *tt2 = &*std::get<ReferenceDescriptorClass>(t2.Descriptor).ReferedType;
            while ((std::get<ReferenceDescriptorClass>(tt1->Descriptor).ReferedType->MyType == Type::Reference) && (std::get<ReferenceDescriptorClass>(tt2->Descriptor).ReferedType->MyType == Type::Reference)) {
                tt1 = &*std::get<ReferenceDescriptorClass>(t1.Descriptor).ReferedType;
                tt2 = &*std::get<ReferenceDescriptorClass>(t2.Descriptor).ReferedType;
            }
            auto tc = CommonType(*(std::get<ReferenceDescriptorClass>(tt1->Descriptor).ReferedType), *(std::get<ReferenceDescriptorClass>(tt2->Descriptor).ReferedType));
            if (tc.MyType == Type::Undefined) {
                return tc;
            } else {
                // Use common type chain
                return t1;
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
            
        case Type::Class:
        {
            const ClassDescriptorClass &cd1 = std::get<ClassDescriptorClass>(t1.Descriptor);
            const ClassDescriptorClass &cd2 = std::get<ClassDescriptorClass>(t2.Descriptor);
            if (cd1 == cd2) {
                return t1;
            } else if (cd1.IsDerivedFrom(cd2)) {
                return t2;
            } else if (cd2.IsDerivedFrom(cd1)) {
                return t1;
            } else {
                return TypeDescriptorClass(Type::Undefined);
            }
        }

        case Type::Object:
        {
            const ObjectDescriptorClass &od1 = std::get<ObjectDescriptorClass>(t1.Descriptor);
            const ObjectDescriptorClass &od2 = std::get<ObjectDescriptorClass>(t2.Descriptor);
            if (od1 == od2) {
                return t1;
            } else if (od1.IsDerivedFrom(od2)) {
                return t2;
            } else if (od2.IsDerivedFrom(od1)) {
                return t1;
            } else {
                return TypeDescriptorClass(Type::Undefined);
            }
        }

        case Type::Dynamic:  return TypeDescriptorClass(Type::Undefined);
            //CommonType(*(std::get<DynamicDescriptorClass>(t1.Descriptor).CurrentType), *(std::get<DynamicDescriptorClass>(t2.Descriptor).CurrentType));
            
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
std::ostream &operator << (std::ostream &s, ArrayDescriptorClass const&t)
{
    s << *(t.BaseType) << "[";
    bool first = true;
    for (auto d: t.Dimensions) {
        if (first == false) {
            s << ",";
        }
        first = false;
        s << d;
    }
    s << "]";
    return s;
}


std::ostream &operator << (std::ostream &s, TypeDescriptorClass const&t)
{
    s << "type<";
    switch(t.MyType) {

    case ValueTypeDescriptorClass::Type::Undefined: s << "undef"; break;
    case ValueTypeDescriptorClass::Type::Integer:   s << "Integer"; break;
    case ValueTypeDescriptorClass::Type::Float:     s << "Float"; break;
    case ValueTypeDescriptorClass::Type::Bool:      s << "Bool"; break;
    case ValueTypeDescriptorClass::Type::String:    s << "String"; break;
    case ValueTypeDescriptorClass::Type::Type:      s << "Type"; break;
    case ValueTypeDescriptorClass::Type::Stack:     s << "Stack"; break;
    case ValueTypeDescriptorClass::Type::List:      s << "List"; break;
    case ValueTypeDescriptorClass::Type::Array:     s << std::get<ArrayDescriptorClass>(t.Descriptor); break;
    case ValueTypeDescriptorClass::Type::Map:       s << "Map"; break;
    case ValueTypeDescriptorClass::Type::Class:     s << "Class"; break;
    case ValueTypeDescriptorClass::Type::Object:    s << "Object"; break;
    case ValueTypeDescriptorClass::Type::Internal:  s << "Internal"; break;
    case ValueTypeDescriptorClass::Type::Function:  s << "Function"; break;
    case ValueTypeDescriptorClass::Type::Expression:s << "Expression"; break;
    case ValueTypeDescriptorClass::Type::Dynamic:   s << "Dynamic"; break;
    case ValueTypeDescriptorClass::Type::Reference: s << "->" << std::get<ReferenceDescriptorClass>(t.Descriptor).ReferedType; break;
    case ValueTypeDescriptorClass::Type::Illegal:   s << "illegal"; break;
        break;
    }
    s << ">";
    return s;
}


ArrayDescriptorClass::ArrayDescriptorClass(
    DimensionType Dimensions, std::unique_ptr<VariableTypeDescriptorClass> BaseType)
    : Dimensions(std::move(Dimensions)), BaseType(std::move(BaseType)) {}

ArrayDescriptorClass::ArrayDescriptorClass(std::vector<int64_t> UncheckedDimensions, std::unique_ptr<VariableTypeDescriptorClass> BaseType)
    : BaseType(std::move(BaseType))
{
    for(auto d: UncheckedDimensions) {
        if (d <= 0) {
            std::ostringstream s;
            s << "[";
            for(auto n: UncheckedDimensions) {
                s << n << ",";
            }
            s << "]";
            throw RuntimeErrorClass("invalid Dimension: " + s.str());
        }
        Dimensions.push_back(static_cast<DimensionType::value_type>(d));
    }
}



