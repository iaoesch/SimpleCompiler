
#include "functionnodehelper.h"
#include "typedescriptorclass.hpp"
#include "varmanag.hpp"
#include "parser.hpp"
#include "compact.h"



std::shared_ptr<Variables::FunctionDefinitionBaseClass> FunctionNodeHelper::BeginFunctionCall(std::string Name, const yy::parser::location_type &l)
{
    FunctionCallsPending.push_back(FunctionCallInfoType{});
    std::get<FunctionCallInfoType>(FunctionCallsPending.back()).NextPositionalParameter = -1;
    std::shared_ptr<VariableClass> VariableHoldingFunction =
        Variables.GetVariableReference(Name);
    if (VariableHoldingFunction == nullptr) {
        throw(yy::parser::syntax_error(l, "Function: Symbol not found"));
    }
    std::get<FunctionCallInfoType>(FunctionCallsPending.back()).CurrentFunction =
        VariableHoldingFunction->GetInitialValue().GetValue<std::shared_ptr<Variables::FunctionDefinitionBaseClass>>();
    if (std::get<FunctionCallInfoType>(FunctionCallsPending.back()).CurrentFunction == nullptr) {throw INTERNAL_ERROR_OBJECT("Not a function object");}
    return std::get<FunctionCallInfoType>(FunctionCallsPending.back()).CurrentFunction;
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
    FunctionsDefinitonsPending.push_back(FunctionDefinitionInfoType__{});
    FunctionDefinitionInfoType__ &CurrentFunctionInfo = std::get<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back());
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
    Variables.LeaveContext(2);
}

std::string FunctionNodeHelper::GetQualifiedName()
{
    std::string Name;
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<GetQualifiedName()> Not inside function"));
    }
    for (auto const &f: FunctionsDefinitonsPending) {
        if (std::holds_alternative<FunctionDefinitionInfoType__>(f)) {
            Name.append("::");
            Name.append(std::get<FunctionDefinitionInfoType__>(f).Name);
        } else if (std::holds_alternative<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back())) {
            Name.append(std::get<MethodDefinitionInfoType>(f).ClassForMethod->GetName());
            Name.append("::");
            Name.append(std::get<MethodDefinitionInfoType>(f).Name);
        } else {
            throw(INTERNAL_ERROR_OBJECT("<GetQualifiedName()> No valid Infotype"));
        }

    }
    return Name;
}

void FunctionNodeHelper::Set(const std::vector<std::shared_ptr<VariableClass> > &Parameters, LocationType const &Loc)
{
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<Set()> Not inside function"));
    }
    if (std::holds_alternative<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back())) {
        std::get<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back()).CurrentFunction->Set(Parameters, Loc);
    } else if (std::holds_alternative<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back())) {
        std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).CurrentMethod->Set(Parameters, Loc);
    } else {
        throw(INTERNAL_ERROR_OBJECT("<Set()> No valid Infotype"));
    }
}

void FunctionNodeHelper::Set(const std::list<std::shared_ptr<StatementClass> > &Statements, LocationType const &Loc)
{
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<Set()> Not inside function"));
    }
    // Env.DebugOutput() << "Settting " << Statements.size() << "statements for " << FunctionsDefinitonsPending.back().Name << "\n";
    if (std::holds_alternative<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back())) {
        std::get<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back()).CurrentFunction->Set(Statements, Loc);
    } else if (std::holds_alternative<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back())) {
        std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).CurrentMethod->Set(Statements, Loc);
    } else {
        throw(INTERNAL_ERROR_OBJECT("<Set()> No valid Infotype"));
    }
    KnownFunctions[GetQualifiedName()] = Statements;
}

void FunctionNodeHelper::Set(Variables::FunctionDefinitionClass::LocalStorageType StorageTemplate, LocationType const &Loc)
{
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<Set()> Not inside function"));
    }
    if (std::holds_alternative<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back())) {
        std::get<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back()).CurrentFunction->Set(StorageTemplate, Loc);
    } else if (std::holds_alternative<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back())) {
        std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).CurrentMethod->Set(StorageTemplate, Loc);
    } else {
        throw(INTERNAL_ERROR_OBJECT("<Set()> No valid Infotype"));
    }
}

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::GetReference()
{
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<GetReference()> Not inside function"));
    }
    if (std::holds_alternative<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back())) {
        return std::get<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back()).CurrentFunction;
    } else {
        throw(INTERNAL_ERROR_OBJECT("<GetReference()> No valid Infotype"));
    }
}

std::string FunctionNodeHelper::GetName()
{
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<GetReference()> Not inside function"));
    }
    if (std::holds_alternative<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back())) {
        return std::get<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back()).Name;
    } else if (std::holds_alternative<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back())) {
        return std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).Name;
    } else {
        throw(INTERNAL_ERROR_OBJECT("<Set()> No valid Infotype"));
    }
}

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::Get(yy::parser::location_type &l)
{
    (void) l;
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<Get()> Not inside function"));
    }
    if (std::holds_alternative<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back())) {
        std::get<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back()).CurrentFunction->SetReturnType(std::move(std::get<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back()).ReturnType));
        return std::get<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back()).CurrentFunction;
  //  } else if (std::holds_alternative<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back())) {
  //      std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).CurrentMethod->SetReturnType(std::move(std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).ReturnType));
  //      return std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).CurrentMethod;
    } else {
        throw(INTERNAL_ERROR_OBJECT("<Set()> No valid Infotype"));
    }
}

void FunctionNodeHelper::SetReturnType(std::unique_ptr<VariableTypeDescriptorClass> NewReturnType)
{
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<SetReturnVariable()> Not inside function"));
    }
    if (std::holds_alternative<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back())) {
        std::get<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back()).ReturnType = std::move(NewReturnType);
        //  return std::get<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back()).CurrentFunction;
    } else if (std::holds_alternative<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back())) {
        std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).ReturnType = std::move(NewReturnType);
    //    return std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).CurrentMethod;
    } else {
        throw(INTERNAL_ERROR_OBJECT("<Set()> No valid Infotype"));
    }
}

void FunctionNodeHelper::BeginMethodDefinition(std::string Name, const LocationType &l)
{
    FunctionsDefinitonsPending.push_back(MethodDefinitionInfoType{});
    MethodDefinitionInfoType &CurrentFunctionInfo = std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back());
    CurrentFunctionInfo.VariableHoldingCurrentFunction = Variables.GetVariableReference(Name);
    if (CurrentFunctionInfo.VariableHoldingCurrentFunction != nullptr) {
        throw(yy::parser::syntax_error(l, "function allready defined"));
    }
  //  CurrentFunctionInfo.VariableHoldingCurrentFunction = Variables.CreateVariable(Name, VariableTypeDescriptorClass(TypeDescriptorClass::Type::Function), 0.0);
    CurrentFunctionInfo.CurrentMethod = std::make_shared<Variables::MethodDefinitionClass>(Name, l);
  //  CurrentFunctionInfo.VariableHoldingCurrentFunction->SetInitialValue(Variables::VariableContentClass(CurrentFunctionInfo.CurrentFunction));
    CurrentFunctionInfo.Name = Name;
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
    
    if ((std::get<FunctionCallInfoType>(FunctionCallsPending.back()).NextPositionalParameter >= 0) && (std::get<FunctionCallInfoType>(FunctionCallsPending.back()).NextPositionalParameter < 0xFFFF)) {
        throw INTERNAL_ERROR_OBJECT ("mixing positional and named parameter not allowed");
    }
    // set marker for positionalmode
    std::get<FunctionCallInfoType>(FunctionCallsPending.back()).NextPositionalParameter = 0xFFFF;
    std::shared_ptr<VariableClass> Var = std::get<FunctionCallInfoType>(FunctionCallsPending.back()).CurrentFunction->GetParameterByName(Assignee);
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
    if (std::get<FunctionCallInfoType>(FunctionCallsPending.back()).NextPositionalParameter >= 0xFFFF) {
        throw INTERNAL_ERROR_OBJECT ("mixing positional and named parameter not allowed");
    }
    if (std::get<FunctionCallInfoType>(FunctionCallsPending.back()).NextPositionalParameter < 0) {
        std::get<FunctionCallInfoType>(FunctionCallsPending.back()).NextPositionalParameter = 0;
    }
    // set marker for positionalmode
    std::shared_ptr<VariableClass> Var = std::get<FunctionCallInfoType>(FunctionCallsPending.back()).CurrentFunction->GetParameterByIndex(std::get<FunctionCallInfoType>(FunctionCallsPending.back()).NextPositionalParameter);
    if (Var == nullptr) {
        throw INTERNAL_ERROR_OBJECT ("Parameter [" + std::to_string(std::get<FunctionCallInfoType>(FunctionCallsPending.back()).NextPositionalParameter) + "] not found");
    }
    std::get<FunctionCallInfoType>(FunctionCallsPending.back()).NextPositionalParameter++;
    auto ToAssign = std::make_shared<VariableValueClass>(Var, Loc);
    return std::make_shared<AssignementClass>(Assigned, ToAssign, Loc);
}

void FunctionNodeHelper::BeginMethodCallForObject(std::string ObjectName, const LocationType &Loc)
{
    FunctionCallsPending.push_back(MethodCallInfoType{});
    std::get<MethodCallInfoType>(FunctionCallsPending.back()).NextPositionalParameter = -1;

    std::shared_ptr<VariableClass> Var = Variables.GetVariableReference(ObjectName);
    if (Var == nullptr) {
        throw SyntaxErrorClass("Object '" + ObjectName + "' not found");
    }
    if (Var->Type() != TypeDescriptorClass::Type::Object) {
        throw SyntaxErrorClass("'" + ObjectName + "' is not an object, cannot send messages to it");
    }
    VariableTypeDescriptorClass Type = Var->Type();
    ObjectDescriptorClass const ObjectDescriptor = Type.GetTypeDetails<ObjectDescriptorClass>();
    std::shared_ptr<const Variables::ClassClass> UsedClass = ObjectDescriptor.GetClass();
    std::get<MethodCallInfoType>(FunctionCallsPending.back()).UsedObject = Var;
    std::get<MethodCallInfoType>(FunctionCallsPending.back()).UsedClass = UsedClass;

#ifdef nonsense //, works only at runtime...
    std::shared_ptr<VariableClass> Var = Variables.GetVariableReference(ObjectName);
    if (Var == nullptr) {
        throw SyntaxErrorClass("Object '" + ObjectName + "' not found");
    }
    if (Var->Type() != TypeDescriptorClass::Type::Object) {
        throw SyntaxErrorClass("'" + ObjectName + "' is not an object, cannot send messages to it");
    }
    if (Var->GetValue().holds_alternative<Variables::ObjectClass>()) {

    } else {
            throw SyntaxErrorClass("'" + ObjectName + "' does not contain an valid object");
    }
#endif
}

void FunctionNodeHelper::SetCalledMethodForObject(std::string MethodName, const LocationType &Loc)
{
    if (FunctionCallsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<SetCalledMethodForObject()> Not inside functioncall"));
    }
    std::shared_ptr<const Variables::ClassClass> UsedClass = std::get<MethodCallInfoType>(FunctionCallsPending.back()).UsedClass;
    std::shared_ptr<Variables::MethodDefinitionClass> Method = UsedClass->GetMethod(MethodName);
    if (Method == nullptr) {
        throw(yy::parser::syntax_error(Loc, "Function: Method not found in Class '" + UsedClass->GetName() + "' (or its parent(s)"));
    }
    std::get<MethodCallInfoType>(FunctionCallsPending.back()).CurrentMethod = Method;

    // make assignement for this (first parameter)
    std::shared_ptr<VariableClass> Var = Method->GetParameterByIndex(0);
    if (Var == nullptr) {
        throw INTERNAL_ERROR_OBJECT ("this pointer not found");
    }
    if (Var->Type() != TypeDescriptorClass::Type::Object) {
        throw INTERNAL_ERROR_OBJECT ("this pointer not found, wrong type for first parameter");
    }
    if (Var->Type().GetTypeDetails<ObjectDescriptorClass>().GetClass() != UsedClass) {
        throw INTERNAL_ERROR_OBJECT ("this pointer for wrong class");
    }
    auto ToAssign = std::make_shared<VariableValueClass>(Var, Loc);
    auto Assignee = std::make_shared<VariableValueClass>(std::get<MethodCallInfoType>(FunctionCallsPending.back()).UsedObject, Loc);
    std::make_shared<AssignementClass>(Assignee, ToAssign, Loc);
    std::get<MethodCallInfoType>(FunctionCallsPending.back()).ThisAssignement = std::make_shared<AssignementClass>(Assignee, ToAssign, Loc);
}

void FunctionNodeHelper::SetParameterAssignListForCalledMethod(std::list<std::shared_ptr<StatementClass> > &&Assignements, const LocationType &Loc)
{
    if (FunctionCallsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<SetCalledMethodForObject()> Not inside functioncall"));
    }
    std::get<MethodCallInfoType>(FunctionCallsPending.back()).Assignements = std::move(Assignements);
    std::get<MethodCallInfoType>(FunctionCallsPending.back()).Assignements.push_back(std::get<MethodCallInfoType>(FunctionCallsPending.back()).ThisAssignement);
}

std::shared_ptr<FunctionCallClass> FunctionNodeHelper::FinishMethodCall(const LocationType &Loc)
{
    if (FunctionCallsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<FinishMethodCall()> Not inside functioncall"));
    }
    //FunctionCallClass(std::shared_ptr<Variables::FunctionDefinitionBaseClass> f, std::list<std::shared_ptr<StatementClass>> a, const LocationType &Loc) : ValueClass(Loc), TheFunction(f), Assignements(a) {}

    FunctionCallClass f(
        std::get<MethodCallInfoType>(FunctionCallsPending.back()).CurrentMethod,
        std::get<MethodCallInfoType>(FunctionCallsPending.back()).Assignements,
        Loc);

    return std::make_shared<FunctionCallClass>(
               std::get<MethodCallInfoType>(FunctionCallsPending.back()).CurrentMethod,
               std::get<MethodCallInfoType>(FunctionCallsPending.back()).Assignements,
               Loc);

}
