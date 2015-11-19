#include "drawingarea.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <errno.h>
#include <QLine>
#include "mxcfb.h"
#include <unistd.h>
#include <QPainter>
#include <epframebuffer.h>

static int s_framebufferFd = -1;
fb_fix_screeninfo s_fbFixedInfo;
fb_var_screeninfo s_fbVarInfo;
static uchar *s_framebufferMem = nullptr;

DrawingArea::DrawingArea() :
    m_invert(false)
{
    setAcceptedMouseButtons(Qt::LeftButton);
}

bool DrawingArea::initFb(const char *dev)
{
    qDebug() << "initializing fb";
    if (!dev) {
        qWarning() << "No framebuffer device given";
        return false;
    }

    s_framebufferFd = open(dev, O_RDWR);
    if (s_framebufferFd < 0) {
        qWarning() << "Unable to open device" << dev << ":" << strerror(errno);
        return false;
    }

    if (ioctl(s_framebufferFd, FBIOGET_FSCREENINFO, &s_fbFixedInfo) != 0) {
        qWarning() << "Unable to get fixed info from framebuffer";
        return false;
    }

    if (ioctl(s_framebufferFd, FBIOGET_VSCREENINFO, &s_fbVarInfo) != 0) {
        qWarning() << "Unable to get fixed info from framebuffer";
        return false;
    }

    QString framebufferId(s_fbFixedInfo.id);
    if (framebufferId != "mxc_epdc_fb") {
        qWarning() << "Framebuffer has wrong id:" << framebufferId;
        return false;
    }

    qDebug() << "mmaping";
    s_framebufferMem = (uchar*)mmap(0,
                            s_fbFixedInfo.smem_len,
                            (PROT_READ | PROT_WRITE),
                            MAP_SHARED,
                            s_framebufferFd,
                            0);

    if (s_framebufferMem == MAP_FAILED) {
        qWarning() << "mmap()ing framebuffer" << dev << "failed:" << strerror(errno);
        s_framebufferMem = nullptr;
        return false;
    }

    qDebug() << "Framebuffer" << framebufferId << "initialized";
    return true;
}

void DrawingArea::closeFb()
{
    if (s_framebufferMem) {
        if (munmap(s_framebufferMem, s_fbFixedInfo.smem_len) == -1) {
            qWarning() << "unmapping framebuffer failed:" << strerror(errno);
        }
    }
    if (s_framebufferFd != -1) {
        close(s_framebufferFd);
    }
}

void DrawingArea::paint(QPainter *painter)
{
    qDebug() << "drawing muh stufs" << height() << width();
    for(const QVector<PenPoint> &line : m_lines) {
        for (int i=1; i<line.length(); i++) {
            EPFrameBuffer::instance()->drawThickLine(QLine(line[i-1].x, line[i-1].y, line[i].x, line[i].y), 0, line[i].pressure);
        }
    }
//    painter->fillRect(0, 0, width(), height(), Qt::white);
}

void DrawingArea::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "Mouse event!:" << event->globalPos();
    if (!s_framebufferMem) {
        qWarning() << "Can't draw without framebuffer";
        return;
    }

    Digitizer *digitizer = Digitizer::instance();

    PenPoint prevPoint(event->globalX(), event->globalY(), 0);
    PenPoint point = digitizer->acquireLock();
    QVector<PenPoint> line;
    do {
        line.append(point);

        EPFrameBuffer::instance()->drawThickLine(QLine(prevPoint.x, prevPoint.y, point.x, point.y), 0, point.pressure);
        EPFrameBuffer::instance()->sendUpdate(QRect(prevPoint.x, prevPoint.y, point.x, point.y), EPFrameBuffer::Fast, EPFrameBuffer::PartialUpdate);

        prevPoint = point;
    } while (digitizer->getPoint(&point));

    digitizer->releaseLock();
    qDebug() << "unlocked digitizer";

    if (line.length() > 1) {
        m_lines.append(line);
    }
}
