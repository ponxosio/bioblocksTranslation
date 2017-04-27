#-------------------------------------------------
#
# Project created by QtCreator 2017-04-25T12:53:42
#
#-------------------------------------------------

# ensure one "debug_and_release" in CONFIG, for clarity...
debug_and_release {
    CONFIG -= debug_and_release
    CONFIG += debug_and_release
}
    # ensure one "debug" or "release" in CONFIG so they can be used as
    #   conditionals instead of writing "CONFIG(debug, debug|release)"...
CONFIG(debug, debug|release) {
    CONFIG -= debug release
    CONFIG += debug
}
CONFIG(release, debug|release) {
    CONFIG -= debug release
    CONFIG += release
}


QT       -= gui

TARGET = bioblocksTranslation
TEMPLATE = lib

DEFINES += BIOBLOCKSTRANSLATOR_LIBRARY

unix {
    target.path = /usr/lib
    INSTALLS += target
}

HEADERS += \
    bioblocksTranslation/bioblockstranslator_global.h \
    bioblocksTranslation/interfaces/translationinterface.h \
    bioblocksTranslation/bioblockstranslator.h \
    bioblocksTranslation/blocks/operationsblocks.h \
    bioblocksTranslation/blocks/containermanager.h \
    bioblocksTranslation/blocks/logicblocks.h \
    bioblocksTranslation/blocks/mathblocks.h \
    bioblocksTranslation/blocks/blocksutils.h

debug {
    QMAKE_POST_LINK=X:\bioblocksTranslation\bioblocksTranslation\setDLL.bat $$shell_path($$OUT_PWD/debug) debug

    INCLUDEPATH += X:\utils\dll_debug\include
    LIBS += -L$$quote(X:\utils\dll_debug\bin) -lutils

    INCLUDEPATH += X:\protocolGraph\dll_debug\include
    LIBS += -L$$quote(X:\protocolGraph\dll_debug\bin) -lprotocolGraph
}

!debug {
    QMAKE_POST_LINK=X:\protocolGraph\protocolGraph\setDLL.bat $$shell_path($$OUT_PWD/release) release

    INCLUDEPATH += X:\utils\dll_release\include
    LIBS += -L$$quote(X:\utils\dll_release\bin) -lutils

    INCLUDEPATH += X:\protocolGraph\dll_release\include
    LIBS += -L$$quote(X:\protocolGraph\dll_release\bin) -lprotocolGraph
}

SOURCES += \
    bioblocksTranslation/bioblockstranslator.cpp \
    bioblocksTranslation/blocks/operationsblocks.cpp \
    bioblocksTranslation/blocks/containermanager.cpp \
    bioblocksTranslation/blocks/logicblocks.cpp \
    bioblocksTranslation/blocks/mathblocks.cpp \
    bioblocksTranslation/blocks/blocksutils.cpp


INCLUDEPATH += X:\libraries\json-2.1.1\src
INCLUDEPATH += X:\libraries\boost_1_63_0
INCLUDEPATH += X:\libraries\cereal-1.2.2\include
