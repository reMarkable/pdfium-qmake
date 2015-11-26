#include "epframebuffer.h"

#include <mxcfb.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <QElapsedTimer>
#include <QDebug>
#include <QPainter>

EPFrameBuffer::EPFrameBuffer() : QObject(),
    m_invert(false)
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

    uchar *fbMem = m_deviceFile.map(0, fixedInfo.smem_len);

    m_fb = QImage(fbMem, varInfo.xres, varInfo.yres, QImage::Format_RGB16);
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
    // Clip to display
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
    if (sync) {
        data.update_marker = updateCounter++;
    } else {
        data.update_marker = 0;
    }
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
