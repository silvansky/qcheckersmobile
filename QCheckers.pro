# -------------------------------------------------
# Project created by QtCreator 2009-06-12T15:14:58
# -------------------------------------------------
TARGET = QCheckers
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    checkerspicture.cpp \
    checkersstate.cpp \
    checkersgame.cpp \
    settingsdialog.cpp
HEADERS += mainwindow.h \
    checkerspicture.h \
    checkersstate.h \
    checkersgame.h \
    settingsdialog.h
FORMS += mainwindow.ui \
    settingsdialog.ui
RESOURCES += icons.qrc
OTHER_FILES += README \
    COPYING
symbian: {
    ICON = qcheckers.svg
}
