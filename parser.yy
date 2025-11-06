%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.1"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  # include <string>
  # include <list>
  class driver;
  class ExpressionClass;
  class StatementClass;
  class ConditionalExpressionClass;
  class VariableClass;
}

// The parsing context.
%param { driver& drv }

%locations

%define parse.trace
%define parse.error verbose

%code {
# include "driver.hh"
# include "compact.h"
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
;

%token <std::string> IDENTIFIER "identifier"
%token <int> NUMBER "number"
%type  <std::shared_ptr<ExpressionClass>> exp
%type  <std::list<std::shared_ptr<StatementClass>>> statements
%type  <std::shared_ptr<StatementClass>> statement
%type  <std::shared_ptr<StatementClass>> assignment
%type  <std::shared_ptr<StatementClass>> definition
%type  <std::shared_ptr<StatementClass>> loopstatement
%type  <std::shared_ptr<StatementClass>> functiondefinition
%type  <std::shared_ptr<ExpressionClass>> functioncall
%type  <std::shared_ptr<ConditionalExpressionClass>> condexp
%type  <std::list<std::shared_ptr<VariableClass>>> argumentlist


%printer { yyoutput << $$; } <*>;
%printer { yyoutput << "Statement list[" << $$.size() << "]"; } <std::list<std::shared_ptr<StatementClass>>>;
%printer { yyoutput << "Parameter list[" << $$.size() << "]"; } <std::list<std::shared_ptr<VariableClass>>>;

%%
%start unit;
unit: statements {drv.result = $1;}

statements:
  statement              {$$ = std::list<std::shared_ptr<StatementClass>>(); $$.push_back($1);}
| statements statement   {$1.push_back($2); $$ = $1;}
| statements error '\n'  {  drv.halt(); yyerrok; $$ = $1; std::cout << "size = " << $1.size() << std::endl; /* simple error recovery */ }

statement:
  definition ";"
| assignment ";"        {$$ = $1;}
| loopstatement ";"     {$$ = $1;}
| functioncall ";"      {$$ = $1;}

loopstatement:
  "repeat" statements "until" "(" condexp ")" {$$ = std::make_shared<RepeatLoopClass>($2, $5);}

assignment:
  "identifier" ":=" exp { $$ = std::make_shared<AssignementClass>($3, drv.Variables.GetOrCreateVariable($1, 0.0)); }

referement:
  "identifier" "->" exp { $$ = std::make_shared<AssignementClass>($3, drv.Variables.GetOrCreateVariable($1, 0.0)); }

definition:
  functiondefinition {$$ = $1;}

functioncall:
  "identifier" {drv.Currentfunction.Set($1);} "(" parameterlist ")"

parameterlist:
  %empty                    {$$ = std::list<>();}
| parameterlist parameter   {$$ = $1; $$.push_back($2);}

parameter:
   "identifier" "->" exp  {$$ = drv.Currentfunction.MakeRef($1, $3);}
|  "identifier" ":=" exp  {$$ = drv.Currentfunction.MakeAssign($1, $3);}


functiondefinition:
  "function" "identifier" {drv.Variables.CreateNewContext($2+"Params"); } "(" argumentlist ")" {drv.Variables.CreateNewContext($2); } statements "endfunction" {$$ = std::make_shared<FunctionClass>($2, $5, $8);}

argumentlist:
  "identifier"           {$$ = std::list<std::shared_ptr<VariableClass>>(); auto var = drv.Variables.CreateVariable($1, 0.0); $$.push_back(var);}
| argumentlist "," "identifier" {auto var = drv.Variables.CreateVariable($3, 0.0); $1.push_back(var); $$ = $1; }

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
| "(" condexp ")"   { std::swap ($$, $2); }


%left "+" "-";
%left "*" "/";
exp:
  exp "+" exp   { $$ = std::make_shared<AdditionClass>($1, $3); }
| exp "-" exp   { $$ = std::make_shared<AdditionClass>($1, std::make_shared<NegationClass>($3)); }
| exp "*" exp   { $$ = std::make_shared<MultiplyClass>($1, $3); }
| exp "/" exp   { $$ = std::make_shared<MultiplyClass>($1, std::make_shared<InverseClass>($3)); }
| "(" exp ")"   { std::swap ($$, $2); }
| "identifier"  { $$ = std::make_shared<VariableValueClass>(drv.Variables.GetVariableReference($1)); }
| "number"      { $$ = std::make_shared<ConstantClass>($1); };
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
