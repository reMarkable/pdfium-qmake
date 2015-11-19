#ifndef DIGITIZER_H
#define DIGITIZER_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QMutexLocker>
#include <QSize>

extern "C" {
#include <linux/input.h>
}

#define EV_BITS_LONGS (1 + EV_MAX / (8 * sizeof (long)))
#define EV_BITS_LEN (EV_BITS_LONGS * sizeof (long))


struct PenPoint {
    PenPoint() : x(-1), y(-1), pressure(-1) {}
    PenPoint(int nx, int ny, int p) : x(nx), y(ny), pressure(p) {}
    int x;
    int y;
    int pressure;

    bool isValid() { return (x != -1 && y != -1); }
};

class Digitizer : public QObject
{
    Q_OBJECT
public:
    ~Digitizer();

    //////////////////
    // Fastpath, blocking
    PenPoint acquireLock() {
        m_reportLock.lock();
        m_readLock.lock();
        PenPoint currentPoint = m_lastPoint;
        m_lastPoint = PenPoint();
        return currentPoint;
    }
    bool getPoint(PenPoint *point);
    void releaseLock() {
        m_readLock.unlock();
        m_reportLock.unlock();
    }
    //////////////

    int xResolution() { return m_xAxis.resolution; }
    int yResolution() { return m_yAxis.resolution; }

    bool isRunning() {
        QMutexLocker locker(&m_readLock);
        return m_running;
    }

    static Digitizer *instance();
    static bool initialize(const char *device);

signals:

private slots:
    void eventLoop();

private:
    explicit Digitizer(const char *device, QObject *parent = 0);

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
    QMutex m_readLock;
    QMutex m_reportLock;
    QThread m_thread;
    bool m_running;
    QSize m_displaySize;
    PenPoint m_lastPoint;
};

#endif // DIGITIZER_H
