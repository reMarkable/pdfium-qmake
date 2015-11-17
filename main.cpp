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

    Digitizer digitizer("/dev/input/event1");
#endif

    QApplication app(argc, argv);

    QGuiApplication::primaryScreen()->size();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

