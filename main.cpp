#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QQuickView>
#include <QUrl>
#include <QTimer>
#include <QScreen>
#include "pulseaudiocontroller.h"
#include "audiomanagerdbusinterface.h"
#include "pulseplayer.h"

#define AM_POC_SURFACE_ID 20

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<PulsePlayer>("com.windriver.ammonitor", 1, 0, "PAPlayer");
    qmlRegisterType<PulseAudioController>("com.windriver.ammonitor", 1, 0, "PAClient");
    qmlRegisterType<AudioManagerDBusInterface>("com.windriver.ammonitor", 1, 0, "AMClient");


    QQuickView view;

    view.setProperty("IVI-Surface-ID", AM_POC_SURFACE_ID);

    if(app.arguments().contains("--debug")) {
        view.setSource(QUrl("main.qml"));
        QSize size(1365, 768);
        if(app.arguments().contains("--sd"))
            size = QSize(1024, 768-68 /* bottom panel height */);
        view.rootObject()->setWidth(size.width());
        view.rootObject()->setHeight(size.height());
        view.resize(size);
        view.show();
    } else if(app.arguments().contains("--help")) {
        qCritical() << "Usage: ";
        qCritical() << " " << app.arguments().at(0) << " [Options]";
        qCritical() << "";
        qCritical() << "[Options]";
        qCritical() << "  --debug       show as window size 1366x768 (default is fullscreen)";
        qCritical() << "                and load QML resources from filesystem";
        qCritical() << "  --sd          show as window size 1024x700";
        qCritical() << "  --help        show help options";
        return 1;
    }else {
        view.setSource(QUrl("qrc:///main.qml"));
        QSize size = app.primaryScreen()->size();
        view.rootObject()->setWidth(size.width());
        view.rootObject()->setHeight(size.height());
        view.showFullScreen();
    }

    return app.exec();
}
