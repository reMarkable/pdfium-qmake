#include "batterymonitor.h"
#include <stdio.h>
#ifdef Q_PROCESSOR_ARM
#include <sys/sysconf.h>
#else
#include <unistd.h>
#endif
#include <QFile>
#include <QTimer>
#include <QDebug>

BatteryMonitor::BatteryMonitor(QObject *parent) : QObject(parent),
    m_batteryLeft(0)
{
    // Get initial values
    doPoll();

    QTimer *timer = new QTimer(this);
    timer->setInterval(60 * 1000); // once a minute
    timer->setSingleShot(false);

    connect(timer, SIGNAL(timeout()), SLOT(doPoll()));
    timer->start();
}

void BatteryMonitor::doPoll()
{
    long rss = 0L;
    FILE* fp = NULL;
    if ((fp = fopen("/proc/self/statm", "r" )) != NULL) {
        if (fscanf(fp, "%*s%ld", &rss ) == 1) {
            qDebug() << ((size_t)rss * (size_t)sysconf(_SC_PAGESIZE)) / (1024 * 1024) << "MB used";
        }
        fclose(fp);
    }

    QFile file("/sys/class/power_supply/max170xx_battery/capacity");

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open file with battery status:" << file.fileName();
        return;
    }

    QByteArray fileContent = file.readAll().trimmed();
    qreal batteryPercentage = fileContent.toDouble() / 100.0;

    if (batteryPercentage == m_batteryLeft) {
        return;
    }

    m_batteryLeft = batteryPercentage;
    emit batteryChanged();
}

