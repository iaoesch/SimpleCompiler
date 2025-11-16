#ifndef VARIABLECLASS_H
#define VARIABLECLASS_H


#include <list>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <iostream>
#include "Errclass.hpp"
#include "environment.hpp"
#include "typedescriptorclass.hpp"


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

#ifdef USE_FLAT_MEMORY
    typedef std::variant<std::vector<int64_t>, std::vector<double>, std::vector<std::string>> DataType;
    //std::vector<std::unique_ptr<ArrayClass>> Data;
    DataType Data;
#endif


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
        void AppendElement(const VariableContentClass &e);

        uint32_t Size() const {return Data.size();}
        bool GetCommonType(TypeDescriptorClass &Type) const;
    };

    class VectorOfRows {
        friend class ArrayClass;
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

        uint32_t Size() const {return Data.size();}

    };

    //typedef std::vector<std::unique_ptr<VariableContentClass>> Element;
    typedef std::variant<VectorOfRows, Row> ArrayContent;

    struct Entry {
        Entry(const VectorOfRows &vr) : Data(vr) {}
        Entry(const Row &vr) : Data(vr) {}
        Entry(const ArrayContent &vr) : Data(vr) {}
        ArrayContent Data;
    };

    typedef ArrayContent ArrayContentType;

    ArrayContent Data;
    std::vector<uint32_t> Dimensions;


public:
    ArrayClass(const ArrayClass &s) = default; //{SIGNAL_UNIMPLEMENTED();}
    ArrayClass &operator = (const ArrayClass &s){SIGNAL_UNIMPLEMENTED();}
    ArrayClass(const VectorOfRows &vr) : Data(vr) {CommonInitialization();}
    ArrayClass(const Row &r) : Data(r) {CommonInitialization();}
    ArrayClass(const ArrayContentType &r);

    TypeDescriptorClass GetTypeDescriptor() const;

    static Row CreateRowOfValues() {return Row();}
    static VectorOfRows CreateRowOfRows() {return VectorOfRows();}
    void PrintDimensions(std::ostream &s) const;

private:
    void DetectArrayStructure(const ArrayContent &Data, std::vector<uint32_t> &Dimensions, TypeDescriptorClass &ContentType, bool &SizeMissmatch, int Deepth = 0);
    void FillUpMissingElements(ArrayContent &Data, const std::vector<uint32_t> &Dimensions, const VariableContentClass &FillValue, int Deepth);
    void CommonInitialization();
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
    std::string Name;

public:
    FunctionDefinitionClass(const std::string &Name_, const std::list<std::shared_ptr<VariableClass> > &Parameters, const std::list<std::shared_ptr<StatementClass> > &Statements);
    static FunctionDefinitionClass MakeEmpty() {return FunctionDefinitionClass("<EmptyFkt>", std::list<std::shared_ptr<VariableClass> >(), std::list<std::shared_ptr<StatementClass> >());}
public:
  //  FunctionDefinitionClass(const FunctionDefinitionClass &s);
  //  FunctionDefinitionClass &operator = (const FunctionDefinitionClass &s);
    void              Print(std::ostream &s) const;
    const std::string &GetName() {return Name;}
    void DrawDeclarationNode(std::ostream &s, int MyNodeNumber) const;
    void DrawDefinitionNode(std::ostream &s, int MyNodeNumber) const;

    void Execute(Environment &Env) const;// = 0;


};


class VariableContentClass {
    friend std::ostream &operator << (std::ostream &s, const VariableContentClass &v);
    friend VariableContentClass operator + (const VariableContentClass &l, const VariableContentClass &r);
    friend bool operator <(const VariableContentClass &r, const VariableContentClass &l);
    friend bool operator >(const VariableContentClass &r, const VariableContentClass &l) {return l < r;}
    friend bool operator >=(const VariableContentClass &r, const VariableContentClass &l) {return !(r < l);}
    friend bool operator <=(const VariableContentClass &r, const VariableContentClass &l) {return !(l < r);}
    friend bool operator ==(const VariableContentClass &r, const VariableContentClass &l);
    friend bool operator !=(const VariableContentClass &r, const VariableContentClass &l) {return !(l==r);}

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
        : Data(std::monostate()), Type(TypeDescriptorClass(TypeDescriptorClass::Type::Undefined)), AssignedExpression(nullptr) {}
    VariableContentClass(const TypeDescriptorClass &Type_)
        : Data(std::monostate()), Type(Type_), AssignedExpression(nullptr) {}

    VariableContentClass(int64_t Value) : Data(Value), Type(TypeDescriptorClass(TypeDescriptorClass::Type::Float)), AssignedExpression(nullptr) {}
    VariableContentClass(double Value) : Data(Value), Type(TypeDescriptorClass(TypeDescriptorClass::Type::Integer)), AssignedExpression(nullptr) {}
    VariableContentClass(std::string Value) : Data(Value), Type(TypeDescriptorClass(TypeDescriptorClass::Type::String)), AssignedExpression(nullptr) {}
    VariableContentClass(Variables::ArrayClass Value) : Data(Value), Type(Value.GetTypeDescriptor()), AssignedExpression(nullptr) {}
    VariableContentClass(std::shared_ptr<FunctionDefinitionClass> Value) : Data(Value), Type(TypeDescriptorClass(TypeDescriptorClass::Type::Function)), AssignedExpression(nullptr) {}
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

    void AssignValue(const VariableContentClass &v) {
        if (getType() == TypeDescriptorClass::Type::Dynamic) {
            if (v.getType() == TypeDescriptorClass::Type::Dynamic) {
                Data = v.Data;
                Type.ChangeDynamicType(v.getType().GetDynamicType());
            } else {
                Data = v.Data;
                Type.ChangeDynamicType(v.getType());
            }
        } else if (v.getType() == getType()) {
           Data = v.Data;
        } else if ( (v.getType() == TypeDescriptorClass::Type::Integer)
             && (getType()  == TypeDescriptorClass::Type::Float)) {
            Data = double(std::get<int64_t>(v.Data));
        } else {
            throw ERROR_OBJECT("Incompatible type for assignement");
        }
    }

private:
    dataType Data;
    TypeDescriptorClass Type;
    std::shared_ptr<ExpressionClass> AssignedExpression;


};

inline TypeDescriptorClass const &VariableContentClass::getType() const
{
    return Type;
}

inline void VariableContentClass::setType(const TypeDescriptorClass &newType)
{
    Type = newType;
}

inline void Variables::ArrayClass::Row::AppendElement(Variables::VariableContentClass const &e)
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

bool operator <(const VariableContentClass &r, const VariableContentClass &l);

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
    virtual void        DrawNode(std::ostream &s, int MyNodeNumber) const;


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
