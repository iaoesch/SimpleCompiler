TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
   Errclass.cpp \
   ParserManual.cpp \
   Untitled1.cpp \
   calc++.cc \
   compact.cpp \
   driver.cc \
   variableclass.cpp \
   varmanag.cpp

#FLEXSOURCES += my_lex_sourcefile.l
FLEXSOURCES += scanner.ll

flex.input = FLEXSOURCES
flex.output = ${QMAKE_FILE_BASE}.cpp
flex.commands = flex  -o ${QMAKE_FILE_IN_BASE}.cpp ${QMAKE_FILE_IN}
flex.variable_out = SOURCES
flex.CONFIG += target_predeps
flex.clean =  ${QMAKE_FILE_IN_BASE}.cpp
flex.name = flex

QMAKE_EXTRA_COMPILERS += flex

BISONSOURCES += parser.yy

bison.input = BISONSOURCES
bison.output = ${QMAKE_FILE_BASE}.cpp
bison.commands = /opt/homebrew/Cellar/bison/3.8.2/bin/bison  -Wcounterexamples --debug --locations -o ${QMAKE_FILE_IN_BASE}.cpp ${QMAKE_FILE_IN}
bison.variable_out = SOURCES
bison.CONFIG += target_predeps
bison.clean =  ${QMAKE_FILE_IN_BASE}.cpp
bison.name = bison

QMAKE_EXTRA_COMPILERS += bison


HEADERS += \
   ERRTEXTS.IPP \
   Errclass.hpp \
   ParserManual.hpp \
   compact.h \
   driver.hh \
   variableclass.h \
   varmanag.hpp

DISTFILES += \
   Makefile \
   README \
   calc++.test \
   local.mk
