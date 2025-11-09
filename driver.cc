#include "driver.hh"
#include "parser.hpp"

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
  int res = 0;
  res = parser.parse ();
  scan_end ();
  return res;
}

void driver::halt()
{
    std::cout << "Error happened" << std::endl;
}

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::Set(std::string Name, const yy::parser::location_type &l)
{
    std::shared_ptr<VariableClass> Var = Variables.GetVariableReference(Name);
    if (Var == nullptr) {
        throw(yy::parser::syntax_error(l, "Symbol not found"));
    }
    CurrentFunction = Var->GetValue().GetValue<std::shared_ptr<Variables::FunctionDefinitionClass>>();
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
    return CurrentFunction;
}

std::shared_ptr<ReferementClass> FunctionNodeHelper::MakeRef(const std::string Referer, std::shared_ptr<ExpressionClass> Refered)
{

}

std::shared_ptr<AssignementClass> FunctionNodeHelper::MakeAssign(const std::string Assignee, std::shared_ptr<ExpressionClass> Assigned)
{

}
