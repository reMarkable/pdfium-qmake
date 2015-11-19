#include "epframebuffer.h"
#include <mxcfb.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <QElapsedTimer>
#include "eprenderer.h"

EPFrameBuffer::EPFrameBuffer() : QObject()
{
    m_deviceFile.setFileName("/dev/graphics/fb0");
    if (!m_deviceFile.open(QIODevice::ReadWrite)) {
        qWarning() << "Unable to open framebuffer" << m_deviceFile.fileName();
        return;
    }
    fb_fix_screeninfo fixedInfo;
    if (ioctl(m_deviceFile.handle(), FBIOGET_FSCREENINFO, &fixedInfo) != 0) {
        qWarning() << "Unable to get fixed info from framebuffer";
        return;
    }

    fb_var_screeninfo varInfo;
    if (ioctl(m_deviceFile.handle(), FBIOGET_VSCREENINFO, &varInfo) != 0) {
        qWarning() << "Unable to get fixed info from framebuffer";
        return;
    }

    QString framebufferId(fixedInfo.id);
    if (framebufferId != "mxc_epdc_fb") {
        qWarning() << "Framebuffer has wrong id:" << framebufferId;
    }

    // Initialize EPDC
    varInfo.bits_per_pixel = 16;
    varInfo.grayscale = 0;
    varInfo.yoffset = 0;
    varInfo.activate = FB_ACTIVATE_FORCE;
    if (ioctl(m_deviceFile.handle() , FBIOPUT_VSCREENINFO, &varInfo) != 0) {
        qWarning() << "Unable to activate EPDC";
        return;
    }

    /*if (ioctl(m_deviceFile.handle() , MXCFB_SET_AUTO_UPDATE_MODE, AUTO_UPDATE_MODE_AUTOMATIC_MODE) != 0) {
        qWarning() << "Unable to set auto mode";
        return;
    }*/
/*
    int mode = UPDATE_SCHEME_QUEUE_AND_MERGE;
    while (ioctl(m_deviceFile.handle(), MXCFB_SET_UPDATE_SCHEME, &mode)) {
        if (errno == -EAGAIN) {
            qDebug() << "we need to wait for stuff to happen";
            QThread::usleep(5000);
        } else {
            qWarning() << "failed to set update scheme:" << strerror(errno);
            return;
        }
    }
*/
    uchar *fbMem = m_deviceFile.map(0, fixedInfo.smem_len);

    m_fb = QImage(fbMem, varInfo.xres, varInfo.yres, QImage::Format_RGB16);

    clearScreen();
}

EPFrameBuffer *EPFrameBuffer::instance()
{
    static EPFrameBuffer framebuffer;
    return &framebuffer;
}

void EPFrameBuffer::clearScreen()
{
    m_fb.fill(Qt::white);
    sendUpdate(m_fb.rect(), Initialize, FullUpdate, true);
}

void EPFrameBuffer::sendUpdate(QRect rect, Waveform waveform, UpdateMode mode, bool sync)
{
    rect.setWidth(rect.width() + 24);
    rect.setHeight(rect.height() + 32);
    rect.setX(rect.x() - 12);
    rect.setY(rect.y() - 16);
    rect = rect.intersected(m_fb.rect());
    if (rect.isEmpty()) {
        return;
    }

    mxcfb_update_data data;
    data.update_region.left = rect.x();
    data.update_region.top = rect.y();
    data.update_region.width = rect.width();
    data.update_region.height = rect.height();
    data.update_mode = mode;
    static int updateCounter = 1;
    data.update_marker = updateCounter++;
    data.waveform_mode = waveform;
    data.temp = 24;
    data.flags = 0;
    ioctl(m_deviceFile.handle(), MXCFB_SEND_UPDATE, &data);

    if (sync) {
        mxcfb_update_marker_data updateMarker;
        updateMarker.collision_test = 0;
        updateMarker.update_marker = data.update_marker;
        ioctl(m_deviceFile.handle(), MXCFB_WAIT_FOR_UPDATE_COMPLETE, &updateMarker);
    }
}
