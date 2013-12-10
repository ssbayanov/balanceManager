#-------------------------------------------------
#
# Project created by QtCreator 2013-12-04T10:53:53
#
#-------------------------------------------------

QT       += core gui webkit network webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = balanceManager
TEMPLATE = app


SOURCES += main.cpp\
        balancewindow.cpp\
        smtp.cpp \
    settingsdialog.cpp \
    balancewebview.cpp

HEADERS  += balancewindow.h\
        smtp.h \
    settingsdialog.h \
    balancewebview.h

FORMS    += balancewindow.ui \
    settingsdialog.ui
