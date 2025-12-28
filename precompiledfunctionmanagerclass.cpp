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

template <class First>
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
class ParameterListBuilder2 {
    void BuildParameterTypeList(std::vector<VariableTypeDescriptorClass> &v)
    {
        ParameterListBuilder<P...> B;
        Convertor<First> C;
        v.push_back(C.MakeDescriptor());
        B.BuildParameterTypeList(v);
    }
};

