#ifndef DRIVER_HH
# define DRIVER_HH
# include <string>
# include <map>
#include <list>
#include "environment.hpp"
# include "parser.hpp"
#include"varmanag.hpp"

// Tell Flex the lexer's prototype ...
# define YY_DECL \
  yy::parser::symbol_type yylex (driver& drv)
// ... and declare it for the parser's sake.
YY_DECL;

class ReferementClass;
class AssignementClass;

class FunctionNodeHelper {
    VariableManager &Variables;
    std::shared_ptr<Variables::FunctionDefinitionClass> CurrentFunction;

public:
    explicit FunctionNodeHelper(VariableManager &Variables)
        : Variables(Variables) , CurrentFunction(nullptr) {}
    std::shared_ptr<Variables::FunctionDefinitionClass> Set(std::string Name, const yy::parser::location_type &l);
    std::shared_ptr<Variables::FunctionDefinitionClass> Create(std::string Name, const yy::parser::location_type &l);
    std::shared_ptr<Variables::FunctionDefinitionClass> Define(const Variables::FunctionDefinitionClass &f, const yy::parser::location_type &l);
    std::shared_ptr<Variables::FunctionDefinitionClass> Get(yy::parser::location_type &l);

    std::shared_ptr<ReferementClass> MakeRef(const std::string Referer, std::shared_ptr<ExpressionClass> Refered);
    std::shared_ptr<AssignementClass> MakeAssign(const std::string Assignee, std::shared_ptr<ExpressionClass>  Assigned);

};

// Conducting the whole scanning and parsing of Calc++.
class driver
{
  Environment Env;

public:
  driver ();

  // std::map<std::string, int> variables;
  VariableManager Variables;
  FunctionNodeHelper Currentfunction;

   std::shared_ptr<ExpressionClass> resulte;
  std::list<std::shared_ptr<StatementClass>> result;

  // Run the parser on file F.  Return 0 on success.
  int parse (const std::string& f);
  // The name of the file being parsed.
  std::string file;
  // Whether to generate parser debug traces.
  bool trace_parsing;

  // Handling the scanner.
  void scan_begin ();
  void scan_end ();
  // Whether to generate scanner debug traces.
  bool trace_scanning;
  // The token's location used by the scanner.
  yy::location location;

  void halt();
  void Run();
  void Run(std::string id);
  void execute(std::shared_ptr<StatementClass> s);
  void compile(std::string id);
  void Print(std::string id);
  void Dump();
  void SetParserDebugLevel(int Level);
};
#endif // ! DRIVER_HH
