#-------------------------------------------------
#
# Project created by QtCreator 2018-12-03T06:36:19
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PathEvaluationDlg
TEMPLATE = app


SOURCES += main.cpp\
        qpathevaluationdlg.cpp \
    qpathevaluation.cpp \
    searchnode.cpp

HEADERS  += qpathevaluationdlg.h \
    qpathevaluation.h \
    searchnode.h \
    pareto.h

FORMS    += qpathevaluationdlg.ui
