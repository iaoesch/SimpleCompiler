#ifndef TYPEDESCRIPTORCLASS_HPP
#define TYPEDESCRIPTORCLASS_HPP
#include <stdexcept>
#include <vector>
#include <variant>

#include <variant>
#include "Errclass.hpp"


class ExpressionClass;
class StatementClass;
class VariableClass;


class VariableContextClass;

class StackDescriptorClass;
class ArrayDescriptorClass;
class DynamicDescriptorClass;
class TypeDescriptorClass;

using TypeDescriptor = std::variant<std::monostate, StackDescriptorClass, ArrayDescriptorClass, DynamicDescriptorClass>;


class StackDescriptorClass  {
public:
    StackDescriptorClass(const StackDescriptorClass &s);
    StackDescriptorClass &operator=(const StackDescriptorClass &s);
    std::unique_ptr<TypeDescriptorClass> BaseType;
};

class ArrayDescriptorClass  {
public:
    ArrayDescriptorClass(std::vector<int> Dimensions,
                         std::unique_ptr<TypeDescriptorClass> BaseType);
    ArrayDescriptorClass(const ArrayDescriptorClass &s);
    ArrayDescriptorClass &operator=(const ArrayDescriptorClass &s);
    std::vector<int> Dimensions; // -1 = flexible dimension
    std::unique_ptr<TypeDescriptorClass> BaseType;
};


class DynamicDescriptorClass {
public:
    DynamicDescriptorClass(const DynamicDescriptorClass &s);
    DynamicDescriptorClass &operator = (const DynamicDescriptorClass &s);
    std::unique_ptr<TypeDescriptorClass> CurrentType;
};




class TypeDescriptorClass {
    
public:
    enum class Type {
        Undefined,
        Integer,
        Float,
        Bool,
        String,
        Stack,
        List,
        Array,
        Map,
        Function,
        Expression,
        Dynamic, //, fixed
        Illegal  // $Internal flag
    };
    
    TypeDescriptorClass(const TypeDescriptor &Descriptor)
        : Descriptor(Descriptor) {SetTypeFromDescriptor();}
    
    TypeDescriptorClass(Type t)
        : MyType(t) {SetDescriptorFromType(t);}
    
    // TypeDescriptorClass &operator = (const TypeDescriptorClass &src) {MyType = src.MyType; Descriptor = src.Descriptor;}
    
    friend TypeDescriptorClass CommonType(const TypeDescriptorClass &t1, const TypeDescriptorClass &t2);
    friend bool operator == (TypeDescriptorClass const&td, TypeDescriptorClass::Type t);
    friend bool operator == (TypeDescriptorClass const&t1, TypeDescriptorClass const&t2);
    
    void ChangeDynamicType(const TypeDescriptorClass& NewType);
    const TypeDescriptorClass& GetDynamicType() const;
    
private:
    void SetTypeFromDescriptor() {
        if (std::holds_alternative<StackDescriptorClass>(Descriptor)) {
            MyType = Type::Stack;
        } else if (std::holds_alternative<ArrayDescriptorClass>(Descriptor)) {
            MyType = Type::Array;
        } else if (std::holds_alternative<DynamicDescriptorClass>(Descriptor)) {
            MyType = Type::Dynamic;
        } else {
            throw std::runtime_error("Inconsistent type state");
        }
    }
    
    void SetDescriptorFromType(Type t) {
        switch(t) {
            
        case Type::Integer:
        case Type::Float:
        case Type::Bool:
        case Type::List:
        case Type::Map:
        case Type::Function:
        case Type::Expression:
        case Type::String:
        case Type::Undefined:
        case Type::Dynamic:
            Descriptor = std::monostate();
            break;
            
        case Type::Stack:
        case Type::Array:
        case Type::Illegal:
            throw ERROR_OBJECT("Invalid Type");
            break;
        }
    }
    Type MyType;
    TypeDescriptor Descriptor;
};



inline DynamicDescriptorClass::DynamicDescriptorClass(const DynamicDescriptorClass &s)
    : CurrentType(std::make_unique<TypeDescriptorClass>(*s.CurrentType))
{
}

inline DynamicDescriptorClass &DynamicDescriptorClass::operator =(const DynamicDescriptorClass &s)
{
    CurrentType = std::make_unique<TypeDescriptorClass>(*s.CurrentType);
    return *this;
}

inline StackDescriptorClass::StackDescriptorClass(const StackDescriptorClass &s) :
    BaseType(std::make_unique<TypeDescriptorClass>(*s.BaseType))
{
}

inline StackDescriptorClass &StackDescriptorClass::operator=(const StackDescriptorClass &s)
{
    BaseType = std::make_unique<TypeDescriptorClass>(*s.BaseType);
    return *this;
}

inline ArrayDescriptorClass::ArrayDescriptorClass(const ArrayDescriptorClass &s)
    : Dimensions(s.Dimensions), BaseType(std::make_unique<TypeDescriptorClass>(*s.BaseType))
{

}

inline ArrayDescriptorClass &ArrayDescriptorClass::operator =(const ArrayDescriptorClass &s)
{
    Dimensions = s.Dimensions;
    BaseType = std::make_unique<TypeDescriptorClass>(*s.BaseType);
    return *this;
}






TypeDescriptorClass CommonType(const TypeDescriptorClass &t1, const TypeDescriptorClass &t2);



inline bool operator == (TypeDescriptorClass const&td, TypeDescriptorClass::Type t)
{
    // If we just compare by typeflag stacks and arrays will never match
    // As we also must compare basetype or dimensions
    if ((t == TypeDescriptorClass::Type::Stack) ||
        (t == TypeDescriptorClass::Type::Array) ||
        (t == TypeDescriptorClass::Type::Illegal)) {
        return false;
    }
    return t == td.MyType;
}

inline bool operator == (TypeDescriptorClass const&t1, TypeDescriptorClass const&t2)
{
    if (t1.MyType != t2.MyType) {
        return false;
    }
    // Here t1.Mytpe and t2.mytype are same, so we must check only one type
    if (t1.MyType == TypeDescriptorClass::Type::Illegal) {
        return false;
    }
    if (t1.MyType == TypeDescriptorClass::Type::Stack) {
        return std::get<StackDescriptorClass>(t1.Descriptor).BaseType->MyType == std::get<StackDescriptorClass>(t2.Descriptor).BaseType->MyType;
    }
    // Array: Basetype and dimensions must match
    if (t1.MyType == TypeDescriptorClass::Type::Array) {
        if (std::get<ArrayDescriptorClass>(t1.Descriptor).BaseType->MyType == std::get<ArrayDescriptorClass>(t2.Descriptor).BaseType->MyType) {
            if (std::get<ArrayDescriptorClass>(t1.Descriptor).Dimensions == std::get<ArrayDescriptorClass>(t2.Descriptor).Dimensions) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
    // At this point we should have sorted out all unmatching pairs
    return true;
}

#endif // TYPEDESCRIPTORCLASS_HPP
