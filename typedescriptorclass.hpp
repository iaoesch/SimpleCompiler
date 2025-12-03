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
class MapDescriptorClass;
class ReferenceDescriptorClass;
class DynamicDescriptorClass;
class VariableTypeDescriptorClass;

using ValueTypeDescriptor = std::variant<std::monostate, StackDescriptorClass, ArrayDescriptorClass, MapDescriptorClass, ReferenceDescriptorClass>;


class StackDescriptorClass  {
public:
    StackDescriptorClass(std::unique_ptr<VariableTypeDescriptorClass> &&BaseType_) : BaseType(std::move(BaseType_)) {}
    StackDescriptorClass(const StackDescriptorClass &s);
    StackDescriptorClass &operator=(const StackDescriptorClass &s);
    std::unique_ptr<VariableTypeDescriptorClass> BaseType;
};

class ArrayDescriptorClass  {
    friend std::ostream &operator << (std::ostream &s, ArrayDescriptorClass const&t);

public:
    typedef std::vector<uint64_t> DimensionType;
    ArrayDescriptorClass(DimensionType Dimensions,
                         std::unique_ptr<VariableTypeDescriptorClass> BaseType);
    ArrayDescriptorClass(std::vector<int64_t> UncheckedDimensions,
                         std::unique_ptr<VariableTypeDescriptorClass> BaseType);
    ArrayDescriptorClass(const ArrayDescriptorClass &s);
    ArrayDescriptorClass &operator=(const ArrayDescriptorClass &s);
    DimensionType Dimensions; // -1 = flexible dimension
    std::unique_ptr<VariableTypeDescriptorClass> BaseType;
};

class MapDescriptorClass  {
public:
    enum class KeyTypes {Integer = 1, String = 2, Bool = 4};

    MapDescriptorClass(KeyTypes PossibleKeys_) : PossibleKeys(PossibleKeys_) {}
    MapDescriptorClass(const MapDescriptorClass &s) = default;
    MapDescriptorClass &operator=(const MapDescriptorClass &s) = default;
    KeyTypes PossibleKeys;
};

inline MapDescriptorClass::KeyTypes operator | (MapDescriptorClass::KeyTypes k1, MapDescriptorClass::KeyTypes k2) {return MapDescriptorClass::KeyTypes(int(k1) | int(k2));}
inline MapDescriptorClass::KeyTypes operator & (MapDescriptorClass::KeyTypes k1, MapDescriptorClass::KeyTypes k2) {return MapDescriptorClass::KeyTypes(int(k1) & int(k2));}


class DynamicDescriptorClass {
public:
    DynamicDescriptorClass(const DynamicDescriptorClass &s);
    DynamicDescriptorClass &operator = (const DynamicDescriptorClass &s);
    std::unique_ptr<VariableTypeDescriptorClass> CurrentType;
};

class ReferenceDescriptorClass {
public:
    ReferenceDescriptorClass(const ReferenceDescriptorClass &s) : ReferedType(std::make_unique<VariableTypeDescriptorClass>(*(s.ReferedType))) {}
    ReferenceDescriptorClass &operator = (const ReferenceDescriptorClass &s) {ReferedType = std::make_unique<VariableTypeDescriptorClass>(*(s.ReferedType)); return *this;}
    std::unique_ptr<VariableTypeDescriptorClass> ReferedType;
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
        Dynamic,
        Reference,
        Illegal  // $Internal flag
    };

    bool IsKindOf(Type t) const {return MyType == t;}
    
protected:
    TypeDescriptorClass(const ValueTypeDescriptor &Descriptor)
        : MyType(TypeFromDescriptor(Descriptor)), Descriptor(Descriptor) {}
    
    TypeDescriptorClass(Type t)
        : MyType(t), Descriptor(DescriptorFromType(t)) {}

    TypeDescriptorClass(const ValueTypeDescriptor &Descriptor, Type t)
        : MyType(t), Descriptor(Descriptor) {}


private:

    // ValueTypeDescriptorClass &operator = (const ValueTypeDescriptorClass &src) {MyType = src.MyType; Descriptor = src.Descriptor;}
    
    friend TypeDescriptorClass CommonType(const TypeDescriptorClass &t1, const TypeDescriptorClass &t2);
    friend bool operator == (TypeDescriptorClass const&td, TypeDescriptorClass::Type t);
    friend bool operator == (TypeDescriptorClass const&t1, TypeDescriptorClass const&t2);
    friend std::ostream &operator << (std::ostream &s, TypeDescriptorClass const&t);

  //  void ChangeDynamicType(const ValueTypeDescriptorClass& NewType);
  //  const ValueTypeDescriptorClass& GetDynamicType() const;
    
protected:
    Type TypeFromDescriptor(const ValueTypeDescriptor &Descriptor) {
        if (std::holds_alternative<StackDescriptorClass>(Descriptor)) {
            return Type::Stack;
        } else if (std::holds_alternative<ArrayDescriptorClass>(Descriptor)) {
            return Type::Array;
        } else if (std::holds_alternative<MapDescriptorClass>(Descriptor)) {
            return Type::Map;
        } else if (std::holds_alternative<ReferenceDescriptorClass>(Descriptor)) {
            return Type::Reference;
        } else {
            throw std::runtime_error("Inconsistent type state");
        }
    }
    
    ValueTypeDescriptor DescriptorFromType(Type t) {
        switch(t) {
            
        case Type::Integer:
        case Type::Float:
        case Type::Bool:
        case Type::List:
        case Type::Function:
        case Type::Expression:
        case Type::String:
        case Type::Undefined:
        case Type::Dynamic:
            return std::monostate();
            break;
            
        case Type::Stack:
        case Type::Array:
        case Type::Map:
        case Type::Reference:
        case Type::Illegal:
            throw INTERNAL_ERROR_OBJECT("Invalid Type");
            break;
        }
    }
   // friend class VariableTypeDescriptorClass;



    Type MyType;
    ValueTypeDescriptor Descriptor;
};

class ValueTypeDescriptorClass : public TypeDescriptorClass {
public:
    ValueTypeDescriptorClass(const ValueTypeDescriptor &Descriptor_)
        : TypeDescriptorClass(Descriptor_) {ThrowOnInvalidType(Descriptor_);}

    ValueTypeDescriptorClass(Type t)
        : TypeDescriptorClass(t) {ThrowOnInvalidType(t);}

    //   ValueTypeDescriptorClass(VariableTypeDescriptorClass const &VariableType)
    //       : TypeDescriptorClass(VariableType) {ThrowOnInvalidType(VariableType);}

private:
    friend class VariableTypeDescriptorClass;
    ValueTypeDescriptorClass(const ValueTypeDescriptor &Descriptor_, Type t)
        : TypeDescriptorClass(Descriptor_, t) {ThrowOnInvalidType(t); ThrowOnInvalidType(Descriptor_);}

    void ThrowOnInvalidType(const ValueTypeDescriptor &Descriptor_)
    { (void)Descriptor_; /* No invalid type in descriptor for now*/}

    void ThrowOnInvalidType(Type t)
    {   if (/*(t == Type::Undefined) || (*/ t == Type::Dynamic) {
            throw INTERNAL_ERROR_OBJECT("Invalid Type");
        }
    }

 //   void ThrowOnInvalidType(const TypeDescriptorClass &VariableType)
 //   {
 //       ThrowOnInvalidType(VariableType.MyType);
 //       ThrowOnInvalidType(VariableType.Descriptor);
 //   }


};

class VariableTypeDescriptorClass : public TypeDescriptorClass {
public:
    VariableTypeDescriptorClass(const VariableTypeDescriptorClass &D) = default;
    VariableTypeDescriptorClass &operator =(const VariableTypeDescriptorClass &D) = default;

    VariableTypeDescriptorClass(const ValueTypeDescriptor &Descriptor_)
        : TypeDescriptorClass(Descriptor_) {ThrowOnInvalidType(Descriptor_);}

    VariableTypeDescriptorClass(Type t)
        : TypeDescriptorClass(t) {ThrowOnInvalidType(t);}

    VariableTypeDescriptorClass(ValueTypeDescriptorClass desc)
        : TypeDescriptorClass(desc) {/*ThrowOnInvalidType(t);*/}

    explicit VariableTypeDescriptorClass(const TypeDescriptorClass &desc)
        : TypeDescriptorClass(desc) {/*ThrowOnInvalidType(t);*/}

    ValueTypeDescriptorClass ToValueType() const
    {
        auto t = MyType;
        if (t == TypeDescriptorClass::Type::Dynamic) {
            t = TypeDescriptorClass::Type::Undefined;
        }

        return ValueTypeDescriptorClass(Descriptor, t);
    }

private:
    void ThrowOnInvalidType(const ValueTypeDescriptor &Descriptor_)
    { (void)Descriptor_;/* No invalid type in descriptor for now*/}

    void ThrowOnInvalidType(Type t)
    {
        (void)t;
    }

};


#if 0

inline DynamicDescriptorClass::DynamicDescriptorClass(const DynamicDescriptorClass &s)
    : CurrentType(std::make_unique<ValueTypeDescriptorClass>(*s.CurrentType))
{
}

inline DynamicDescriptorClass &DynamicDescriptorClass::operator =(const DynamicDescriptorClass &s)
{
    CurrentType = std::make_unique<ValueTypeDescriptorClass>(*s.CurrentType);
    return *this;
}
#endif

inline StackDescriptorClass::StackDescriptorClass(const StackDescriptorClass &s) :
    BaseType(std::make_unique<VariableTypeDescriptorClass>(*s.BaseType))
{
}

inline StackDescriptorClass &StackDescriptorClass::operator=(const StackDescriptorClass &s)
{
    BaseType = std::make_unique<VariableTypeDescriptorClass>(*s.BaseType);
    return *this;
}

inline ArrayDescriptorClass::ArrayDescriptorClass(const ArrayDescriptorClass &s)
    : Dimensions(s.Dimensions), BaseType(std::make_unique<VariableTypeDescriptorClass>(*s.BaseType))
{

}

inline ArrayDescriptorClass &ArrayDescriptorClass::operator =(const ArrayDescriptorClass &s)
{
    Dimensions = s.Dimensions;
    BaseType = std::make_unique<VariableTypeDescriptorClass>(*s.BaseType);
    return *this;
}






TypeDescriptorClass CommonType(const TypeDescriptorClass &t1, const TypeDescriptorClass &t2);



inline bool operator == (TypeDescriptorClass const&td, TypeDescriptorClass::Type t)
{
    // If we just compare by typeflag stacks and arrays will never match
    // As we also must compare basetype or dimensions
    // Not implemented yet
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
    if (t1.MyType == TypeDescriptorClass::Type::Reference) {
        return std::get<ReferenceDescriptorClass>(t1.Descriptor).ReferedType->MyType == std::get<ReferenceDescriptorClass>(t2.Descriptor).ReferedType->MyType;
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
