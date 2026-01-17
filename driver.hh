#ifndef DRIVER_HH
# define DRIVER_HH
# include <string>
# include <map>
#include <list>
#include "classnodehelper.h"
#include "environment.hpp"
#include "functionnodehelper.h"
# include "parser.hpp"
#include "precompiledfunctionmanagerclass.h"
#include"varmanag.hpp"

// Tell Flex the lexer's prototype ...
# define YY_DECL \
  yy::parser::symbol_type yylex (driver& drv)
// ... and declare it for the parser's sake.
YY_DECL;

class ReferementClass;
class AssignementClass;
namespace yy {
class location;
}
typedef yy::location  LocationType;

class SystemInterfaceClass;




// Conducting the whole scanning and parsing of Calc++.
class driver
{
  Environment &Env;

public:
  driver (Environment &Env, SystemInterfaceClass *SystemInterface_);

  // std::map<std::string, int> variables;
  VariableManager Variables;
  FunctionNodeHelper Currentfunction;
  ClassNodeHelper CurrentClass;
  PrecompiledFunctionManagerClass PrecompiledManager;
  SystemInterfaceClass *SystemInterface;

  std::shared_ptr<ExpressionClass> resulte;
  std::list<std::shared_ptr<StatementClass>> result;

   std::shared_ptr<StatementClass> LastStatement;

  struct ErrorInformation{
      yy::location Location;
      std::string  Message;
  };

  typedef std::list<ErrorInformation> ErrorListType;
  ErrorListType Errors;

  private:
  void SetupPredefinedFunctions();

  public:
  // Run the parser on file F.  Return 0 on success.
  int parse (const std::string& f);
  int parse (const char *Code);

  std::ostream &GetOutputStream() {return Env.OutputStream();}
  // The name of the file being parsed.
  std::string file;
  // Whether to generate parser debug traces.
  bool trace_parsing;

  // Handling the scanner.
  void scan_begin ();
  void scan_begin (const char *Input);

  void scan_end ();
  // Whether to generate scanner debug traces.
  bool trace_scanning;
  // The token's location used by the scanner.
  yy::location location;

  void halt();
  void Run();
  void Run(std::string id);
  void execute(std::shared_ptr<StatementClass> s);
  void AddStatement(std::shared_ptr<StatementClass> s);
  void compile(std::string id);
  void Print(std::string id);
  void Dump();
  void SetParserDebugLevel(int Level);
  void Tree(std::string FilePath = "");
  void ReportError(const yy::location& l, const std::string& m);
  ErrorListType GetErrors() {return Errors;}
  std::map<std::string, std::list<std::shared_ptr<StatementClass>>> const &GetListOfDefinedFunctions();
};
#endif // ! DRIVER_HH
