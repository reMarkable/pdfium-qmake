#include "digitizer.h"

#include <QtGui/qpa/qwindowsysteminterface.h>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>

#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

static Digitizer *s_instance = nullptr;

Digitizer::Digitizer(const char *device, QObject *parent) :
    QObject(parent),
    m_device(device),
    m_fd(-1),
    m_penOn(false)
{
    m_fd = open(m_device, O_RDONLY);

    if (m_fd < 0) {
        qWarning() << "Unable to open device " << m_device << ": " << strerror(errno);
        return;
    }

    char name[255] = { 0 };
    if (ioctl(m_fd, EVIOCGNAME(255), name) < 0) {
        qWarning() << "Unable to get name from input device " << m_device;
        return;
    }
    m_deviceName = name;

    if (ioctl(m_fd, EVIOCGID, &m_inputId) < 0) {
        qWarning() << "Unable to get input id from device " << m_device;
        return;
    }

    if (ioctl(m_fd, EVIOCGBIT(0, EV_BITS_LEN), m_bitmask) < 0) {
        qWarning() << "Unable to get event bitmasks for device " << m_device;
        return;
    }

    initAxisData(&m_xAxis, ABS_X);
    if (m_xAxis.resolution == 0) {
        qWarning() << "Device " << m_device << " has no x axis";
        return;
    }

    initAxisData(&m_yAxis, ABS_Y);
    if (m_yAxis.resolution == 0) {
        qWarning() << "Device " << m_device << " has no y axis";
        return;
    }
    initAxisData(&m_pressureAxis, ABS_PRESSURE);
    if (m_pressureAxis.resolution == 0) {
        qWarning() << "Device " << m_device << " has no pressure axis";
        return;
    }

    qDebug() << "Digitizer" << m_deviceName << "initialized";

    m_displaySize = QSize(1600, 1200);//QGuiApplication::primaryScreen()->size();

    m_running = true;
    moveToThread(&m_thread);
    QMetaObject::invokeMethod(this, "eventLoop");
    m_thread.start();
}

Digitizer::~Digitizer()
{
    if (m_fd >= 0) {
        close(m_fd);
    }
    m_readLock.lock();
    m_running = false;
    m_readLock.unlock();
    m_thread.wait();
}

bool Digitizer::getPoint(PenPoint *point)
{
    if (!m_running) {
        return false;
    }

    bool done = false;
    point->x = -1;
    point->y = -1;
    point->pressure = 0;

    while (!done) {
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(m_fd, &fdset);

        if (select(m_fd + 1, &fdset, 0, 0, 0) == -1) {
            qWarning() << "Failed to poll for event from " << m_device << ": " << strerror(errno);
            return false;
        }

        input_event event;
        if (read(m_fd, &event, sizeof(input_event)) == -1) {
            qWarning() << "Failed to read event from " << m_device << ": " << strerror(errno);
            return false;
        }

        /*std::cout << "Event type "
                  << std::hex << event.type
                  << ", code: " << event.code
                  << ", value: " << event.value
                  << std::dec;*/

        if (event.type == EV_SYN && event.code == SYN_REPORT && point->isValid()) {
            return true;
        }
        processEvent(event, point);
        if (!m_penOn) {
            break;
        }
    }

    return false;
}

Digitizer *Digitizer::instance()
{
    return s_instance;
}

bool Digitizer::initialize(const char *device)
{
    if (s_instance) {
        qWarning() << "Digitizer already initialized!";
        return false;
    }

    s_instance = new Digitizer(device);
    if (!s_instance->isRunning()) {
        qWarning() << "Failed to initialize digitizer!";
        delete s_instance;
        s_instance = nullptr;
        return false;
    }

    return true;
}

void Digitizer::eventLoop()
{
    PenPoint point;
    forever {
        QMutexLocker locker(&m_readLock);

        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(m_fd, &fdset);

        if (select(m_fd + 1, &fdset, 0, 0, 0) == -1) {
            qWarning() << "Failed to poll for event from " << m_device << ": " << strerror(errno);
            break;
        }

        input_event event;
        if (read(m_fd, &event, sizeof(input_event)) == -1) {
            qWarning() << "Failed to read event from " << m_device << ": " << strerror(errno);
            break;
        }

        if (!processEvent(event, &point)) { // No report yet
            continue;
        }
        locker.unlock();

        QMutexLocker reportLocker(&m_reportLock);
        // We got a sync event, and we have a valid point
        if (point.isValid()) {
            QPointF position(point.x, point.y);

            QWindowSystemInterface::handleMouseEvent(0, // No specified window
                                                     QPointF(0, 0), // No local position
                                                     position, // Global position
                                                     m_penOn ? Qt::LeftButton : Qt::NoButton
                                                     );
        }

        if (!m_running) {
            break;
        }
    }
}

bool Digitizer::processEvent(const input_event &event, PenPoint *point)
{
    if (event.type == EV_ABS) {
        switch (event.code) {
        case ABS_X:
          //  std::cout << "absolute x: " << event.value;
            point->x = (event.value * m_displaySize.width() / xResolution());
            break;
        case ABS_Y:
          //  std::cout << "absolute y: " << event.value;
            point->y = (event.value * m_displaySize.height() / yResolution());
            break;
        case ABS_PRESSURE:
            //std::cout << "absolute pressure: " << event.value;
            point->pressure = event.value;
            break;
        default:
            qWarning() << "Unexpected absolute event code: 0x" << std::hex << event.code << std::dec;
        }
    } else if (event.type == EV_SYN) {
        //std::cout << "event synch, report?: " << (event.code == SYN_REPORT);
        if (event.code == SYN_REPORT) {
            return true;
        }
    } else if (event.type == EV_KEY) {
        //qDebug() << "key event, touch?: " << (event.code == BTN_TOUCH) << " btn0?: " << (event.code == BTN_0);
        if (event.code == BTN_TOUCH || event.code == BTN_0) {
            m_penOn = event.value;
        }
    } else if (event.type == EV_REL) {
        qWarning() << "Unexpected relative event";
    } else {
        qWarning() << "Unexpected event type "
                  << std::hex << event.type
                  << ", code: " << event.code
                  << ", value: " << event.value
                  << std::dec;
    }
    return false;
}

void Digitizer::initAxisData(Digitizer::AxisData *axisData, int axis)
{
    if (ioctl(m_fd, EVIOCGABS(axis), &axisData->info) < 0) {
        axisData->resolution = 0;
    } else {
        axisData->resolution = axisData->info.maximum - axisData->info.minimum;
    }
}
