
#include "functionnodehelper.h"
#include "varmanag.hpp"
#include "parser.hpp"
#include "compact.h"


std::shared_ptr<Variables::FunctionDefinitionBaseClass> FunctionNodeHelper::BeginFunctionCall(std::string Name, const yy::parser::location_type &l)
{
    FunctionCallsPending.push_back({});
    FunctionCallsPending.back().NextPositionalParameter = -1;
    std::shared_ptr<VariableClass> VariableHoldingFunction =
        Variables.GetVariableReference(Name);
    if (VariableHoldingFunction == nullptr) {
        throw(yy::parser::syntax_error(l, "Function: Symbol not found"));
    }
    FunctionCallsPending.back().CurrentFunction =
        VariableHoldingFunction->GetInitialValue().GetValue<std::shared_ptr<Variables::FunctionDefinitionBaseClass>>();
    if (FunctionCallsPending.back().CurrentFunction == nullptr) {throw INTERNAL_ERROR_OBJECT("Not a function object");}
    return FunctionCallsPending.back().CurrentFunction;
}

void FunctionNodeHelper::EndFunctionCall(const yy::parser::location_type &l)
{
    (void) l;
    if (FunctionCallsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<EndFunctionCall()> Not inside function"));
    }
    FunctionCallsPending.pop_back();
}

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::BeginFunctionDefinition(std::string Name, const yy::parser::location_type &l)
{
    FunctionsDefinitonsPending.push_back({});
    FunctionDefinitionInfoType &CurrentFunctionInfo = this->FunctionsDefinitonsPending.back();
    CurrentFunctionInfo.VariableHoldingCurrentFunction = Variables.GetVariableReference(Name);
    if (CurrentFunctionInfo.VariableHoldingCurrentFunction != nullptr) {
        throw(yy::parser::syntax_error(l, "function allready defined"));
    }
    CurrentFunctionInfo.VariableHoldingCurrentFunction = Variables.CreateVariable(Name, VariableTypeDescriptorClass(TypeDescriptorClass::Type::Function), 0.0);
    CurrentFunctionInfo.CurrentFunction = std::make_shared<Variables::FunctionDefinitionClass>(Name, l);
    CurrentFunctionInfo.VariableHoldingCurrentFunction->SetInitialValue(Variables::VariableContentClass(CurrentFunctionInfo.CurrentFunction));
    CurrentFunctionInfo.Name = Name;
    //auto &i = typeid(CurrentFunction);
    return CurrentFunctionInfo.CurrentFunction;
}

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::BeginFunctionDefinition(const yy::parser::location_type &l)
{
    
    return BeginFunctionDefinition("_Anonym_" + std::to_string(AnonymeousElementCounter++), l);
}

void FunctionNodeHelper::EndFunctionDefinition(const yy::parser::location_type &l)
{
    (void) l;
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<EndFunctionDefinition()> Not inside function"));
    }
    FunctionsDefinitonsPending.pop_back();
}

std::string FunctionNodeHelper::GetQualifiedName()
{
    std::string Name;
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<GetQualifiedName()> Not inside function"));
    }
    for (auto const &f: FunctionsDefinitonsPending) {
        Name.append("::");
        Name.append(f.Name);
    }
    return Name;
}

void FunctionNodeHelper::Set(const std::vector<std::shared_ptr<VariableClass> > &Parameters, LocationType const &Loc)
{
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<Set()> Not inside function"));
    }
    FunctionsDefinitonsPending.back().CurrentFunction->Set(Parameters, Loc);
}

void FunctionNodeHelper::Set(const std::list<std::shared_ptr<StatementClass> > &Statements, LocationType const &Loc)
{
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<Set()> Not inside function"));
    }
    // Env.DebugOutput() << "Settting " << Statements.size() << "statements for " << FunctionsDefinitonsPending.back().Name << "\n";
    FunctionsDefinitonsPending.back().CurrentFunction->Set(Statements, Loc);
    KnownFunctions[GetQualifiedName()] = Statements;
}

void FunctionNodeHelper::Set(Variables::FunctionDefinitionClass::LocalStorageType StorageTemplate, LocationType const &Loc)
{
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<Set()> Not inside function"));
    }
    FunctionsDefinitonsPending.back().CurrentFunction->Set(StorageTemplate, Loc);
}

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::GetReference()
{
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<GetReference()> Not inside function"));
    }
    return FunctionsDefinitonsPending.back().CurrentFunction;
}

std::string FunctionNodeHelper::GetName()
{
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<GetReference()> Not inside function"));
    }
    return FunctionsDefinitonsPending.back().Name;
}

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::Get(yy::parser::location_type &l)
{
    (void) l;
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<Get()> Not inside function"));
    }
    FunctionsDefinitonsPending.back().CurrentFunction->SetReturnType(std::move(FunctionsDefinitonsPending.back().ReturnType));
    return FunctionsDefinitonsPending.back().CurrentFunction;
}

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::SetReturnType(std::unique_ptr<VariableTypeDescriptorClass> NewReturnType)
{
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<SetReturnVariable()> Not inside function"));
    }
    //FunctionsDefinitonsPending.back().ReturnVariable = NewReturnVariable;
    FunctionsDefinitonsPending.back().ReturnType = std::move(NewReturnType);
    return FunctionsDefinitonsPending.back().CurrentFunction;
}

std::shared_ptr<ReferementClass> FunctionNodeHelper::MakeRef(const std::string Referer, std::shared_ptr<ExpressionClass> Refered, const LocationType &Loc)
{
    (void) Loc;
    (void) Refered;
    (void) Referer;
    return nullptr;
}

std::shared_ptr<ReferementClass> FunctionNodeHelper::MakeRefBySequence(std::shared_ptr<ExpressionClass> Assigned, const LocationType &Loc)
{
    (void) Loc;
    (void) Assigned;
    return nullptr;
}

std::shared_ptr<AssignementClass> FunctionNodeHelper::MakeAssign(const std::string Assignee, std::shared_ptr<ExpressionClass> Assigned, LocationType const &Loc)
{
    if (FunctionCallsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<MakeAssign()> Not inside functioncall"));
    }
    
    if ((FunctionCallsPending.back().NextPositionalParameter >= 0) && (FunctionCallsPending.back().NextPositionalParameter < 0xFFFF)) {
        throw INTERNAL_ERROR_OBJECT ("mixing positional and named parameter not allowed");
    }
    // set marker for positionalmode
    FunctionCallsPending.back().NextPositionalParameter = 0xFFFF;
    std::shared_ptr<VariableClass> Var = FunctionCallsPending.back().CurrentFunction->GetParameterByName(Assignee);
    if (Var == nullptr) {
        throw INTERNAL_ERROR_OBJECT ("Parameter not found");
    }
    auto ToAssign = std::make_shared<VariableValueClass>(Var, Loc);
    return std::make_shared<AssignementClass>(Assigned, ToAssign, Loc);
}

std::shared_ptr<AssignementClass> FunctionNodeHelper::MakeAssignBySequence(std::shared_ptr<ExpressionClass> Assigned, const LocationType &Loc)
{
    if (FunctionCallsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<MakeAssign()> Not inside functioncall"));
    }
    if (FunctionCallsPending.back().NextPositionalParameter >= 0xFFFF) {
        throw INTERNAL_ERROR_OBJECT ("mixing positional and named parameter not allowed");
    }
    if (FunctionCallsPending.back().NextPositionalParameter < 0) {
        FunctionCallsPending.back().NextPositionalParameter = 0;
    }
    // set marker for positionalmode
    std::shared_ptr<VariableClass> Var = FunctionCallsPending.back().CurrentFunction->GetParameterByIndex(FunctionCallsPending.back().NextPositionalParameter);
    if (Var == nullptr) {
        throw INTERNAL_ERROR_OBJECT ("Parameter [" + std::to_string(FunctionCallsPending.back().NextPositionalParameter) + "] not found");
    }
    FunctionCallsPending.back().NextPositionalParameter++;
    auto ToAssign = std::make_shared<VariableValueClass>(Var, Loc);
    return std::make_shared<AssignementClass>(Assigned, ToAssign, Loc);
}
