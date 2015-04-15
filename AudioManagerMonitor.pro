TEMPLATE = app

QT += qml quick dbus
CONFIG += link_pkgconfig

CONFIG += c++11
SOURCES += main.cpp \
    pulseaudiocontroller.cpp \
    pulseplayer.cpp \
    audiomanagerdbusinterface.cpp \

RESOURCES += qml.qrc

QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

FORMS +=

HEADERS += \
    pulseaudiocontroller.h \
    pulseplayer.h \
    audiomanagerdbusinterface.h \


#unix:!macx: LIBS += -L$$PWD/../../../../../../usr/lib/x86_64-linux-gnu/ -lpulse
#INCLUDEPATH += $$PWD/../../../../../../usr/include
#DEPENDPATH += $$PWD/../../../../../../usr/include
#unix:!macx: LIBS += -lpulse-mainloop-glib -lpulse-simple

PKGCONFIG += libpulse


DISTFILES += \
    commandlist.txt
