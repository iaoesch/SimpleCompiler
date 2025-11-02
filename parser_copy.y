

%{

/*
 * Parser.y file
 * To generate the parser run: "bison Parser.y"
 */

#include "Expression.h"
#include "Parser.hpp"
#include "Lexer.hpp"

// reference the implementation provided in Lexer.l
int yyerror(SExpression **expression, yyscan_t scanner, const char *msg);

%}

%code requires {
   typedef void* yyscan_t;
}

/*
%code requires {
  typedef void* yyscan_t;
}
*/
%language "C++"
%output  "Parser.cpp"
%defines "Parser.hpp"

//%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { SExpression **expression }
%parse-param { yyscan_t scanner }

%union {
    int value;
    SExpression *expression;
}

%token TOKEN_LPAREN   "("
%token TOKEN_RPAREN   ")"
%token TOKEN_PLUS     "+"
%token TOKEN_STAR     "*"
%token <value> TOKEN_NUMBER "number"

%type <expression> expr

/* Precedence (increasing) and associativity:
   a+b+c is (a+b)+c: left associativity
   a+b*c is a+(b*c): the precedence of "*" is higher than that of "+". */
%left "+"
%left "*"

%%

input
    : expr { *expression = $1; }
    ;

expr
    : expr[L] "+" expr[R] { $$ = createOperation( eADD, $L, $R ); }
    | expr[L] "*" expr[R] { $$ = createOperation( eMULTIPLY, $L, $R ); }
    | "(" expr[E] ")"     { $$ = $E; }
    | "number"            { $$ = createNumber($1); }
    ;

%%

