
%{

/*
 * Lexer.l file
 * To generate the lexical analyzer run: "flex Lexer.l"
 */

#include "Expression.h"
#include "lexer.hpp"
#include "parser.hpp"

#include <stdio.h>

#undef  YY_DECL
#define YY_DECL int FooLexer::yylex(std::string *const yylval)

%}

%option outfile="Lexer.cpp" header-file="Lexer.hpp"
%option warn nodefault

%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

%option c++ noyywrap

%option yyclass="FooLexer"

%%

[ \r\n\t]*   { continue; /* Skip blanks. */ }
[0-9]+       { sscanf(yytext, "%d", &yylval->value); return TOKEN_NUMBER; }

"*"          { return TOKEN_STAR; }
"+"          { return TOKEN_PLUS; }
"("          { return TOKEN_LPAREN; }
")"          { return TOKEN_RPAREN; }

.            { continue; /* Ignore unexpected characters. */}

%%

int yyerror(SExpression **expression, yyscan_t scanner, const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    return 0;
}

