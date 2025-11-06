#ifndef DRIVER_HH
# define DRIVER_HH
# include <string>
# include <map>
#include <list>
# include "parser.hpp"
#include"varmanag.hpp"

// Tell Flex the lexer's prototype ...
# define YY_DECL \
  yy::parser::symbol_type yylex (driver& drv)
// ... and declare it for the parser's sake.
YY_DECL;

class FunctionNodeHelper {
    VariableManager &Variables;

public:
    explicit FunctionNodeHelper(VariableManager &Variables)
        : Variables(Variables) {}
    void Set(std::string);

    void MakeRef(VariableClass &Referer, VariableClass Refered);
    void MakeAssign(VariableClass &Assignee, VariableClass Assigned);

};

// Conducting the whole scanning and parsing of Calc++.
class driver
{
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
};
#endif // ! DRIVER_HH
