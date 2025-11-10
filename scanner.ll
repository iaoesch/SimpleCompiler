%{ /* -*- C++ -*- */
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <string>
# include "driver.hh"
# include "parser.hpp"

// Work around an incompatibility in flex (at least versions
// 2.5.31 through 2.5.33): it generates code that does
// not conform to C89.  See Debian bug 333231
// <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.
# undef yywrap
# define yywrap() 1

// Pacify warnings in yy_init_buffer (observed with Flex 2.6.4)
// and GCC 7.3.0.
#if defined __GNUC__ && 7 <= __GNUC__
# pragma GCC diagnostic ignored "-Wnull-dereference"
#endif
%}

%option noyywrap nounput batch debug noinput

id    [a-zA-Z][a-zA-Z_0-9]*
int   [0-9]+
float [0-9]*([0-9]\.?|\.[0-9])[0-9]*([Ee][-+]?[0-9]+)
string   \"(\(.|\n)|[^\\"\n])*\"
string2  \"(\\.|\\\n|[^"\])*\"
string3  ["]([^"\n\\]|\\(.|\n))*["]
blank [ \t]

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}
%%
%{
  // A handy shortcut to the location held by the driver.
  yy::location& loc = drv.location;
  // Code run each time yylex is called.
  loc.step ();
%}
{blank}+   loc.step ();
[\n]+      loc.lines (yyleng); loc.step ();

"-"      return yy::parser::make_MINUS  (loc);
"+"      return yy::parser::make_PLUS   (loc);
"*"      return yy::parser::make_STAR   (loc);
"/"      return yy::parser::make_SLASH  (loc);
"("      return yy::parser::make_LPAREN (loc);
")"      return yy::parser::make_RPAREN (loc);
"["      return yy::parser::make_LBRACKET (loc);
"]"      return yy::parser::make_RBRACKET (loc);
"{"      return yy::parser::make_LBRACE (loc);
"}"      return yy::parser::make_RBRACE (loc);
":="     return yy::parser::make_ASSIGN (loc);
"=="     return yy::parser::make_EQUAL (loc);
"<"      return yy::parser::make_LESSTHAN (loc);
";"      return yy::parser::make_SEMICOLON (loc);
","      return yy::parser::make_KOMMA (loc);
"->"     return yy::parser::make_REFERTO (loc);
"repeat"      return yy::parser::make_REPEAT (loc);
"until"      return yy::parser::make_UNTIL (loc);
"function"      return yy::parser::make_FUNCTION (loc);
"endfunction"      return yy::parser::make_ENDFUNCTION (loc);
"compile"   return yy::parser::make_COMPILE (loc);
"run"    return yy::parser::make_RUN (loc);
"dump"   return yy::parser::make_DUMP (loc);
"exit"   return yy::parser::make_END (loc);
"$"      return yy::parser::make_END (loc);

{int}      {
  errno = 0;
  long n = strtol (yytext, NULL, 10);
  if (! (INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
    throw yy::parser::syntax_error (loc, "integer is out of range: "
                                    + std::string(yytext));
  return yy::parser::make_INTEGER (n, loc);
}

{float}    {
             errno = 0;
             char *end;
             double f = std::strtod(yytext, &end);

             if (errno == ERANGE)
             {
                 errno = 0;
                 throw yy::parser::syntax_error (loc, "float is out of range: "
                                                         + std::string(yytext));
             }
             if ((end - yytext) != yyleng) {
                 throw yy::parser::syntax_error (loc, "float internal error: used only "
                                                         + std::to_string(end - yytext)
                                                         + " characters of <"
                                                         + std::string(yytext)
                                                         + ">");

             }


             return yy::parser::make_FLOAT (f, loc);
      }

{id}       return yy::parser::make_IDENTIFIER (yytext, loc);

.          {
             throw yy::parser::syntax_error
               (loc, "invalid character: " + std::string(yytext));
            }

{string3}   return yy::parser::make_STRING (std::string(yytext), loc);

<<EOF>>    return yy::parser::make_END (loc);

"//".*                                    { /* eat one line comments */ }
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]       { /* eat multi line comments */ }
[/][*]                                    { yy_fatal_error("Unterminated comment"); }

%%

static bool DoNotCloseyyin = false;

void
driver::scan_begin ()
{
  yy_flex_debug = trace_scanning;
    DoNotCloseyyin = false;
    if (file.empty () || file == "-") {
        yyin = stdin;
        DoNotCloseyyin = true;
    }
  else if (!(yyin = fopen (file.c_str (), "r")))
    {
      std::cerr << "cannot open " << file << ": " << strerror(errno) << '\n';
      exit (EXIT_FAILURE);
    }
}

void
driver::scan_end ()
{
    if (DoNotCloseyyin == false) {
  fclose (yyin);
    }
}
