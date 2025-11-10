#include "driver.hh"
#include "parser.hpp"
#include "compact.h"

driver::driver ()
    : Currentfunction(Variables), trace_parsing (false), trace_scanning (false)
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

void driver::halt()
{
    std::cout << "Error happened" << std::endl;
}

void driver::Run()
{

}

void driver::Run(std::string id)
{

}

void driver::execute(std::shared_ptr<StatementClass> s)
{
    std::cout << ">>>";
    s->Print(std::cout);
}

void driver::compile(std::string id)
{

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

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::Set(std::string Name, const yy::parser::location_type &l)
{
    std::shared_ptr<VariableClass> Var = Variables.GetVariableReference(Name);
    if (Var == nullptr) {
        throw(yy::parser::syntax_error(l, "Function: Symbol not found"));
    }
    CurrentFunction = Var->GetValue().GetValue<std::shared_ptr<Variables::FunctionDefinitionClass>>();
    if (Var == nullptr) {ERROR_OBJECT("Not a function object");}
    FunctionDefinitionClassSharedPtr p = CurrentFunction;
    return CurrentFunction;
}

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::Create(std::string Name, const yy::parser::location_type &l)
{
    std::shared_ptr<VariableClass> Var = Variables.GetVariableReference(Name);
    if (Var != nullptr) {
        throw(yy::parser::syntax_error(l, "function alllready defined"));
    }
    Var = Variables.CreateVariable(Name, TypeDescriptorClass(TypeDescriptorClass::Type::Function), 0.0);
    CurrentFunction = std::make_shared<Variables::FunctionDefinitionClass>(Variables::FunctionDefinitionClass::MakeEmpty());
    Var->SetValue(Variables::VariableContentClass(CurrentFunction));
    auto &i = typeid(CurrentFunction);
    return CurrentFunction;
}

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::Define(const Variables::FunctionDefinitionClass &f, const yy::parser::location_type &l)
{
    *CurrentFunction = f;
    return CurrentFunction;
}

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::Get(yy::parser::location_type &l)
{
    return CurrentFunction;
}

std::shared_ptr<ReferementClass> FunctionNodeHelper::MakeRef(const std::string Referer, std::shared_ptr<ExpressionClass> Refered)
{

}

std::shared_ptr<AssignementClass> FunctionNodeHelper::MakeAssign(const std::string Assignee, std::shared_ptr<ExpressionClass> Assigned)
{

}
