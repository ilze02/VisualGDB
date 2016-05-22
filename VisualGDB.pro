TEMPLATE = app
TARGET = VisualGDB

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    main.cpp \
    vdwindow.cpp \
    gdbmiparser.cpp \
    vdvariable.cpp \
    vdvariablelist.cpp \
    gdbmiwriter.cpp

HEADERS += \
    vdwindow.h \
    gdbmiparser.h \
    vdvariable.h \
    vdvariablelist.h \
    gdbmiwriter.h

RESOURCES += \
    VD-icons/icons-resource.qrc





unix:!macx: LIBS += -L$$PWD/../prefix/lib/ -lgdbwire

INCLUDEPATH += $$PWD/../prefix
DEPENDPATH += $$PWD/../prefix

unix:!macx: PRE_TARGETDEPS += $$PWD/../prefix/lib/libgdbwire.a


