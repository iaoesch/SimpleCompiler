#ifndef PREDEFINEDFUNCTIONHELPER_HPP
#define PREDEFINEDFUNCTIONHELPER_HPP

#include "precompiledfunctionmanagerclass.h"
#include "typedescriptorclass.hpp"
#include "variablecontentclass.h"


template <class T>
struct Convertor {
    static VariableTypeDescriptorClass MakeDescriptor() {return VariableTypeDescriptorClass(TypeDescriptorClass::Type::Undefined);}
};

template <>
struct Convertor<int64_t> {
    static VariableTypeDescriptorClass MakeDescriptor() {return VariableTypeDescriptorClass(TypeDescriptorClass::Type::Integer);}
};

template <>
struct Convertor<double> {
    static VariableTypeDescriptorClass MakeDescriptor() {return VariableTypeDescriptorClass(TypeDescriptorClass::Type::Float);}
};

template <>
struct Convertor<std::string> {
    static VariableTypeDescriptorClass MakeDescriptor() {return VariableTypeDescriptorClass(TypeDescriptorClass::Type::String);}
};

template <>
struct Convertor<Variables::ListClass> {
    static VariableTypeDescriptorClass MakeDescriptor() {return VariableTypeDescriptorClass(TypeDescriptorClass::Type::List);}
};

template <>
struct Convertor<Variables::ArrayClass> {
    static VariableTypeDescriptorClass MakeDescriptor(Variables::ArrayClass &a) {return a.GetTypeDescriptor();}
};
#ifdef JustFor //Reference
typedef std::variant<std::monostate,
                     int64_t,
                     double,
                     std::string,
                     Variables::StackClass,
                     Variables::ListClass,
                     Variables::ArrayClass,
                     Variables::MapClass,
                     TypeDescriptorClass,
                     std::shared_ptr<ExpressionClass>,
                     std::shared_ptr<Variables::FunctionDefinitionBaseClass>,
                     std::shared_ptr<Variables::VariableContentClass>> dataType;

#endif

template <class ...P>
class ParameterListBuilder {
public:
    static std::vector<VariableTypeDescriptorClass> BuildParameterTypeList()
    {
        std::vector<VariableTypeDescriptorClass> v{Convertor<P>::MakeDescriptor()...};
        return v;
    }
};

void test ()
{
    std::vector<VariableTypeDescriptorClass> v = ParameterListBuilder<int64_t, float, float>::BuildParameterTypeList();
}

template <class R, class ...P>
std::vector<VariableTypeDescriptorClass> BuildParameterTypeList(R (*f)(P...))
{
    return ParameterListBuilder<P...>::BuildParameterTypeList();
}

int f(double d, int i, double d2);

void test2 ()
{
    std::vector<VariableTypeDescriptorClass> v = BuildParameterTypeList(f);
}

template <class  R, class ...P>
class FunctionOrMethodInterface : public FunctionInterfaceBase{

    R (*FunctionPtr)(P...);
    std::vector<std::string> Parameternames;

public:
    FunctionOrMethodInterface(R (*f)(P...)) : FunctionPtr(f), Parameternames(sizeof...(P), "")
    {
        BuildParameterNames();
        if (Parameternames.size() != sizeof...(P)) {
            throw INTERNAL_ERROR_OBJECT("Number of parametername missmatch");
        }
        BuildParameterDescriptorList();
    }

    FunctionOrMethodInterface(R (*f)(P...), std::vector<std::string> Parameternames_) : FunctionPtr(f), Parameternames(Parameternames_)
    {
        BuildParameterNames();
        if (Parameternames.size() != sizeof...(P)) {
            throw INTERNAL_ERROR_OBJECT("Number of parametername missmatch");
        }
        BuildParameterDescriptorList();
    }

    // Callable interface
public:
    virtual Variables::VariableContentClass Execute(Variables::FunctionDefinitionBaseClass::LocalStorageType &Parameters) override
    {
        if (Parameters.size() != sizeof...(P)) {
            throw INTERNAL_ERROR_OBJECT("Number of parametername missmatch");
        }
        auto I = std::make_index_sequence<sizeof...(P)>();
        return FunctionPtr(std::get<P>(Parameters[I])...);
    }

private:

    void BuildParameterDescriptorList()
    {
        ParameterList.clear();
        std::vector<VariableTypeDescriptorClass> v{Convertor<P>::MakeDescriptor()...};

        for (int i = 0; i < v.size(); i++) {
            ParameterList.push_back({Parameternames.at(i), v.at(i), Variables::VariableContentClass::MakeUndefined()});
        }
        Returntype = Convertor<R>::MakeDescriptor();
    }

    void BuildParameterNames()
    {
        for (int i = Parameternames.size(); i < sizeof...(P); i++) {
            Parameternames.push_back("P_" + std::to_string(i));
        }
    }

};

template <class  R, class ...P>
class FunctionInterface : public FunctionOrMethodInterface<R, P...>{

    R (*FunctionPtr)(P...);
    std::vector<std::string> Parameternames;

public:
    FunctionInterface(R (*f)(P...)) : FunctionOrMethodInterface<R, P...>(f)
    {
    }

    FunctionInterface(R (*f)(P...), std::vector<std::string> Parameternames_) : FunctionOrMethodInterface<R, P...>(f, Parameternames_)
    {
    }

    // Callable interface
public:
    virtual Variables::VariableContentClass Execute(Variables::FunctionDefinitionBaseClass::LocalStorageType &Parameters) override
    {
        if (Parameters.size() != sizeof...(P)) {
            throw INTERNAL_ERROR_OBJECT("Number of parametername missmatch");
        }
        auto I = std::make_index_sequence<sizeof...(P)>();
        return FunctionPtr(std::get<P>(Parameters[I])...);
    }

};

template <class  R, class C, class ...P>
class MethodInterface : public FunctionOrMethodInterface<R, P...>{

    R (C::*FunctionPtr)(P...);
    std::vector<std::string> Parameternames;
    C *Object;

public:
    MethodInterface(C *Obj, R (C::*f)(P...)) : FunctionOrMethodInterface<R, P...>(f), Object(Obj)
    {
    }

    MethodInterface(C *Obj, R (C::*f)(P...), std::vector<std::string> Parameternames_) : FunctionOrMethodInterface<R, P...>(f, Parameternames_), Object(Obj)
    {
    }

    // Callable interface
public:
    virtual Variables::VariableContentClass Execute(Variables::FunctionDefinitionBaseClass::LocalStorageType &Parameters) override
    {
        if (Parameters.size() != sizeof...(P)) {
            throw INTERNAL_ERROR_OBJECT("Number of parametername missmatch");
        }
        auto I = std::make_index_sequence<sizeof...(P)>();
        return Object->*FunctionPtr(std::get<P>(Parameters[I])...);
    }

};
#if 0
template <class ...First>
class ParameterListBuilder2 {
    void BuildParameterTypeList(std::vector<VariableTypeDescriptorClass> &v)
    {
        ParameterListBuilder<P...> B;
        Convertor<First> C;
        v.push_back(C.MakeDescriptor());
        B.BuildParameterTypeList(v);
    }
};

template <class First, class ...P>
class ParameterListBuilder2<First, P...> {
    void BuildParameterTypeList(std::vector<VariableTypeDescriptorClass> &v)
    {
        ParameterListBuilder<P...> B;
        Convertor<First> C;
        v.push_back(C.MakeDescriptor());
        B.BuildParameterTypeList(v);
    }
};
#endif

#endif // PREDEFINEDFUNCTIONHELPER_HPP
