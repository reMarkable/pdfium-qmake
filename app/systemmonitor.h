#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QObject>

class SystemMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal batteryLeft READ batteryLeft NOTIFY batteryChanged)

public:
    explicit SystemMonitor(QObject *parent = 0);

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

#endif // SYSTEMMONITOR_H
