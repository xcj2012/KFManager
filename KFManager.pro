QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH +=\
        include
INCLUDEPATH += $$PWD/include/zlib
SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/projectmanager.cpp \
    src/new_project.cpp \
    src/pugixml.cpp \
    src/manage_project.cpp \
    src/global.cpp \
    src/libmanage.cpp \
    src/zip_process.cpp \
    include/zlib/ioapi.c \
    include/zlib/mztools.c \
    include/zlib/unzip.c \
    include/zlib/zip.c \
    src/objectproperty.cpp \
    src/mytableview.cpp \
    src/scddelegate.cpp \
    src/packagetool.cpp

HEADERS += \
    include/mainwindow.h \
    include/pugixml.hpp \
    include/pugiconfig.hpp \
    include/projectmanager.h \
    include/new_project.h \
    include/global.h \
    include/manage_project.h \
    include/libmanage.h \
    include/zip_process.h \
    include/zlib/crypt.h \
    include/zlib/ioapi.h \
    include/zlib/mztools.h \
    include/zlib/unzip.h \
    include/zlib/zip.h \
    include/objectproperty.h \
    include/mytableview.h \
    include/scddelegate.h \
    include/packagetool.h

FORMS += \
    ui/mainwindow.ui \
    ui/projectmanager.ui \
    ui/new_project.ui \
    ui/manage_project.ui \
    ui/libmanage.ui \
    ui/zip_process.ui \
    ui/objectproperty.ui \
    ui/packagetool.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resouce/icon.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/release/ -lz
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/debug/ -lz
else:unix: LIBS += -L$$PWD/lib/ -lz

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/release/libz.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/debug/libz.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/release/z.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/debug/z.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/libz.a
