#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QObject>

class SystemMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int batteryLeft READ batteryLeft NOTIFY batteryChanged)
    Q_PROPERTY(int memoryUsed READ memoryUsed NOTIFY memoryUsedChanged)

public:
    explicit SystemMonitor(QObject *parent = 0);

public slots:
    int batteryLeft() {
        return m_batteryLeft;
    }
    int memoryUsed() {
        return m_memoryUsed;
    }

signals:
    void batteryChanged();
    void memoryUsedChanged();

private slots:
    void doPoll();

private:
    void checkBattery();
    void checkMemory();

    qreal m_batteryLeft;
    int m_memoryUsed;
};

#endif // SYSTEMMONITOR_H
