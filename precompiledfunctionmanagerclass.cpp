#include "precompiledfunctionmanagerclass.h"
#include "variablecontentclass.h"
#include "varmanag.hpp"
#include <string>

PrecompiledFunctionManagerClass::PrecompiledFunctionManagerClass(VariableManager &Variables_)
    :Variables(Variables_)
{

}

void PrecompiledFunctionManagerClass::RegisterFunction(std::string Name, FunctionsDescriptor &Fkt)
{
    struct FunctionDefinitionInfoType {
        std::shared_ptr<Variables::PredefinedFunctionDefinitionClass> CurrentFunction;
        std::string Name;
        //std::shared_ptr<VariableClass> ReturnVariable;
        std::shared_ptr<VariableClass> VariableHoldingCurrentFunction;
        //Variables::VariableContentClass ReturnedValue;
        std::unique_ptr<VariableTypeDescriptorClass> ReturnType;
        int NextPositionalParameter;
    } CurrentFunctionInfo;

    std::cout << "%%1%";
    CurrentFunctionInfo.VariableHoldingCurrentFunction = Variables.GetVariableReference(Name);
    if (CurrentFunctionInfo.VariableHoldingCurrentFunction != nullptr) {
        throw(INTERNAL_ERROR_OBJECT("function allready defined"));
    }
    std::cout << "%%2%";
    CurrentFunctionInfo.VariableHoldingCurrentFunction = Variables.CreateVariable(Name, VariableTypeDescriptorClass(TypeDescriptorClass::Type::Function), 0.0);
    CurrentFunctionInfo.CurrentFunction = std::make_shared<Variables::PredefinedFunctionDefinitionClass>(Name, &(Fkt.Function), LocationType());

    CurrentFunctionInfo.VariableHoldingCurrentFunction->SetValue(Variables::VariableContentClass(CurrentFunctionInfo.CurrentFunction));
    CurrentFunctionInfo.Name = Name;
    Variables.StartLocal(CurrentFunctionInfo.CurrentFunction);
    Variables.CreateNewContext(Name + "Params");
    std::vector<std::shared_ptr<VariableClass>> Params;
    for (auto &p: Fkt.Parameterlist) {
       auto var = Variables.CreateVariable(p.Name, VariableTypeDescriptorClass(p.TypeDescriptor), 0.0);
       Params.push_back(var);
    }
    CurrentFunctionInfo.CurrentFunction->SetReturnType(std::make_unique<VariableTypeDescriptorClass>(Fkt.Returntype));
    CurrentFunctionInfo.CurrentFunction->Set(Params, LocationType());
    auto StorageTemplate = Variables.EndLocal();
    CurrentFunctionInfo.CurrentFunction->Set(StorageTemplate, LocationType());
    Variables.LeaveContext(1);

}

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
class FunctionInterface : public Variables::Callable{

    R (*FunctionPtr)(P...);
    std::vector<std::string> Parameternames;

    struct FunctionsParameterDescriptor {
        std::string Name;
        VariableTypeDescriptorClass TypeDescriptor;
        Variables::VariableContentClass DefaultValue;
    };

    typedef std::vector<FunctionsParameterDescriptor> ParameterListType;
    VariableTypeDescriptorClass Returntype;

public:
    FunctionInterface(R (*f)(P...)) : FunctionPtr(f), Parameternames(sizeof...(P), "")
    {
        BuildParameterNames();
        if (Parameternames.size() != sizeof...(P)) {
            throw INTERNAL_ERROR_OBJECT("Number of parametername missmatch");
        }
    }

    FunctionInterface(R (*f)(P...), std::vector<std::string> Parameternames_) : FunctionPtr(f), Parameternames(Parameternames_)
    {
        BuildParameterNames();
        if (Parameternames.size() != sizeof...(P)) {
            throw INTERNAL_ERROR_OBJECT("Number of parametername missmatch");
        }
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

    ParameterListType GetParameterDescriptorList()
    {
        std::vector<VariableTypeDescriptorClass> v{Convertor<P>::MakeDescriptor()...};
        ParameterListType DescriptorList;

        for (int i = 0; i < v.size(); i++) {
            DescriptorList.push_back({Parameternames.at(i), v.at(i), Variables::VariableContentClass::MakeUndefined()});
        }
        return DescriptorList;

    }

    VariableTypeDescriptorClass GetReturnType()
    {
       return Convertor<R>::MakeDescriptor();
    }

private:
    void BuildParameterNames()
    {
        for (int i = Parameternames.size(); i < sizeof...(P); i++) {
            Parameternames.push_back("P_" + std::to_string(i));
        }
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
