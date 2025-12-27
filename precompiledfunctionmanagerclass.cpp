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


#if 0

Anonymeousfunctiondefinition:
                               "function" {
    drv.Currentfunction.BeginFunctionDefinition(@1);
    FunctionsDefinitonsPending.push_back({});
    FunctionDefinitionInfoType &CurrentFunctionInfo = this->FunctionsDefinitonsPending.back();
    CurrentFunctionInfo.VariableHoldingCurrentFunction = Variables.GetVariableReference(Name);
    if (CurrentFunctionInfo.VariableHoldingCurrentFunction != nullptr) {
        throw(yy::parser::syntax_error(l, "function allready defined"));
    }
    CurrentFunctionInfo.VariableHoldingCurrentFunction = Variables.CreateVariable(Name, VariableTypeDescriptorClass(TypeDescriptorClass::Type::Function), 0.0);
    CurrentFunctionInfo.CurrentFunction = std::make_shared<Variables::FunctionDefinitionClass>(Name, l);
    CurrentFunctionInfo.VariableHoldingCurrentFunction->SetValue(Variables::VariableContentClass(CurrentFunctionInfo.CurrentFunction));
    CurrentFunctionInfo.Name = Name;
    //auto &i = typeid(CurrentFunction);
    return CurrentFunctionInfo.CurrentFunction;

}
functionBodydefinition
{
    $$ = $3;
}
;


functiondefinition:
                     "function" "identifier"
{
    drv.Currentfunction.BeginFunctionDefinition($2, @2);
}
functionBodydefinition
{
    $$ = $4;
}
;

functionBodydefinition:
{
    /*FktDefContainer tmp;*/
    auto ptr = drv.Currentfunction.GetReference();
    /*$<FktDefContainer>$ = tmp;*/
    drv.Variables.StartLocal(ptr);
    drv.Variables.CreateNewContext(drv.Currentfunction.GetName()+"Params");
}
returntype.opt
    "(" argumentlist ")"    {
    drv.Variables.CreateNewContext(drv.Currentfunction.GetName());
    //auto ReturnValue = drv.Variables.CreateVariable($<std::string>0, *$2, 0.0);
    drv.Currentfunction.SetReturnType(std::move($2));
    drv.Currentfunction.Set($4, @4);
}
statements
    "endfunction" {
    /**$<FktDefContainer>3 = Variables::FunctionDefinitionClass($5, $8);*/
    /*$$ = $<FktDefContainer>3.ptr;*/
    auto StorageTemplate = drv.Variables.EndLocal();
    //drv.Currentfunction.Define($4, $7, std::move(StorageTemplate), @1+@5);
    drv.Currentfunction.Set($7, @7);
    drv.Currentfunction.Set(std::move(StorageTemplate), @1+@5);
    $$ = drv.Currentfunction.Get(@7); drv.Variables.LeaveContext(2);
}
| error "endfunction" {$$ = std::make_shared<Variables::FunctionDefinitionClass>(Variables::FunctionDefinitionClass::MakeEmpty());}
;

returntype.opt:
                 %empty                       {$$ = std::make_unique<VariableTypeDescriptorClass>(TypeDescriptorClass::Type::Undefined);}
| "returning" typedefinition {$$ = std::move($2);}

argumentlist:
               "identifier"           {$$ = std::vector<std::shared_ptr<VariableClass>>(); auto var = drv.Variables.CreateVariable($1, VariableTypeDescriptorClass(TypeDescriptorClass::Type::Dynamic), 0.0); $$.push_back(var);}
| argumentlist "," "identifier" {auto var = drv.Variables.CreateVariable($3, VariableTypeDescriptorClass(TypeDescriptorClass::Type::Dynamic), 0.0); $1.push_back(var); $$ = $1; };
#endif
