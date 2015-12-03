#include "batterymonitor.h"
#include <QFile>
#include <QTimer>
#include <QDebug>

BatteryMonitor::BatteryMonitor(QObject *parent) : QObject(parent),
    m_batteryLeft(0)
{
    doPoll();

    QTimer *timer = new QTimer(this);
    timer->setInterval(60 * 1000); // once a minute
    timer->setSingleShot(false);

    connect(timer, SIGNAL(timeout()), SLOT(doPoll()));
    timer->start();
}

void BatteryMonitor::doPoll()
{
    QFile file("/sys/class/power_supply/max170xx_battery/capacity");

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open file with battery status:" << file.fileName();
        return;
    }

    QByteArray fileContent = file.readAll().trimmed();
    qDebug() << "battery content:" << fileContent << fileContent.toInt();

    qreal batteryPercentage = fileContent.toDouble() / 100.0;

    if (batteryPercentage == m_batteryLeft) {
        return;
    }

    m_batteryLeft = batteryPercentage;
    emit batteryChanged();
}

