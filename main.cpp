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

#include "parser.h"
#include <iostream>
using namespace std;
int main(int argc, char ** argv)
{
    Parser parser;
    parser.yyparse();
    return 0;
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
