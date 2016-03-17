#include <QApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QScreen>
#include <QPainter>
#include <QStringList>
#include <QTimer>
#include <QProcess>

#include "drawingarea.h"
#include "systemmonitor.h"
#include "collection.h"
#include "pagepreview.h"
#include "settings.h"
#include "messagehandler.h"

#ifdef Q_PROCESSOR_ARM
#include "digitizer.h"
#include <epframebuffer.h>

#include <QtPlugin>
Q_IMPORT_PLUGIN(QsgEpaperPlugin)

#include <unistd.h>
#include <errno.h>

static int s_crashRecursingCounter = 0;
static char *s_programPath = nullptr;

class DocumentWorker;

static void s_crashHandler(int sig)
{
    //Q_UNUSED(sig);
    printf("counter: %d sig: %d\n", s_crashRecursingCounter, sig);

    // Ensure that we don't loop this forever
    s_crashRecursingCounter++;

    // Automatically kill ourselves if we hang for 10 seconds
    signal(SIGALRM, SIG_DFL);
    alarm(10);

    if (s_crashRecursingCounter > 1) {
        if (EPFrameBuffer::instance() && EPFrameBuffer::instance()->framebuffer()) {
            QImage *fb = EPFrameBuffer::instance()->framebuffer();
            QPainter painter(fb);
            QImage splashScreen("/data/crash.jpg");
            painter.drawImage(0, 0, splashScreen);
            EPFrameBuffer::instance()->sendUpdate(fb->rect(), EPFrameBuffer::Grayscale, EPFrameBuffer::FullUpdate, true);
        }

        printf("We have crashed too much, aborting...\n");
        _exit(255);
    }

    alarm(0);
    printf("We have crashed, attempting to restart: %s...\n", s_programPath);
    if (execl(s_programPath, s_programPath, "-havecrashed", (char*)NULL) < 0) {
        printf("Failed to launch ourselves: %d (%s)\n", errno, strerror(errno));
        _exit(255);
    }
}

#endif // Q_PROCESSOR_ARM

int main(int argc, char *argv[])
{
    //MessageHandler::enableDebugOutput(false);

#ifdef Q_PROCESSOR_ARM
    qDebug() << "we're running on an epaper device";
    QCoreApplication::addLibraryPath("/data/lib/");
    qputenv("QMLSCENE_DEVICE", "epaper");
    qputenv("QT_QPA_PLATFORM", "minimal:enable_fonts");
    qputenv("QT_QPA_FONTDIR", "/data/fonts/");
    qputenv("QT_QPA_EVDEV_KEYBOARD_PARAMETERS", "/dev/input/event0");
#endif

    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("xochitl"));
    app.setOrganizationName("remarkable");

#ifdef Q_PROCESSOR_ARM
    s_programPath = argv[0];

    { // Check if we have already crashed, avoid restart-loops
        QStringList arguments = app.arguments();
        if (arguments.contains("-havecrashed")) {
            s_crashRecursingCounter = 1;

            // If we run for more than 10 seconds,
            // assume it is okay to restart if we crash again
            QTimer::singleShot(10000, []() -> void {
                s_crashRecursingCounter = 0;
            });
        }
    }

    { // Install crash handler
        sigset_t mask;
        sigemptyset(&mask);
        signal(SIGSEGV, &s_crashHandler);
        sigaddset(&mask, SIGSEGV);
        signal(SIGBUS, &s_crashHandler);
        sigaddset(&mask, SIGBUS);
        signal(SIGFPE, &s_crashHandler);
        sigaddset(&mask, SIGFPE);
        signal(SIGILL, &s_crashHandler);
        sigaddset(&mask, SIGILL);
        signal(SIGABRT, &s_crashHandler);
        sigaddset(&mask, SIGABRT);

        sigprocmask(SIG_UNBLOCK, &mask, 0);
    }

    { // Show loading screen
        QImage *fb = EPFrameBuffer::instance()->framebuffer();
        QPainter painter(fb);
        QImage splashScreen("/data/start-crushed.png");
        painter.drawImage(0, 0, splashScreen);
        EPFrameBuffer::instance()->sendUpdate(fb->rect(), EPFrameBuffer::Grayscale, EPFrameBuffer::FullUpdate, true);
        EPFrameBuffer::instance()->framebuffer()->fill(Qt::white);
    }

    if (!Digitizer::initialize("/dev/input/event1")) {
        qWarning() << "Bailing without digitizer";
        return 1;
    }
#endif

    qmlRegisterType<DocumentWorker>();
    qmlRegisterType<DrawingArea>("com.magmacompany", 1, 0, "DrawingArea");
    qmlRegisterType<PagePreview>("com.magmacompany", 1, 0, "PagePreview");
    qmlRegisterSingletonType<SystemMonitor>("com.magmacompany", 1, 0, "SystemMonitor", [](QQmlEngine *, QJSEngine*) -> QObject* {
        return new SystemMonitor;
    });
    qmlRegisterSingletonType<Collection>("com.magmacompany", 1, 0, "Collection", [](QQmlEngine *, QJSEngine*) -> QObject* {
        return new Collection;
    });
    qmlRegisterSingletonType<Settings>("com.magmacompany", 1, 0, "Settings", [](QQmlEngine *, QJSEngine*) -> QObject* {
        return new Settings;
    });

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

#ifdef Q_PROCESSOR_ARM
    app.exec();
    QImage *fb = EPFrameBuffer::instance()->framebuffer();
    fb->fill(Qt::white);
    QPainter painter(fb);
    QImage splashScreen("/data/shutdown.jpg");
    painter.drawImage(0, 0, splashScreen);
    EPFrameBuffer::instance()->sendUpdate(fb->rect(), EPFrameBuffer::Grayscale, EPFrameBuffer::FullUpdate, true);
    QProcess::execute("/system/bin/reboot", QStringList("-p"));
    return 0;
#else
    return app.exec();
#endif
}

