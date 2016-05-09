TEMPLATE = app
TARGET = VisualGDB

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    main.cpp \
    vdwindow.cpp \
    vdgraph.cpp

HEADERS += \
    vdwindow.h \
    vdgraph.h

RESOURCES += \
    VD-icons/icons-resource.qrc


LIBS += -L$$PWD/Graphviz2.38/lib/release/ -lgvc
LIBS += -L$$PWD/Graphviz2.38/lib/release/ -lcgraph
LIBS += -L$$PWD/Graphviz2.38/lib/release/ -lcdt
LIBS += -L$$PWD/Graphviz2.38/lib/release/ -lgvplugin_core
LIBS += -L$$PWD/Graphviz2.38/lib/release/ -lgvplugin_neato_layout

INCLUDEPATH += $$PWD/Graphviz2.38/include/graphviz
