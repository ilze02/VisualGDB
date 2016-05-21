TEMPLATE = app
TARGET = VisualGDB

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    main.cpp \
    vdwindow.cpp \
    vdgraph.cpp \
    gdbmiparser.cpp \
    vdvariable.cpp \
    vdvariablelist.cpp

HEADERS += \
    vdwindow.h \
    vdgraph.h \
    gdbmiparser.h \
    vdvariable.h \
    vdvariablelist.h

RESOURCES += \
    VD-icons/icons-resource.qrc





unix:!macx: LIBS += -L$$PWD/../prefix/lib/ -lgdbwire

INCLUDEPATH += $$PWD/../prefix
DEPENDPATH += $$PWD/../prefix

unix:!macx: PRE_TARGETDEPS += $$PWD/../prefix/lib/libgdbwire.a

unix:!macx: LIBS += -L$$PWD/../prefix_graphviz/lib/ -lgvc

INCLUDEPATH += $$PWD/../prefix_graphviz/include
DEPENDPATH += $$PWD/../prefix_graphviz/include

unix:!macx: LIBS += -L$$PWD/../prefix_graphviz/lib/ -lcgraph

INCLUDEPATH += $$PWD/../prefix_graphviz/include
DEPENDPATH += $$PWD/../prefix_graphviz/include

unix:!macx: LIBS += -L$$PWD/../prefix_graphviz/lib/ -lcdt

INCLUDEPATH += $$PWD/../prefix_graphviz/include
DEPENDPATH += $$PWD/../prefix_graphviz/include
