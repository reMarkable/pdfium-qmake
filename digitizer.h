#ifndef DIGITIZER_H
#define DIGITIZER_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QMutexLocker>

extern "C" {
#include <linux/input.h>
}

#define EV_BITS_LONGS (1 + EV_MAX / (8 * sizeof (long)))
#define EV_BITS_LEN (EV_BITS_LONGS * sizeof (long))


struct PenPoint {
    PenPoint(int p = 0) : x(-1), y(-1), pressure(p) {}
    int x;
    int y;
    int pressure;

    bool isValid() { return (x != -1 && y != -1); }
};

class Digitizer : public QObject
{
    Q_OBJECT
public:
    explicit Digitizer(const char *device, QObject *parent = 0);
    ~Digitizer();

    // Fastpath, blocking
    bool getPoint(PenPoint *point);

    int xResolution() { return m_xAxis.resolution; }
    int yResolution() { return m_yAxis.resolution; }

    bool isRunning() {
        QMutexLocker locker(&m_mutex);
        return m_running;
    }

signals:

public slots:
    void eventLoop();

private:

    bool processEvent(const input_event &event, PenPoint *point);

    struct AxisData {
        input_absinfo info;
        int resolution;
    };
    void initAxisData(AxisData *axisData, int axis);

    const char *m_device;
    int m_fd;
    input_id m_inputId;

    QString m_deviceName;
    unsigned long m_bitmask[EV_BITS_LONGS];

    AxisData m_xAxis;
    AxisData m_yAxis;
    AxisData m_pressureAxis;
    bool m_penOn;
    QMutex m_mutex;
    QThread m_thread;
    bool m_running;
};

#endif // DIGITIZER_H
