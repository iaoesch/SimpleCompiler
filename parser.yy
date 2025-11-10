%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.1"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  # include <string>
  # include <list>
  #include"variableclass.h"
  class driver;
  class ExpressionClass;
  class FunctionCallClass;
  class StatementClass;
  class ConditionalExpressionClass;
  class VariableClass;
  namespace Variables {
     class FunctionDefinitionClass;
     class VariableContentClass;
  }
  typedef std::shared_ptr<Variables::FunctionDefinitionClass> FunctionDefinitionClassSharedPtr;

  class FktDefContainer {
     public:
     std::shared_ptr<Variables::FunctionDefinitionClass> ptr;
  };

}

// The parsing context.
%param { driver& drv }

%locations

%define parse.trace
%define parse.error verbose

%code {
# include "driver.hh"
# include "compact.h"
# include "variableclass.h"
}
// typen: integer, float, string, stack, liste, array, map, function, dynamic, fixed

// fixed a = 5.0;
// fixed b = array of nnn integer
// c = "Hallo"
// a = 7;   ok
// b[n] = 7; ok
// a = {1, 4, 8} -> error change type not allowed
// c = 7; -> ok, change type

%define api.token.prefix {TOK_}
%token
  END  0  "end of file"
  ASSIGN  ":="
  MINUS   "-"
  PLUS    "+"
  STAR    "*"
  SLASH   "/"
  LPAREN  "("
  RPAREN  ")"
  LBRACKET "["
  RBRACKET  "]"
  LBRACE   "{"
  RBRACE   "}"
  KOMMA   ","
  SEMICOLON  ";"
  REPEAT  "repeat"
  UNTIL   "until"
  FUNCTION "function"
  ENDFUNCTION "endfunction"
  AND     "and"
  OR      "or"
  NOT     "not"
  REFERTO "->"
  LESSTHAN "<"
  LESSORSAME "<="
  EQUAL   "=="
  NOTEQUAL "!="
  MORETHAN   ">"
  MOREORSAME   ">="
  COMPILE "compile"
  RUN     "run"
  DUMP    "dump"
;

%token <std::string> IDENTIFIER "identifier"
%token <int64_t> INTEGER "integer"
%token <std::string> STRING "string"
%token <double> FLOAT "float"


%type  <std::shared_ptr<ExpressionClass>> exp
%type  <std::list<std::shared_ptr<StatementClass>>> statements
%type  <std::shared_ptr<StatementClass>> statement
%type  <std::shared_ptr<StatementClass>> assignment
%type  <std::shared_ptr<StatementClass>> referement
%type  <std::shared_ptr<StatementClass>> loopstatement
%type  <std::shared_ptr<Variables::FunctionDefinitionClass>> functiondefinition
%type  <std::shared_ptr<FunctionCallClass>> functioncall
%type  <std::shared_ptr<ConditionalExpressionClass>> condexp
%type  <std::list<std::shared_ptr<VariableClass>>> argumentlist
%type  <std::shared_ptr<StatementClass>> parameter
%type  <std::list<std::shared_ptr<StatementClass>>> parameterlist
%type  <std::shared_ptr<VariableClass>> assignable
%type  <Variables::VariableContentClass> literal
%type  <Variables::VariableContentClass> numericliteral



%printer { yyoutput << $$; } <*>;
%printer { yyoutput << "Statement list[" << $$.size() << "]"; } <std::list<std::shared_ptr<StatementClass>>>;
%printer { yyoutput << "Parameter list[" << $$.size() << "]"; } <std::list<std::shared_ptr<VariableClass>>>;

%%
%start unit;
unit:
   input
|  unit input
;

input:
   statement {drv.execute($1);};
|  definition
|  command

command:
   "dump" {drv.Dump();}
|  "run"  {drv.Run();}
|  "run" "identifier" {drv.Run($2);}
|  "identifier" {drv.Print($1);}

statements:
  definition             {$$ = std::list<std::shared_ptr<StatementClass>>();}
| statements definition  {$$ = $1;}
| statement              {$$ = std::list<std::shared_ptr<StatementClass>>(); $$.push_back($1);}
| statements statement   {$1.push_back($2); $$ = $1;}
;

/*| statements error ";"  {  drv.halt(); yyerrok; $$ = $1; std::cout << "size = " << $1.size() << std::endl; /* simple error recovery  };*/

statement:
  assignment ";"        {$$ = $1;}
| loopstatement ";"     {$$ = $1;}
| functioncall ";"      {$$ = std::make_shared<FunctionCallStatementClass>($1);}
| error ";"             {$$ = std::make_shared<ErrorStatement>();}
;

loopstatement:
  "repeat" statements "until" "(" condexp ")" {$$ = std::make_shared<RepeatLoopClass>($2, $5);};

assignment:
  assignable ":=" exp { $$ = std::make_shared<AssignementClass>($3, $1); };

/* assignable ":=" exp { $$ = std::make_shared<AssignementClass>($3, drv.Variables.GetOrCreateVariable($1, $3->Type(), 0.0)); }; */

assignable:
  "identifier"  { $$ = drv.Variables.GetOrCreateVariable($1, TypeDescriptorClass(TypeDescriptorClass::Type::Undefined), 0.0); };
| assignable "[" rangedindexes "]"
| assignable "{" rangedindex "}"
;

rangedindexes:
   rangedindex
|  rangedindexes "," rangedindex
;

rangedindex:
   exp
|  exp "-" exp
|  "*"



referement:
  "identifier" "->" exp { $$ = std::make_shared<AssignementClass>($3, drv.Variables.GetOrCreateVariable($1, 0.0)); };

definition:
  functiondefinition {};


functioncall:
  "identifier" {drv.Currentfunction.Set($1, @1);} "(" parameterlist ")" {$$ = std::make_shared<FunctionCallClass>($<FunctionDefinitionClassSharedPtr>2, $4);};

parameterlist:
  %empty                    {$$ = std::list<std::shared_ptr<StatementClass>>();}
| parameterlist parameter   {$$ = $1; $$.push_back($2);};

parameter:
   "identifier" "->" exp  {$$ = drv.Currentfunction.MakeRef($1, $3);}
|  "identifier" ":=" exp  {$$ = drv.Currentfunction.MakeAssign($1, $3);};


functiondefinition:
  "function" "identifier" {
                             /*FktDefContainer tmp;*/
                             /*tmp.ptr = */drv.Currentfunction.Create($2, @2);
                             /*$<FktDefContainer>$ = tmp;*/
                             drv.Variables.CreateNewContext($2+"Params"); }
  "(" argumentlist ")"    {drv.Variables.CreateNewContext($2); }
  statements
  "endfunction" {/**$<FktDefContainer>3 = Variables::FunctionDefinitionClass($5, $8);*/ /*$$ = $<FktDefContainer>3.ptr;*/drv.Currentfunction.Define(Variables::FunctionDefinitionClass($5, $8), @8); $$ = drv.Currentfunction.Get(@8); drv.Variables.LeaveContext(2);}
| error "endfunction"
;

argumentlist:
  "identifier"           {$$ = std::list<std::shared_ptr<VariableClass>>(); auto var = drv.Variables.CreateVariable($1, TypeDescriptorClass(TypeDescriptorClass::Type::Dynamic), 0.0); $$.push_back(var);}
| argumentlist "," "identifier" {auto var = drv.Variables.CreateVariable($3, TypeDescriptorClass(TypeDescriptorClass::Type::Dynamic), 0.0); $1.push_back(var); $$ = $1; };

%left or;
%left and;
%left not;
%left ">" ">=" "==" "!=" "<" "<=";

condexp:
  condexp and condexp   { $$ = std::make_shared<AndClass>($1, $3); }
| condexp or condexp   { $$ = std::make_shared<AndClass>($1, $3); }
| not condexp    { $$ = std::make_shared<AndClass>($2, $2); }
| exp ">" exp    { $$ = std::make_shared<LessThanClass>($1, $3); }
| exp ">=" exp   { $$ = std::make_shared<LessThanClass>($1, $3);}
| exp "==" exp   { $$ = std::make_shared<LessThanClass>($1, $3); }
| exp "!=" exp   { $$ = std::make_shared<LessThanClass>($1, $3); }
| exp "<" exp    { $$ = std::make_shared<LessThanClass>($1, $3); }
| exp "<=" exp   { $$ = std::make_shared<LessThanClass>($1, $3); }
| "(" condexp ")"   { std::swap ($$, $2); };


%left "+" "-";
%left "*" "/";
exp:
  exp "+" exp   { $$ = std::make_shared<AdditionClass>($1, $3); }
| exp "-" exp   { $$ = std::make_shared<AdditionClass>($1, std::make_shared<NegationClass>($3)); }
| exp "*" exp   { $$ = std::make_shared<MultiplyClass>($1, $3); }
| exp "/" exp   { $$ = std::make_shared<MultiplyClass>($1, std::make_shared<InverseClass>($3)); }
| "(" exp ")"   { std::swap ($$, $2); }
| "identifier"  { $$ = std::make_shared<VariableValueClass>(drv.Variables.GetVariableReference($1)); }
| literal       { $$ = std::make_shared<ConstantClass>($1); }
;

literal:
  numericliteral {$$ = $1;}
| "string"       {$$ = Variables::VariableContentClass($1); }
| arrayliteral   {$$ = Variables::VariableContentClass::MakeUndefined();}
| listliteral    {$$ = Variables::VariableContentClass::MakeUndefined();}
| mapliteral     {$$ = Variables::VariableContentClass::MakeUndefined();}
;

numericliteral:
  "integer"   { $$ = Variables::VariableContentClass($1); }
| "float"     { $$ = Variables::VariableContentClass($1); }
;

arrayliteral:
   "[" arrayentries "]"
;

arrayentries:
   arrayentry
|  arrayentries "," arrayentry
;

arrayentry:
   literal
|  "[" arrayentries "]"

listliteral:
   "{" listentries "}"
;

listentries:
   literal
|  listentries "," literal
;

mapliteral:
   "[" mapentries "]"
;

mapentries:
   mapentry
|  mapentries "," mapentry
;

mapentry:
   "<" key ":=" literal ">"
;

key:
   "string"
|  "integer"


%%


/*
expr
: term
| term '+' term
| term '-' term
;

term
: factor
| factor '*' factor
| factor '/' factor
| factor '%' factor // if you have the % operator
;

factor
: unary
| unary '^' factor // if you have an exponentiation operator. Note right-associativity
;

unary
: primary
| '+' unary
| '-' unary
;

primary
: id
| constant
| '(' expr ')'
;
*/

void
yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l;
  std::cerr << ": ";
  std::cerr << m << '\n';
  for (int i = 0; i < l.begin.column; i++) {
     std::cerr << "~";
  }
  for (int i = l.begin.column; i <= l.end.column; i++) {
     std::cerr << "^";
  }
  std::cerr << '\n';
  //std::cerr << l << ": " << m << '\n';
}
