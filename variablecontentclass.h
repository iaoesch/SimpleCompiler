#ifndef VARIABLECONTENTCLASS_H
#define VARIABLECONTENTCLASS_H


#include <list>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <variant>
#include <iostream>
#include "Errclass.hpp"
#include "environment.hpp"
#include "typedescriptorclass.hpp"

//#include "varmanag.hpp"
class ProxyVariableClass;



namespace Variables {

using DimensionType = ArrayDescriptorClass::DimensionType;
using IndexType = DimensionType::value_type;

struct IndexRangeType {IndexType From; IndexType To;};
typedef std::variant<IndexType, IndexRangeType> SingleElementSelectorType;
typedef std::vector<SingleElementSelectorType> ElementSelectorType;

class VariableContentClass;

class StackClass {
    std::vector<std::unique_ptr<VariableContentClass>> Data;
public:
    StackClass(const StackClass &s){ (void)s; SIGNAL_UNIMPLEMENTED();}
    StackClass &operator = (const StackClass &s){ (void)s; SIGNAL_UNIMPLEMENTED();}
    void PrintDetail(std::ostream &s, int Limit) const;

};

class ListClass {
    std::list<std::unique_ptr<VariableContentClass>> Data;
public:
    ListClass(const ListClass &s){ (void)s; SIGNAL_UNIMPLEMENTED();}
    ListClass &operator = (const ListClass &s){(void)s; SIGNAL_UNIMPLEMENTED();}

    void PrintDetail(std::ostream &s, int Limit) const;
};

class ArrayClass {

#ifdef USE_FLAT_MEMORY
    typedef std::variant<std::vector<int64_t>, std::vector<double>, std::vector<std::string>> DataType;
    //std::vector<std::unique_ptr<ArrayClass>> Data;
    DataType Data;
#endif
    using DimensionType = ArrayDescriptorClass::DimensionType;
    using IndexType = DimensionType::value_type;

public:
    struct Entry;


    //typedef std::variant<std::vector<int64_t>, std::vector<double>, std::vector<std::string>, std::vector<std::unique_ptr<Entry>>> RecursiveDataType;

    class Row {
        friend class ArrayClass;
        std::vector<std::unique_ptr<VariableContentClass>> Data;
    public:
        Row() = default;
        Row(const Row &src) {for (auto &r: src.Data) {Data.push_back(std::make_unique<VariableContentClass>(*r));}}
        Row(Row &&) = default;
        Row &operator=(const Row &src) {Data.clear(); for (auto &r: src.Data) {Data.push_back(std::make_unique<VariableContentClass>(*r));} return *this;}
        Row &operator=(Row &&) = default;
        void AppendElement(const VariableContentClass &e);

        uint32_t Size() const {return Data.size();}
        bool GetCommonType(ValueTypeDescriptorClass &Type) const;
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
    DimensionType Dimensions;
    VariableTypeDescriptorClass BaseType;

public:
    ArrayClass(const ArrayClass &s) = default; //{SIGNAL_UNIMPLEMENTED();}
    ArrayClass &operator = (const ArrayClass &s) = default; //{ (void)s; SIGNAL_UNIMPLEMENTED();}
    ArrayClass(ArrayClass &&s) = default; //{SIGNAL_UNIMPLEMENTED();}
    ArrayClass &operator = (ArrayClass &&s) = default; //{ (void)s; SIGNAL_UNIMPLEMENTED();}
    ArrayClass(const VectorOfRows &vr) : Data(vr), BaseType(TypeDescriptorClass::Type::Undefined) {CommonInitialization();}
    ArrayClass(const Row &r) : Data(r), BaseType(TypeDescriptorClass::Type::Undefined) {CommonInitialization();}
    ArrayClass(const ArrayContentType &r);

    ValueTypeDescriptorClass GetTypeDescriptor() const;

    static Row CreateRowOfValues() {return Row();}
    static VectorOfRows CreateRowOfRows() {return VectorOfRows();}
    void PrintDimensions(std::ostream &s) const;
    void PrintDetail(std::ostream &s, int Limit) const;

    ProxyVariableClass GetIndexedElement(std::string BaseName, ElementSelectorType Selector) const;
    VariableContentClass &GetIndexedElement(ElementSelectorType Selector) const;
private:

    void DetectArrayStructure(const ArrayContent &Data, DimensionType &Dimensions, ValueTypeDescriptorClass &ContentType, bool &SizeMissmatch, int Deepth = 0);
    void FillUpMissingElements(ArrayContent &Data, const DimensionType &Dimensions, const VariableContentClass &FillValue, unsigned int Deepth);
    void CommonInitialization();
    void PrintDetail(const ArrayContent &Data, std::ostream &s, int &Limit, int Indent) const;
    std::string ConvertIndexToText(ElementSelectorType Selector) const;
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
    SparseArrayClass(const SparseArrayClass &s){ (void)s; SIGNAL_UNIMPLEMENTED();}
    SparseArrayClass &operator = (const SparseArrayClass &s){ (void)s; SIGNAL_UNIMPLEMENTED();}

    void PrintDetail(std::ostream &s, int Limit) const;
};

class MapClass {

    typedef std::map<std::string, std::unique_ptr<VariableContentClass>> MapStringKeyType;
    typedef std::map<int64_t, std::unique_ptr<VariableContentClass>> MapIntegerKeyType;
    typedef std::map<std::variant<std::string, int64_t>, std::unique_ptr<VariableContentClass>> MapStringAndIntegerKeyType;
//    typedef std::variant<std::monostate, MapStringKeyType, MapIntegerKeyType, MapStringAndIntegerKeyType> MapType;
    typedef MapStringKeyType MapType;
    MapType Data;
    MapDescriptorClass::KeyTypesType KeyType;
    VariableTypeDescriptorClass BaseType;

public:
    MapClass(const MapClass &s) : KeyType(s.KeyType), BaseType(s.BaseType) {
        for (auto const &e: s.Data) {
            Data[e.first] = std::make_unique<VariableContentClass>(*(e.second));
        }
        } // = default; //{ (void)s; SIGNAL_UNIMPLEMENTED();}
    MapClass &operator = (const MapClass &s) { //= default; //{ (void)s; SIGNAL_UNIMPLEMENTED();}
        Data.clear();
        for (auto const &e: s.Data) {
            Data[e.first] = std::make_unique<VariableContentClass>(*(e.second));
        }
        return *this;
    } // = default; //{ (void)s; SIGNAL_UNIMPLEMENTED();}
    MapClass( MapClass &&s) = default; //{ (void)s; SIGNAL_UNIMPLEMENTED();}
    MapClass &operator = (MapClass &&s) = default; //{ (void)s; SIGNAL_UNIMPLEMENTED();}
    MapClass(MapType &&r) : Data(std::move(r)) , BaseType(TypeDescriptorClass::Type::Undefined)
    {
        //CommonInitialization();
    }

    ValueTypeDescriptorClass GetTypeDescriptor() const;

    void PrintDetail(std::ostream &s, int Limit) const;

    ProxyVariableClass GetIndexedElement(std::string BaseName, ElementSelectorType Selector) const;
    VariableContentClass &GetIndexedElement(ElementSelectorType Selector) const;

};

class FunctionDefinitionClass;


class
    VariableContentClass {
    friend std::ostream &operator << (std::ostream &s, const VariableContentClass &v);
    friend VariableContentClass operator + (const VariableContentClass &l, const VariableContentClass &r);
    friend bool operator <(const VariableContentClass &r, const VariableContentClass &l);
    friend bool operator >(const VariableContentClass &r, const VariableContentClass &l) {return l < r;}
    friend bool operator >=(const VariableContentClass &r, const VariableContentClass &l) {return !(r < l);}
    friend bool operator <=(const VariableContentClass &r, const VariableContentClass &l) {return !(l < r);}
    friend bool operator ==(const VariableContentClass &r, const VariableContentClass &l);
    friend bool operator !=(const VariableContentClass &r, const VariableContentClass &l) {return !(l==r);}

    typedef std::variant<std::monostate,
                         int64_t,
                         double,
                         std::string,
                         Variables::StackClass,
                         Variables::ListClass,
                         Variables::ArrayClass,
                         MapClass,
                         std::shared_ptr<ExpressionClass>,
                         std::shared_ptr<FunctionDefinitionClass>,
                         std::shared_ptr<VariableContentClass>> dataType;

public:

    VariableContentClass(const VariableContentClass &) = default;
    VariableContentClass(VariableContentClass &&) = default;
    VariableContentClass &operator=(const VariableContentClass &) = default;
    VariableContentClass &operator=(VariableContentClass &&) = default;
    VariableContentClass()
        : Data(std::monostate()), Type(ValueTypeDescriptorClass(TypeDescriptorClass::Type::Undefined)), AssignedExpression(nullptr) {}
    VariableContentClass(const ValueTypeDescriptorClass &Type_)
        : Data(std::monostate()), Type(Type_), AssignedExpression(nullptr) {}
    VariableContentClass(const VariableTypeDescriptorClass &Type_)
        : Data(std::monostate()), Type(Type_.ToValueType()), AssignedExpression(nullptr) {}

    VariableContentClass(int64_t Value) : Data(Value), Type(ValueTypeDescriptorClass(TypeDescriptorClass::Type::Integer)), AssignedExpression(nullptr) {}
    VariableContentClass(double Value) : Data(Value), Type(ValueTypeDescriptorClass(TypeDescriptorClass::Type::Float)), AssignedExpression(nullptr) {}
    VariableContentClass(std::string Value) : Data(Value), Type(ValueTypeDescriptorClass(TypeDescriptorClass::Type::String)), AssignedExpression(nullptr) {}
    VariableContentClass(Variables::ArrayClass Value) : Data(Value), Type(Value.GetTypeDescriptor()), AssignedExpression(nullptr) {}
    VariableContentClass(std::shared_ptr<FunctionDefinitionClass> Value) : Data(Value), Type(ValueTypeDescriptorClass(TypeDescriptorClass::Type::Function)), AssignedExpression(nullptr) {}
    VariableContentClass(std::shared_ptr<ExpressionClass> Value) : Data(Value), Type(ValueTypeDescriptorClass(TypeDescriptorClass::Type::Expression)), AssignedExpression(nullptr) {}
    // VariableContentClass(const VariableContentClass &s) : std::shared_ptr<FunctionDefinitionClass> Value) : Type(TypeDescriptorClass(TypeDescriptorClass::Type::Function)), Data(Value), AssignedExpression(nullptr) {}
//        Variables::StackClass,
//        Variables::ListClass,
//        MapClass,
//        std::shared_ptr<VariableContentClass>;

    bool Isempty() const {return std::holds_alternative<std::monostate>(Data);}
    VariableContentClass GetEmtpy() const {return VariableContentClass(this->Type);}

    static VariableContentClass MakeUndefined() {return VariableContentClass(ValueTypeDescriptorClass(TypeDescriptorClass::Type::Undefined));}

    void PrintDetail(std::ostream &s, int Limit) const;

    const ValueTypeDescriptorClass &getType() const;
    //void setType(const ValueTypeDescriptorClass &newType);

    const VariableTypeDescriptorClass &getContainedType() const
    {
        if (Type.IsKindOf(TypeDescriptorClass::Type::Array)) {
            return std::get<ArrayClass>(Data).BaseType;
        } else if(Type == TypeDescriptorClass::Type::Map) {
            throw RuntimeErrorClass("No Basetype availlable");
            //            return std::get<MapClass>(Data).GetIndexedElement(Selector);
        } else if(Type == TypeDescriptorClass::Type::List) {
            throw RuntimeErrorClass("No Basetype availlable");
            //            return std::get<ListClass>(Data).GetIndexedElement(Selector);
        } else {
            std::ostringstream Msg;
            Msg << "No Basetype availlable [" << Type << "]";
            throw RuntimeErrorClass(Msg.str());
        }

    }

    VariableContentClass &operator [](const ElementSelectorType &Selector) const
    {
        if (Type.IsKindOf(TypeDescriptorClass::Type::Array)) {
            return std::get<ArrayClass>(Data).GetIndexedElement(Selector);
        } else if(Type == TypeDescriptorClass::Type::Map) {
            throw RuntimeErrorClass("Indexing not possible");
//            return std::get<MapClass>(Data).GetIndexedElement(Selector);
        } else if(Type == TypeDescriptorClass::Type::List) {
            throw RuntimeErrorClass("Indexing not possible");
//            return std::get<ListClass>(Data).GetIndexedElement(Selector);
        } else {
            std::ostringstream Msg;
            Msg << "Indexing not possible [" << Type << "]";
            throw RuntimeErrorClass(Msg.str());
        }
    }

    template <class T>
        const T &GetValue() const {
        return std::get<T>(Data);
    }

    template <class T>
    bool holds_alternative() const {
        return std::holds_alternative<T>(Data);
    }

private:
    template <class T>
    void SetValue(const T &v) {
        Data = v;
    }
public:

#if 0
    void AssignValue(const VariableContentClass &v) {
        if (getType() == TypeDescriptorClass::Type::Dynamic) {
                Data = v.Data;
        } else if (v.getType() == getType()) {
           Data = v.Data;
        } else if ( (v.getType() == TypeDescriptorClass::Type::Integer)
             && (getType()  == TypeDescriptorClass::Type::Float)) {
            Data = double(std::get<int64_t>(v.Data));
        } else {
            throw INTERNAL_ERROR_OBJECT("Incompatible type for assignement");
        }
    }
#endif

private:
    dataType Data;
    ValueTypeDescriptorClass Type;
    std::shared_ptr<ExpressionClass> AssignedExpression;


};

inline ValueTypeDescriptorClass const &VariableContentClass::getType() const
{
    return Type;
}

/*
inline void VariableContentClass::setType(const ValueTypeDescriptorClass &newType)
{
    Type = newType;
}
*/

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
    (void)l;
    return r.GetEmtpy();
}

inline VariableContentClass operator /(const VariableContentClass &r, const VariableContentClass &l)
{
    (void)l;
    return r.GetEmtpy();
}

VariableContentClass operator +(const VariableContentClass &r, const VariableContentClass &l);

inline VariableContentClass operator -(const VariableContentClass &r, const VariableContentClass &l)
{
    (void)l;
    return r.GetEmtpy();
}

inline VariableContentClass operator -(const VariableContentClass &o)
{
    return o.GetEmtpy();
}

inline VariableContentClass log(const VariableContentClass &o)
{
    return o.GetEmtpy();
}

inline VariableContentClass exp(const VariableContentClass &o)
{
    return o.GetEmtpy();
}

inline VariableContentClass sqrt(const VariableContentClass &o)
{
    return o.GetEmtpy();
}

inline VariableContentClass pow(const VariableContentClass &l, const VariableContentClass &r)
{
    (void)r;
    return l.GetEmtpy();
}

class FunctionDefinitionClass {
public:
    typedef std::vector<Variables::VariableContentClass> LocalStorageType;
private:
    std::vector<std::shared_ptr<VariableClass>> Parameters;
    std::list<std::shared_ptr<StatementClass>> Statements;
    std::string Name;
    LocalStorageType StorageTemplate;
    mutable LocalStorageType ActiveStorage;
    std::shared_ptr<VariableClass> ReturnVariable;

public:
    FunctionDefinitionClass(const std::string &Name_, const std::vector<std::shared_ptr<VariableClass> > &Parameters, const std::list<std::shared_ptr<StatementClass> > &Statements, LocalStorageType StorageTemplate);
    FunctionDefinitionClass(FunctionDefinitionClass &&src) = default;
    FunctionDefinitionClass &operator =(const FunctionDefinitionClass &src) = default;
    FunctionDefinitionClass &operator =(FunctionDefinitionClass &&src) = default;

    static FunctionDefinitionClass MakeEmpty() {return FunctionDefinitionClass("<EmptyFkt>", std::vector<std::shared_ptr<VariableClass> >(), std::list<std::shared_ptr<StatementClass> >(), LocalStorageType());}
public:
    //  FunctionDefinitionClass(const FunctionDefinitionClass &s);
    //  FunctionDefinitionClass &operator = (const FunctionDefinitionClass &s);
    void              Print(std::ostream &s) const;
    const std::string &GetName() {return Name;}
    void DrawDeclarationNode(std::ostream &s, int MyNodeNumber) const;
    void DrawDefinitionNode(std::ostream &s, int MyNodeNumber) const;

    void CreateFrame() {ActiveStorage = StorageTemplate;}
    void ReleaseFrame() {ActiveStorage.clear();}
    void SetReturnValue(std::shared_ptr<VariableClass> ReturnVariable_) {ReturnVariable = ReturnVariable_;}
    VariableContentClass Execute(Environment &Env) const;// = 0;
    const VariableContentClass &GetTemplateContentForOffset(uint32_t Offset) const {return StorageTemplate.at(Offset);}
    VariableContentClass const &GetVariableContentForOffset(uint32_t Offset) const {return ActiveStorage.at(Offset);}
    VariableContentClass       &GetVariableContentWriteReferenceForOffset(uint32_t Offset) const {return ActiveStorage.at(Offset);}
    void                        SetVariableContentForOffset(uint32_t Offset, VariableContentClass const &v) {ActiveStorage.at(Offset) = v;}
    std::shared_ptr<VariableClass> GetParameterByName(std::string Name);
    std::shared_ptr<VariableClass> GetParameterByIndex(int i);
    TypeDescriptorClass const &GetReturnType() const;
  };
}

#endif // VARIABLECONTENTCLASS_H
