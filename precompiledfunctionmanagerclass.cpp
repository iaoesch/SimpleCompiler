#include "precompiledfunctionmanagerclass.h"
#include "variablecontentclass.h"
#include "varmanag.hpp"
#include <string>

PrecompiledFunctionManagerClass::PrecompiledFunctionManagerClass(VariableManager &Variables_)
    :Variables(Variables_)
{

}

void PrecompiledFunctionManagerClass::RegisterFunction(std::string Name, std::shared_ptr<FunctionInterfaceBase> Fkt)
{
    std::shared_ptr<Variables::PredefinedFunctionDefinitionClass> CurrentFunction;
    std::shared_ptr<VariableClass> VariableHoldingCurrentFunction;

    std::cout << "%%1%";
    VariableHoldingCurrentFunction = Variables.GetVariableReference(Name);
    if (VariableHoldingCurrentFunction != nullptr) {
        throw(INTERNAL_ERROR_OBJECT("function allready defined"));
    }
    std::cout << "%%2%";
    VariableHoldingCurrentFunction = Variables.CreateVariable(Name, VariableTypeDescriptorClass(TypeDescriptorClass::Type::Function), 0.0);
    CurrentFunction = std::make_shared<Variables::PredefinedFunctionDefinitionClass>(Name, Fkt, LocationType());

    VariableHoldingCurrentFunction->SetValue(Variables::VariableContentClass(CurrentFunction));
    Variables.StartLocal(CurrentFunction);
    Variables.CreateNewContext(Name + "Params");
    std::vector<std::shared_ptr<VariableClass>> Params;
    for (auto &p: Fkt->GetParameterDescriptorList()) {
       auto var = Variables.CreateVariable(p.Name, VariableTypeDescriptorClass(p.TypeDescriptor), 0.0);
       Params.push_back(var);
    }
    CurrentFunction->SetReturnType(std::make_unique<VariableTypeDescriptorClass>(Fkt->GetReturnType()));
    CurrentFunction->Set(Params, LocationType());
    auto StorageTemplate = Variables.EndLocal();
    CurrentFunction->Set(StorageTemplate, LocationType());
    Variables.LeaveContext(1);

}
