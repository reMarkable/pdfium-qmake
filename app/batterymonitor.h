#ifndef BATTERYMONITOR_H
#define BATTERYMONITOR_H

#include <QObject>

class BatteryMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal batteryLeft READ batteryLeft NOTIFY batteryChanged)

public:
    explicit BatteryMonitor(QObject *parent = 0);

public slots:
    qreal batteryLeft() {
        return m_batteryLeft;
    }

signals:
    void batteryChanged();

private slots:
    void doPoll();

private:
    qreal m_batteryLeft;
};

#endif // BATTERYMONITOR_H
