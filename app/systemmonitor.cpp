#include "systemmonitor.h"
#include <stdio.h>
#ifdef Q_PROCESSOR_ARM
#include <sys/sysconf.h>
#else
#include <unistd.h>
#endif
#include <QFile>
#include <QDebug>

SystemMonitor::SystemMonitor(QObject *parent) : QObject(parent),
    m_batteryLeft(50),
    m_memoryUsed(0)
{
    // Get initial values
    doPoll();

    m_timer.setInterval(1000); // every second
    m_timer.setSingleShot(false);

    connect(&m_timer, SIGNAL(timeout()), SLOT(doPoll()));
}

void SystemMonitor::doPoll()
{
    checkBattery();
    checkMemory();
}

void SystemMonitor::checkBattery()
{
    QFile file("/sys/class/power_supply/max170xx_battery/capacity");

    if (!file.open(QIODevice::ReadOnly)) {
        //qWarning() << "Unable to open file with battery status:" << file.fileName();
        return;
    }

    QByteArray fileContent = file.readAll().trimmed();
    int batteryPercentage = fileContent.toDouble();

    if (batteryPercentage == m_batteryLeft) {
        return;
    }

    m_batteryLeft = batteryPercentage;
    emit batteryChanged();
}

void SystemMonitor::checkMemory()
{
    FILE* fp = NULL;
    if ((fp = fopen("/proc/self/statm", "r" )) == NULL) {
        qWarning() << "Unable to open statm";
        return;
    }

    long rss = 0L;
    int valuesRead = fscanf(fp, "%*s%ld", &rss);
    fclose(fp);

    if (valuesRead != 1) {
        qWarning() << "Failed to read memory usage";
        return;
    }

    int usage = (size_t(rss) * (size_t(sysconf(_SC_PAGESIZE)))) / (1024 * 1024);

    if (usage != m_memoryUsed) {
        m_memoryUsed = usage;
        emit memoryUsedChanged();
    }
}

