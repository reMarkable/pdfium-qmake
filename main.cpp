#include <QApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QScreen>

#include "digitizer.h"

int main(int argc, char *argv[])
{
#ifdef Q_PROCESSOR_ARM
    qDebug() << "we're running on an epaper device";
    QCoreApplication::addLibraryPath("/data/lib/");
    qputenv("QMLSCENE_DEVICE", "epaper");
    qputenv("QT_QPA_PLATFORM", "minimal:enable_fonts");
    qputenv("QT_QPA_FONTDIR", "/data/fonts/");
    qputenv("QT_QPA_EVDEV_KEYBOARD_PARAMETERS", "/dev/input/event0");

    Digitizer digitizer("/dev/input/event1");

    if (!digitizer.isRunning()) {
        qWarning() << "Failed to initialize digitizer driver!";
    }

#endif

    QApplication app(argc, argv);

    QGuiApplication::primaryScreen()->size();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

