QT       += core gui
QT       += svgwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



#TEMPLATE = app
CONFIG += console c++17
#CONFIG -= app_bundle
#CONFIG -= qt

SOURCES += \
   Errclass.cpp \
   ParserManual.cpp \
   Untitled1.cpp \
   calc++.cc \
   compact.cpp \
   driver.cc \
   environment.cpp \
   highlighter.cpp \
   typedescriptorclass.cpp \
   variableclass.cpp \
   varmanag.cpp\
    maingui.cpp \
    mainwindow.cpp


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
   environment.hpp \
   highlighter.h \
   typedescriptorclass.hpp \
   variableclass.h \
   varmanag.hpp \
    mainwindow.h


DISTFILES += \
   Makefile \
   README \
   calc++.test \
   local.mk

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


