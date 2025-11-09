#ifndef VARIABLECLASS_H
#define VARIABLECLASS_H


#include <iomanip>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <iostream>

class ExpressionClass;
class StatementClass;
class VariableClass;

class VariableContextClass;

class StackDescriptorClass;
class ArrayDescriptorClass;
class DynamicDescriptorClass;

using TypeDescriptor = std::variant<std::monostate, StackDescriptorClass, ArrayDescriptorClass, DynamicDescriptorClass>;

class StackDescriptorClass  {
public:
    StackDescriptorClass(const StackDescriptorClass &s);
    StackDescriptorClass &operator=(const StackDescriptorClass &s);
    std::unique_ptr<TypeDescriptor> BaseType;
};

class ArrayDescriptorClass  {
public:
    ArrayDescriptorClass(const ArrayDescriptorClass &s);
    ArrayDescriptorClass &operator = (const ArrayDescriptorClass &s);
    std::vector<int> Dimensions; // -1 = flexible dimension
    std::unique_ptr<TypeDescriptor> BaseType;
};


class DynamicDescriptorClass {
public:
    DynamicDescriptorClass(const DynamicDescriptorClass &s);
    DynamicDescriptorClass &operator = (const DynamicDescriptorClass &s);
    std::unique_ptr<TypeDescriptor> CurrentType;
};

inline DynamicDescriptorClass::DynamicDescriptorClass(const DynamicDescriptorClass &s)
    : CurrentType(std::make_unique<TypeDescriptor>(*s.CurrentType))
{
}

inline DynamicDescriptorClass &DynamicDescriptorClass::operator =(const DynamicDescriptorClass &s)
{
    CurrentType = std::make_unique<TypeDescriptor>(*s.CurrentType);
    return *this;
}

inline StackDescriptorClass::StackDescriptorClass(const StackDescriptorClass &s) :
    BaseType(std::make_unique<TypeDescriptor>(*s.BaseType))
{
}

inline StackDescriptorClass &StackDescriptorClass::operator=(const StackDescriptorClass &s)
{
    BaseType = std::make_unique<TypeDescriptor>(*s.BaseType);
    return *this;
}

inline ArrayDescriptorClass::ArrayDescriptorClass(const ArrayDescriptorClass &s)
    : Dimensions(s.Dimensions), BaseType(std::make_unique<TypeDescriptor>(*s.BaseType))
{

}

inline ArrayDescriptorClass &ArrayDescriptorClass::operator =(const ArrayDescriptorClass &s)
{
    Dimensions = s.Dimensions;
    BaseType = std::make_unique<TypeDescriptor>(*s.BaseType);
    return *this;
}



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
        Dynamic //, fixed
    };

    TypeDescriptorClass(const TypeDescriptor &Descriptor)
        : Descriptor(Descriptor) {SetTypeFromDescriptor();}

    TypeDescriptorClass(Type t)
        : MyType(t) {SetDescriptorFromType(t);}

   // TypeDescriptorClass &operator = (const TypeDescriptorClass &src) {MyType = src.MyType; Descriptor = src.Descriptor;}

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
        case Type::String:
            Descriptor = std::monostate();
            break;

        case Type::Stack:
        case Type::Array:
        case Type::Dynamic:
            throw std::runtime_error("Invalid Type");
            break;
        }
    }
    Type MyType;
    TypeDescriptor Descriptor;
};

namespace Variables {
class VariableContentClass;

class StackClass {
    std::vector<std::unique_ptr<VariableContentClass>> Data;
public:
    StackClass(const StackClass &s){}
    StackClass &operator = (const StackClass &s){}

};

class ListClass {
    std::list<std::unique_ptr<VariableContentClass>> Data;
public:
    ListClass(const ListClass &s){}
    ListClass &operator = (const ListClass &s){}
};

class ArrayClass {

    std::vector<std::unique_ptr<ArrayClass>> Data;
public:
    ArrayClass(const ArrayClass &s){}
    ArrayClass &operator = (const ArrayClass &s){}
};

class SparseArrayClass {

    std::map<int, std::unique_ptr<VariableContentClass>> Data;
public:
    SparseArrayClass(const SparseArrayClass &s){}
    SparseArrayClass &operator = (const SparseArrayClass &s){}
};

class MapClass {

    std::map<std::string, std::unique_ptr<VariableContentClass>> Data;
public:
    MapClass(const MapClass &s) {}
    MapClass &operator = (const MapClass &s){}
};

class FunctionDefinitionClass {
    std::list<std::shared_ptr<VariableClass>> Parameters;
    std::list<std::shared_ptr<StatementClass>> Statements;
public:
    FunctionDefinitionClass(const std::list<std::shared_ptr<VariableClass> > &Parameters, const std::list<std::shared_ptr<StatementClass> > &Statements);
    static FunctionDefinitionClass MakeEmpty() {return FunctionDefinitionClass(std::list<std::shared_ptr<VariableClass> >(), std::list<std::shared_ptr<StatementClass> >());}
public:
  //  FunctionDefinitionClass(const FunctionDefinitionClass &s);
  //  FunctionDefinitionClass &operator = (const FunctionDefinitionClass &s);
    void              Print(std::ostream &s) const;

};


class VariableContentClass {
    friend std::ostream &operator << (std::ostream &s, const VariableContentClass &v);

public:
    typedef std::variant<std::monostate,
                         int64_t,
                         double,
                         std::string,
                         StackClass,
                         ListClass,
                         ArrayClass,
                         MapClass,
                         std::shared_ptr<FunctionDefinitionClass>> dataType;

    VariableContentClass(const TypeDescriptorClass &Type_) : Type(Type_), Data(std::monostate()), AssignedExpression(nullptr) {}

    VariableContentClass(int64_t Value) : Type(TypeDescriptorClass(TypeDescriptorClass::Type::Float)), Data(Value), AssignedExpression(nullptr) {}
    VariableContentClass(double Value) : Type(TypeDescriptorClass(TypeDescriptorClass::Type::Integer)), Data(Value), AssignedExpression(nullptr) {}
    VariableContentClass(std::string Value) : Type(TypeDescriptorClass(TypeDescriptorClass::Type::String)), Data(Value), AssignedExpression(nullptr) {}
    VariableContentClass(std::shared_ptr<FunctionDefinitionClass> Value) : Type(TypeDescriptorClass(TypeDescriptorClass::Type::Function)), Data(Value), AssignedExpression(nullptr) {}


    const TypeDescriptorClass &getType() const;
    void setType(const TypeDescriptorClass &newType);
    template <class T>
        const T &GetValue() {
        return std::get<T>(Data);
    }
    template <class T>
    void SetValue(const T &v) {
        Data = v;
    }

private:
    dataType Data;
    TypeDescriptorClass Type;
    std::shared_ptr<ExpressionClass> AssignedExpression;


};

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

/*
std::monostate,
                         int64_t,
                         double,
                         std::string,
                         StackClass,
                         ListClass,
                         ArrayClass,
                         MapClass,
                         FunctionDefinitionClass
 */
inline std::ostream &operator << (std::ostream &s, const VariableContentClass &v)
{
    std::visit(overloaded{
                   [&s](const std::monostate &arg) { s << "empty"; },
                   [&s](int64_t arg) { s << arg << ' '; },
                   [&s](double arg) { s << std::fixed << arg << ' '; },
                   [&s](const StackClass &arg) { s << "<stack>"; },
                   [&s](const ListClass &arg) { s << "<list>"; },
                   [&s](const ArrayClass &arg) { s << "<Array>"; },
                   [&s](const MapClass &arg) { s << "<map>"; },
                   [&s](const std::shared_ptr<FunctionDefinitionClass> &arg) { s << "<function>"; },
                   [&s](const std::string& arg) { s << '"' << arg << '"'; }
               }, v.Data);
    return s;
}

inline bool operator ==(const VariableContentClass &r, const VariableContentClass &l)
{
    return false;
}

inline bool operator <(const VariableContentClass &r, const VariableContentClass &l)
{
    return false;
}

inline VariableContentClass operator *(const VariableContentClass &r, const VariableContentClass &l)
{
    return r;
}

inline VariableContentClass operator /(const VariableContentClass &r, const VariableContentClass &l)
{
    return r;
}

inline VariableContentClass operator +(const VariableContentClass &r, const VariableContentClass &l)
{
    return r;
}

inline VariableContentClass operator -(const VariableContentClass &r, const VariableContentClass &l)
{
    return r;
}

inline VariableContentClass operator -(const VariableContentClass &o)
{
    return o;
}

inline VariableContentClass log(const VariableContentClass &o)
{
    return o;
}

inline VariableContentClass exp(const VariableContentClass &o)
{
    return o;
}

inline VariableContentClass sqrt(const VariableContentClass &o)
{
    return o;
}

inline VariableContentClass pow(const VariableContentClass &l, const VariableContentClass &r)
{
    return l;
}

}
class VariableClass
{
    VariableContextClass *MyContext;
    const std::string Name;
    Variables::VariableContentClass Content;

public:
    VariableClass(const std::string &Name_, const TypeDescriptorClass &Type_) : MyContext(nullptr), Name(Name_), Content(Type_) {}
    virtual ~VariableClass() {}
    void SetContext(VariableContextClass *Context);
    const std::string GetName()  const {return Name;}
    virtual Variables::VariableContentClass      GetValue() const;
    virtual void        SetValue(Variables::VariableContentClass v);

    const TypeDescriptorClass &Type() {return GetType();}
private:
    virtual const TypeDescriptorClass &GetType() const;
};

class DoubleVariableClass : public VariableClass {

    double Value;

    // VariableClass interface
public:
    DoubleVariableClass(const std::string &Name_, double Value);
    virtual Variables::VariableContentClass GetValue() const override;
    virtual void SetValue(Variables::VariableContentClass v) override;
};

#endif // VARIABLECLASS_H
