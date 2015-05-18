TEMPLATE = app

QT += qml quick dbus
CONFIG += link_pkgconfig resources_big

CONFIG += c++11
SOURCES += main.cpp \
    pulseaudiocontroller.cpp \
    pulseplayer.cpp \
    audiomanagerdbusinterface.cpp \

RESOURCES += qml.qrc

QML_IMPORT_PATH =

# Default rules for deployment.

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


#DISTFILES +=
target.path = /usr/bin
INSTALLS += target
