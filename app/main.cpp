#include <QApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QScreen>

#include "drawingarea.h"

#ifdef Q_PROCESSOR_ARM
#include "digitizer.h"

#include <QtPlugin>
Q_IMPORT_PLUGIN(QsgEpaperPlugin)
#endif

int main(int argc, char *argv[])
{
#ifdef Q_PROCESSOR_ARM
    qDebug() << "we're running on an epaper device";
    QCoreApplication::addLibraryPath("/data/lib/");
    qputenv("QMLSCENE_DEVICE", "epaper");
    qputenv("QT_QPA_PLATFORM", "minimal:enable_fonts");
    qputenv("QT_QPA_FONTDIR", "/data/fonts/");
    qputenv("QT_QPA_EVDEV_KEYBOARD_PARAMETERS", "/dev/input/event0");
#endif
    QApplication app(argc, argv);

#ifdef Q_PROCESSOR_ARM
    if (!Digitizer::initialize("/dev/input/event1")) {
        qWarning() << "Bailing without digitizer";
        return 1;
    }
#endif

    qmlRegisterType<DrawingArea>("com.magmacompany", 1, 0, "DrawingArea");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

