#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QObject>
#include <QTimer>

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
    void start() { m_timer.start(); }
    void stop() { m_timer.stop(); }

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
    QTimer m_timer;
};

#endif // SYSTEMMONITOR_H
