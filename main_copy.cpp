#if 0
flex.name = Flex ${QMAKE_FILE_IN}
            flex.input = FLEXSOURCES
          flex.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.lexer.cpp
                               flex.commands = flex -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
             flex.CONFIG += target_predeps
           flex.variable_out = GENERATED_SOURCES
          silent:flex.commands = @echo Lex ${QMAKE_FILE_NAME} && $$flex.commands
          QMAKE_EXTRA_COMPILERS += flex
--------
    ### For building the gnu flex part
        FLEXSOURCES += my_lex_sourcefile.l
           flex.input = FLEXSOURCES
          flex.output = ${QMAKE_FILE_BASE}.cpp
          flex.commands = flex  -o ${QMAKE_FILE_IN_BASE}.c ${QMAKE_FILE_IN}
             flex.variable_out = SOURCES
          flex.CONFIG += target_predeps
           flex.clean =  ${QMAKE_FILE_IN_BASE}.c
          flex.name = flex
# finally add this to the qmake project so it will have the makefile generated
        QMAKE_EXTRA_COMPILERS += flex

#endif


/*
 * main.c file
 */

#include "Expression.h"
#include "Parser.hpp"
#include "Lexer.hpp"

#include <stdio.h>

int yyparse(SExpression **expression, yyscan_t scanner);

SExpression *getAST(const char *expr)
{
    SExpression *expression;
    yyscan_t scanner;
    YY_BUFFER_STATE state;

    if (yylex_init(&scanner)) {
        /* could not initialize */
        return NULL;
    }

    state = yy_scan_string(expr, scanner);

    if (yyparse(&expression, scanner)) {
        /* error parsing */
        return NULL;
    }

    yy_delete_buffer(state, scanner);

    yylex_destroy(scanner);

    return expression;
}

int evaluate(SExpression *e)
{
    switch (e->type) {
        case eVALUE:
            return e->value;
        case eMULTIPLY:
            return evaluate(e->left) * evaluate(e->right);
        case eADD:
            return evaluate(e->left) + evaluate(e->right);
        default:
            /* should not be here */
            return 0;
    }
}

int main(void)
{
    char test[] = " 4 + 2*10 + 3*( 5 + 1 )";
    SExpression *e = getAST(test);
    int result = evaluate(e);
    printf("Result of '%s' is %d\n", test, result);
    deleteExpression(e);
    return 0;
}

int yyerror(SExpression **expression, yyscan_t scanner, const char *msg)
{
    printf("Scanner error :%s\n", msg);
}


#if 0
A simple makefile to build the project is the following.

# Makefile

FILES = Lexer.c Parser.c Expression.c main.c
CC = g++
CFLAGS = -g -ansi

test: $(FILES)
    $(CC) $(CFLAGS) $(FILES) -o test

Lexer.c: Lexer.l
    flex Lexer.l

Parser.c: Parser.y Lexer.c
    bison Parser.y

clean:
    rm -f *.o *~ Lexer.c Lexer.h Parser.c Parser.h test
#endif
