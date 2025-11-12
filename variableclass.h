#ifndef VARIABLECLASS_H
#define VARIABLECLASS_H


#include <iomanip>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <iostream>
#include "Errclass.hpp"

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
    ArrayDescriptorClass(std::vector<int> Dimensions,
                         std::unique_ptr<TypeDescriptor> BaseType)
        : Dimensions(std::move(Dimensions)), BaseType(std::move(BaseType)) {}
    ArrayDescriptorClass(const ArrayDescriptorClass &s);
    ArrayDescriptorClass &operator=(const ArrayDescriptorClass &s);
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
    friend TypeDescriptorClass CommonType(const TypeDescriptorClass &t1, const TypeDescriptorClass &t2);
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
        case Type::Undefined:
        case Type::Dynamic:
            Descriptor = std::monostate();
            break;

        case Type::Stack:
        case Type::Array:
            throw std::runtime_error("Invalid Type");
            break;
        }
    }
    Type MyType;
    TypeDescriptor Descriptor;
};
TypeDescriptorClass CommonType(const TypeDescriptorClass &t1, const TypeDescriptorClass &t2);

namespace Variables {

class VariableContentClass;

class StackClass {
    std::vector<std::unique_ptr<VariableContentClass>> Data;
public:
    StackClass(const StackClass &s){SIGNAL_UNIMPLEMENTED();}
    StackClass &operator = (const StackClass &s){SIGNAL_UNIMPLEMENTED();}

};

class ListClass {
    std::list<std::unique_ptr<VariableContentClass>> Data;
public:
    ListClass(const ListClass &s){SIGNAL_UNIMPLEMENTED();}
    ListClass &operator = (const ListClass &s){SIGNAL_UNIMPLEMENTED();}
};

class ArrayClass {
    std::vector<uint32_t> Dimensions;
    typedef std::variant<std::vector<int64_t>, std::vector<double>, std::vector<std::string>> DataType;
    //std::vector<std::unique_ptr<ArrayClass>> Data;
    DataType Data;

    // or
public:
    struct Entry;

    //typedef std::variant<std::vector<int64_t>, std::vector<double>, std::vector<std::string>, std::vector<std::unique_ptr<Entry>>> RecursiveDataType;

    class Row {
        std::vector<std::unique_ptr<VariableContentClass>> Data;
    public:
        Row() = default;
        Row(const Row &src) {for (auto &r: src.Data) {Data.push_back(std::make_unique<VariableContentClass>(*r));}}
        Row(Row &&) = default;
        Row &operator=(const Row &src) {Data.clear(); for (auto &r: src.Data) {Data.push_back(std::make_unique<VariableContentClass>(*r));} return *this;}
        Row &operator=(Row &&) = default;
        void AppendElement(VariableContentClass &e);
    };

    class VectorOfRows {
        std::vector<std::unique_ptr<Entry>> Data;
    public:
        VectorOfRows() = default;
        VectorOfRows(const VectorOfRows &src) {for (auto &r: src.Data) {Data.push_back(std::make_unique<Variables::ArrayClass::Entry>(*r));}}
        VectorOfRows(VectorOfRows &&) = default;
        VectorOfRows &operator=(const VectorOfRows &src) {Data.clear(); for (auto &r: src.Data) {Data.push_back(std::make_unique<Variables::ArrayClass::Entry>(*r));} return *this;}

        VectorOfRows &operator=(VectorOfRows &&) = default;
        explicit VectorOfRows(std::vector<std::unique_ptr<Entry>> Data)
            : Data(std::move(Data)) {}
        void AppendElement(const Row &e);
        void AppendElement(const VectorOfRows &e);
        void AppendElement(const std::variant<VectorOfRows, Row> &e);
    };

    //typedef std::vector<std::unique_ptr<VariableContentClass>> Element;
    typedef std::variant<VectorOfRows, Row> ArrayContent;

    struct Entry {
        Entry(const VectorOfRows &vr) : Data(vr) {}
        Entry(const Row &vr) : Data(vr) {}
        Entry(const ArrayContent &vr) : Data(vr) {}
        ArrayContent Data;
    };

    ArrayContent Data2;

    typedef ArrayContent ArrayContentType;

public:
    ArrayClass(const ArrayClass &s) = default; //{SIGNAL_UNIMPLEMENTED();}
    ArrayClass &operator = (const ArrayClass &s){SIGNAL_UNIMPLEMENTED();}
    ArrayClass(const VectorOfRows &vr) : Data2(vr) {}
    ArrayClass(const Row &r) : Data2(r) {}
    ArrayClass(const ArrayContentType &r) : Data2(r) {}

    TypeDescriptorClass GetTypeDescriptor() const;

    static Row CreateRowOfValues() {return Row();}
    static VectorOfRows CreateRowOfRows() {return VectorOfRows();}
};


inline void ArrayClass::VectorOfRows::AppendElement(const Row &e)
{
    Data.push_back(std::make_unique<Entry>(e));
}

inline void ArrayClass::VectorOfRows::AppendElement(const VectorOfRows &e)
{
    Data.push_back(std::make_unique<Entry>(e));
}

inline void ArrayClass::VectorOfRows::AppendElement(const std::variant<VectorOfRows, Row> &e)
{
    Data.push_back(std::make_unique<Entry>(e));
}


class SparseArrayClass {

    std::map<int, std::unique_ptr<VariableContentClass>> Data;
public:
    SparseArrayClass(const SparseArrayClass &s){SIGNAL_UNIMPLEMENTED();}
    SparseArrayClass &operator = (const SparseArrayClass &s){SIGNAL_UNIMPLEMENTED();}
};

class MapClass {

    std::map<std::string, std::unique_ptr<VariableContentClass>> Data;
public:
    MapClass(const MapClass &s) {SIGNAL_UNIMPLEMENTED();}
    MapClass &operator = (const MapClass &s){SIGNAL_UNIMPLEMENTED();}
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
    friend VariableContentClass operator + (const VariableContentClass &l, const VariableContentClass &r);

public:
    typedef std::variant<std::monostate,
                         int64_t,
                         double,
                         std::string,
                         Variables::StackClass,
                         Variables::ListClass,
                         Variables::ArrayClass,
                         MapClass,
                         std::shared_ptr<ExpressionClass>,
                         std::shared_ptr<FunctionDefinitionClass>> dataType;

    VariableContentClass(const VariableContentClass &) = default;
    VariableContentClass(VariableContentClass &&) = default;
    VariableContentClass &operator=(const VariableContentClass &) = default;
    VariableContentClass &operator=(VariableContentClass &&) = default;
    VariableContentClass()
        : Type(TypeDescriptorClass(TypeDescriptorClass::Type::Undefined)), Data(std::monostate()), AssignedExpression(nullptr) {}
    VariableContentClass(const TypeDescriptorClass &Type_)
        : Type(Type_), Data(std::monostate()), AssignedExpression(nullptr) {}

    VariableContentClass(int64_t Value) : Type(TypeDescriptorClass(TypeDescriptorClass::Type::Float)), Data(Value), AssignedExpression(nullptr) {}
    VariableContentClass(double Value) : Type(TypeDescriptorClass(TypeDescriptorClass::Type::Integer)), Data(Value), AssignedExpression(nullptr) {}
    VariableContentClass(std::string Value) : Type(TypeDescriptorClass(TypeDescriptorClass::Type::String)), Data(Value), AssignedExpression(nullptr) {}
    VariableContentClass(Variables::ArrayClass Value) : Type(TypeDescriptorClass(TypeDescriptorClass::Type::String)), Data(Value), AssignedExpression(nullptr) {}
    VariableContentClass(std::shared_ptr<FunctionDefinitionClass> Value) : Type(TypeDescriptorClass(TypeDescriptorClass::Type::Function)), Data(Value), AssignedExpression(nullptr) {}
   // VariableContentClass(const VariableContentClass &s) : std::shared_ptr<FunctionDefinitionClass> Value) : Type(TypeDescriptorClass(TypeDescriptorClass::Type::Function)), Data(Value), AssignedExpression(nullptr) {}

    static VariableContentClass MakeUndefined() {return VariableContentClass(TypeDescriptorClass(TypeDescriptorClass::Type::Undefined));}

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

inline void Variables::ArrayClass::Row::AppendElement(Variables::VariableContentClass &e)
{
    Data.push_back(std::make_unique<VariableContentClass>(e));
}


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
std::ostream &operator << (std::ostream &s, const VariableContentClass &v);

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

VariableContentClass operator +(const VariableContentClass &r, const VariableContentClass &l);

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
    virtual void        Print(std::ostream &s);

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
