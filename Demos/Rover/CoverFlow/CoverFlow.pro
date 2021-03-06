TEMPLATE = lib
CONFIG += shared plugin # the plugin is to avoid symbolic links to the .so or .dylib
TARGET = CoverFlowView
DESTDIR = $$PWD/../../../Binary/Demos/RoverComponents

QT += gui widgets qml quick
INCLUDEPATH += $$PWD/../../../Include
LIBS += -L$$PWD/../../../Binary -lGCF3 -lGCFGui3

HEADERS += \
    CoverFlowComponent.h

SOURCES += \
    CoverFlowComponent.cpp

OTHER_FILES += \
    CoverFlow.xml \
    FolderCoverFlowView.qml

RESOURCES += \
    CoverFlow.qrc
