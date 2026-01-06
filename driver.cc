#include "driver.hh"
#include "parser.hpp"
#include "compact.h"
#include <fstream>
#include "systeminterfaceclass.h"

driver::driver (Environment &Env_, SystemInterfaceClass *SystemInterface_)
    : Env(Env_), Currentfunction(Variables), PrecompiledManager(Variables), SystemInterface(SystemInterface_), trace_parsing (false), trace_scanning (false)
{
//  variables["one"] = 1;
//  variables["two"] = 2;
   SetNodeDefaultEnvironment(Env);
   VariableClass::SetDefaultEnvironment(Env);
   VariableManager::SetDefaultEnvironment(Env);
}

class TestCaller : public Variables::Callable {


    // Callable interface
public:
    virtual Variables::VariableContentClass Execute(Variables::FunctionDefinitionBaseClass::LocalStorageType &Parameters) override;
};

Variables::VariableContentClass TestCaller::Execute(Variables::FunctionDefinitionBaseClass::LocalStorageType &Parameters)
{
    return Variables::VariableContentClass(6LL);
}

void driver::SetupPredefinedFunctions()
{
    static TestCaller t;

    SystemInterface->Register(PrecompiledManager);
  //  PrecompiledFunctionManagerClass::FunctionsDescriptor Fkt(t, VariableTypeDescriptorClass(VariableTypeDescriptorClass::Type::Integer));
  //  PrecompiledManager.RegisterFunction("TTT", Fkt);
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
  SetupPredefinedFunctions();
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
    SetupPredefinedFunctions();
    int res = 0;
    res = parser.parse ();
    scan_end ();
    return res;
}

void driver::halt()
{
    Env.DebugOutput() << "Error happened" << std::endl;
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
    Env.DebugOutput() << ">>>";
    s->Print(Env.DebugOutput());
    Env.DebugOutput() << "\n>exe>";
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
        Env.DebugOutput() << "Error, variabe <" << id << "> does not exist\n";
    } else {
        Env.DebugOutput() << id << " = ";
        Var->Print(Env.DebugOutput());
        Env.DebugOutput() << std::endl;
    }
}

void driver::Dump()
{
    Variables.Dump(Env.DebugOutput());
}

void driver::SetParserDebugLevel(int Level)
{
    trace_parsing = (Level > 0);
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

const std::map<std::string, std::list<std::shared_ptr<StatementClass> > > &driver::GetListOfDefinedFunctions()
{
    return Currentfunction.GetListOfDefinedFunctions();
}

















#if 0
std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::Define(Variables::FunctionDefinitionClass &&f, const yy::parser::location_type &l)
{
    (void) l;
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<Define()> Not inside function"));
    }
    *(FunctionsDefinitonsPending.back().CurrentFunction) = std::move(f);
    return FunctionsDefinitonsPending.back().CurrentFunction;
}

std::shared_ptr<Variables::FunctionDefinitionClass> FunctionNodeHelper::Define(const std::vector<std::shared_ptr<VariableClass> > &Parameters, const std::list<std::shared_ptr<StatementClass> > &Statements, Variables::FunctionDefinitionClass::LocalStorageType StorageTemplate, LocationType const &Loc)
{
    if (FunctionsDefinitonsPending.empty()) {
        throw(INTERNAL_ERROR_OBJECT("<Define()> Not inside function"));
    }
    *(FunctionsDefinitonsPending.back().CurrentFunction) = Variables::FunctionDefinitionClass(
        FunctionsDefinitonsPending.back().Name,
        Parameters,
        Statements,
        StorageTemplate,
        Loc
        );
    return FunctionsDefinitonsPending.back().CurrentFunction;

}
#endif


























