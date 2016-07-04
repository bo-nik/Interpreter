#-------------------------------------------------
#
# Project created by QtCreator 2013-04-26T12:45:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = interpretator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    code_editor/highlighter.cpp \
    code_editor/codeeditor.cpp \
    analyzer/lexicalanalyzer.cpp \
    analyzer/analyzer.cpp \
    analyzer/syntaxanalyzer.cpp \
    analyzer/precedencetablebuilder.cpp \
    console/console.cpp \
    analyzer/evaluator.cpp \
    tools/grammardialog.cpp \
    tools/priorityshowdialog.cpp \
    tools/precedencetabledialog.cpp \
    tools/outputdialog.cpp \
    tools/polisbuilddialog.cpp \
    tools/polisshowdialog.cpp

HEADERS  += mainwindow.h \
    code_editor/highlighter.h \
    code_editor/codeeditor.h \
    analyzer/lexicalanalyzer.h \
    analyzer/analyzer.h \
    analyzer/syntaxanalyzer.h \
    analyzer/precedencetablebuilder.h \
    console/console.h \
    analyzer/evaluator.h \
    tools/grammardialog.h \
    tools/priorityshowdialog.h \
    tools/precedencetabledialog.h \
    tools/outputdialog.h \
    tools/polisbuilddialog.h \
    tools/polisshowdialog.h

RESOURCES += \
    resources.qrc
