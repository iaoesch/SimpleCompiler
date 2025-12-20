#include "driver.hh"
#include "parser.hpp"
#include "compact.h"
#include <fstream>

driver::driver (Environment &Env_)
    : Env(Env_), Currentfunction(Variables), trace_parsing (false), trace_scanning (false)
{
//  variables["one"] = 1;
//  variables["two"] = 2;
}

int
driver::parse (const std::string &f)
{
  file = f;
  location.initialize (&file);
  scan_begin ();
  yy::parser parser (*this);
  parser.set_debug_level (trace_parsing);
  Variables.CreateNewContext("$$Global_Context$$");
  int res = 0;
  res = parser.parse ();
  scan_end ();
  return res;
}

int driver::parse(const char *Code)
{
    Errors.clear();
    std::string DummyFileName;
    location.initialize (&DummyFileName);
    scan_begin (Code);
    yy::parser parser (*this);
    parser.set_debug_level (trace_parsing);
    Variables.clear();
    Variables.CreateNewContext("$$Global_Context$$");
    int res = 0;
    res = parser.parse ();
    scan_end ();
    return res;
}

void driver::halt()
{
    std::cout << "Error happened" << std::endl;
}

void driver::Run()
{
    Env.ExecutionStarted();
    try {
        for (auto const &s: result) {
            s->Execute(Env);
        }
    }
    catch (...) {
        Env.ExecutionStopped();
        throw;
    }
    Env.ExecutionStopped();
}

void driver::Run(std::string id)
{
    (void)id;
}

void driver::execute(std::shared_ptr<StatementClass> s)
{
    LastStatement = s;
    std::cout << ">>>";
    s->Print(std::cout);
    std::cout << "\n>exe>";
    Env.ExecutionStarted();
    try {
       s->Execute(Env);
    }
    catch (...) {
       Env.ExecutionStopped();
       throw;
    }
    Env.ExecutionStopped();

}

void driver::AddStatement(std::shared_ptr<StatementClass> s)
{
    result.push_back(s);
}

void driver::compile(std::string id)
{
    (void)id;

}

void driver::Print(std::string id)
{
    auto Var = Variables.GetVariableReference(id);
    if (Var == nullptr) {
        std::cout << "Error, variabe <" << id << "> does not exist\n";
    } else {
        std::cout << id << " = ";
        Var->Print(std::cout);
        std::cout << std::endl;
    }
}

void driver::Dump()
{
    Variables.Dump(std::cout);
}

void driver::SetParserDebugLevel(int Level)
{
    (void)Level;
 //  parser.set_debug_level (trace_parsing);
}

void driver::Tree(std::string FilePath)
{
    (void)FilePath;
    if (LastStatement != nullptr) {
        std::ofstream Drawing("DrawDot.dot");
        Drawing << "digraph g {" << std::endl;
        Drawing << "node [shape = record,height=.1];" << std::endl;
        LastStatement->DrawNode(Drawing, 0);
        Drawing << "}" << std::endl;
        Drawing.close();

        system("/opt/homebrew/bin/dot -Tpng DrawDot.dot -o tree2.png");
        system("open tree2.png");

    }

}

void driver::ReportError(const yy::location &l, const std::string &m)
{
    Errors.push_back({l, m});
}

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::BeginFunctionCall(std::string Name, const yy::parser::location_type &l)
{
    FunctionCallsPending.push_back({});
    FunctionCallsPending.back().NextPositionalParameter = -1;
    std::shared_ptr<VariableClass> VariableHoldingFunction =
        Variables.GetVariableReference(Name);
    if (VariableHoldingFunction == nullptr) {
        throw(yy::parser::syntax_error(l, "Function: Symbol not found"));
    }
    FunctionCallsPending.back().CurrentFunction =
        VariableHoldingFunction->GetValue().GetValue<std::shared_ptr<Variables::FunctionDefinitionClass>>();
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
    CurrentFunctionInfo.CurrentFunction = std::make_shared<Variables::FunctionDefinitionClass>(Variables::FunctionDefinitionClass::MakeEmpty());
    CurrentFunctionInfo.VariableHoldingCurrentFunction->SetValue(Variables::VariableContentClass(CurrentFunctionInfo.CurrentFunction));
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

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::Define(Variables::FunctionDefinitionClass &&f, const yy::parser::location_type &l)
{
    (void) l;
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<Define()> Not inside function"));
    }
    *(FunctionsDefinitonsPending.back().CurrentFunction) = std::move(f);
    return FunctionsDefinitonsPending.back().CurrentFunction;
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
    // set marker for positionalmode
    std::shared_ptr<VariableClass> Var = FunctionCallsPending.back().CurrentFunction->GetParameterByIndex(FunctionCallsPending.back().NextPositionalParameter);
    if (Var == nullptr) {
        throw INTERNAL_ERROR_OBJECT ("Parameter not found");
    }
    FunctionCallsPending.back().NextPositionalParameter++;
    auto ToAssign = std::make_shared<VariableValueClass>(Var, Loc);
    return std::make_shared<AssignementClass>(Assigned, ToAssign, Loc);
}

