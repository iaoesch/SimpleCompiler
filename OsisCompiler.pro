TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        expression.cpp \
        main.cpp \
        main_copy.cpp \
        typedescriptorclass.cpp \
        variableclass.cpp

#FLEXSOURCES += my_lex_sourcefile.l
FLEXSOURCES += lexer.l

flex.input = FLEXSOURCES
flex.output = ${QMAKE_FILE_BASE}.cpp
flex.commands = flex  -o ${QMAKE_FILE_IN_BASE}.cpp ${QMAKE_FILE_IN}
flex.variable_out = SOURCES
flex.CONFIG += target_predeps
flex.clean =  ${QMAKE_FILE_IN_BASE}.cpp
flex.name = flex

QMAKE_EXTRA_COMPILERS += flex

BISONSOURCES += parser.y

bison.input = FLEXSOURCES
bison.output = ${QMAKE_FILE_BASE}.cpp
bison.commands = bison  -o ${QMAKE_FILE_IN_BASE}.cpp ${QMAKE_FILE_IN}
bison.variable_out = SOURCES
bison.CONFIG += target_predeps
bison.clean =  ${QMAKE_FILE_IN_BASE}.cpp
bison.name = bison

QMAKE_EXTRA_COMPILERS += bison


HEADERS += \
   expression.h \
   typedescriptorclass.hpp \
   variableclass.h

DISTFILES += \
   lexer.l \
   lexer_copy.l \
   parser.y \
   parser_copy.y
