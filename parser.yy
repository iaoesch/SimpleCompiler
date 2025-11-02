%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.1"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  # include <string>
  class driver;
  class ExpressionClass;
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
;

%token <std::string> IDENTIFIER "identifier"
%token <int> NUMBER "number"
%type  <std::shared_ptr<ExpressionClass>> exp

%printer { yyoutput << $$; } <*>;

%%
%start unit;
unit: assignments exp  { drv.result = $2; };

assignments:
  %empty                 {}
| assignments assignment {};

assignment:
  "identifier" ":=" exp { drv.Variables.GetVariableReference($1); }

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

void
yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l;
  std::cerr << ": ";
  std::cerr << m << '\n';
  //std::cerr << l << ": " << m << '\n';
}
