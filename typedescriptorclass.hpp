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
namespace Variables {

   class ClassClass;
}


class VariableContextManageClass;

class StackDescriptorClass;
class ArrayDescriptorClass;
class MapDescriptorClass;
class ReferenceDescriptorClass;
class DynamicDescriptorClass;
class VariableTypeDescriptorClass;
class ObjectDescriptorClass;
class ClassDescriptorClass;

using ValueTypeDescriptor = std::variant<std::monostate, StackDescriptorClass, ArrayDescriptorClass, MapDescriptorClass, ReferenceDescriptorClass, ObjectDescriptorClass>;


#if 0
class ClassDescriptorClass  {
    std::string ClassName;

    std::shared_ptr<ClassDescriptorClass> BaseClass;

public:
    ClassDescriptorClass(std::string &ClassName_, std::shared_ptr<ClassDescriptorClass> BaseClass_ = nullptr) : ClassName(ClassName_), BaseClass(BaseClass_) {}
    ClassDescriptorClass(const ClassDescriptorClass &s) = default;
    ClassDescriptorClass &operator=(const ClassDescriptorClass &s) = default;
    bool operator ==(const ClassDescriptorClass &s) const {return ClassName == s.ClassName;}
    bool IsDerivedFrom(const ClassDescriptorClass &s) const {
        if (BaseClass == nullptr) {
            return false; // We have no baseclass
        } else if (BaseClass->ClassName == s.ClassName) {
            return true;
        } else {
            return (BaseClass->IsDerivedFrom(s));
        }
    }
};
#endif

class ObjectDescriptorClass  {
    std::shared_ptr<Variables::ClassClass> MyClass;

public:
    ObjectDescriptorClass(std::shared_ptr<Variables::ClassClass> MyClass_) : MyClass(MyClass_) {}
    ObjectDescriptorClass(const ObjectDescriptorClass &s) = default;
    ObjectDescriptorClass &operator=(const ObjectDescriptorClass &s) = default;
    bool operator ==(const ObjectDescriptorClass &s) const
    {
        // If we have just one class, we might just compare the pointer to it
        if (MyClass == s.MyClass) {
            return true;
        } else {
            return false;
           // return (MyClass->IsDerivedFrom(*s.MyClass)) || (s.MyClass->IsDerivedFrom(*MyClass));
        }
    }
    bool IsDerivedFrom(const ObjectDescriptorClass &s) const {
       // return MyClass->IsDerivedFrom(*s.MyClass);
    }
    std::shared_ptr<const Variables::ClassClass> GetClass() const { return MyClass;}
};

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
    enum class KeyTypesType {None = 0, Integer = 1, String = 2, Bool = 4};

    MapDescriptorClass(KeyTypesType PossibleKeys_, std::unique_ptr<VariableTypeDescriptorClass> BaseType) : PossibleKeys(PossibleKeys_), BaseType(std::move(BaseType)) {}
    MapDescriptorClass(const MapDescriptorClass &s) : PossibleKeys(s.PossibleKeys), BaseType(std::make_unique<VariableTypeDescriptorClass>(*s.BaseType)) {}
    MapDescriptorClass &operator=(const MapDescriptorClass &s)  {
        PossibleKeys = s.PossibleKeys;
        BaseType = std::make_unique<VariableTypeDescriptorClass>(*s.BaseType);
        return *this;
    }

private:
    KeyTypesType PossibleKeys;
    std::unique_ptr<VariableTypeDescriptorClass> BaseType;
};

inline constexpr MapDescriptorClass::KeyTypesType operator | (MapDescriptorClass::KeyTypesType const &k1, MapDescriptorClass::KeyTypesType const &k2) {return static_cast<MapDescriptorClass::KeyTypesType>(static_cast<int>(k1) | static_cast<int>(k2));}
inline constexpr MapDescriptorClass::KeyTypesType operator & (MapDescriptorClass::KeyTypesType const &k1, MapDescriptorClass::KeyTypesType const &k2) {return MapDescriptorClass::KeyTypesType(int(k1) & int(k2));}


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
        Type,
        Object,
        Class,
        Internal,
        Illegal  // $Internal flag
    };

    bool IsKindOf(Type t) const {return MyType == t;}
    template<class T>
    const T &GetTypeDetails() const
    {
        try {
            return std::get<T>(Descriptor);
        }
        catch (...){
            throw(INTERNAL_ERROR_OBJECT(std::string("Try to acces nonexistent Descriptor '") + typeid(T).name()));
        }
    }
    
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
        } else if (std::holds_alternative<ObjectDescriptorClass>(Descriptor)) {
            return Type::Object;
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
        case Type::Type:
        case Type::Internal:
        case Type::Dynamic:
        case Type::Class:
            return std::monostate();
            break;
            
        case Type::Stack:
        case Type::Array:
        case Type::Map:
        case Type::Reference:
        case Type::Object:
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
    VariableTypeDescriptorClass( VariableTypeDescriptorClass &&D) = default;
    VariableTypeDescriptorClass &operator =( VariableTypeDescriptorClass &&D) = default;

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
        (t == TypeDescriptorClass::Type::Internal) ||
        (t == TypeDescriptorClass::Type::Object) ||
        //(t == TypeDescriptorClass::Type::Class) ||
        (t == TypeDescriptorClass::Type::Illegal)) {
        return false;
    }
    return t == td.MyType;
}

inline bool operator != (TypeDescriptorClass const&td, TypeDescriptorClass::Type t)
{
    return ! (td == t);
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
    if (t1.MyType == TypeDescriptorClass::Type::Internal) {
        return false;
    }
    if (t1.MyType == TypeDescriptorClass::Type::Object) {
        return std::get<ObjectDescriptorClass>(t1.Descriptor) == std::get<ObjectDescriptorClass>(t2.Descriptor);
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

inline bool operator != (TypeDescriptorClass const&t1, TypeDescriptorClass const&t2)
{
    return !(t1 == t2);
}


#endif // TYPEDESCRIPTORCLASS_HPP
