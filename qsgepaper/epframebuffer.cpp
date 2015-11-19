#include "epframebuffer.h"
#include <mxcfb.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <QElapsedTimer>

EPFrameBuffer::EPFrameBuffer(EPRenderer *renderer) : QObject(renderer),
    m_renderer(renderer)
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

    connect(m_renderer, &EPRenderer::renderComplete, this, &EPFrameBuffer::draw);
}

void EPFrameBuffer::draw()
{
    if (m_fb.isNull()) {
        qWarning() << "Can't draw without a framebuffer";
    }

    QElapsedTimer timer;
    timer.start();
    QMutexLocker locker(&m_renderer->rectanglesMutex);

    QList<QRect> fastAreas;

    QMutableListIterator<EPNode*> it(m_renderer->currentRects);
    while (it.hasNext()) {
        EPNode *rect = it.next();
        if (!rect->visible) {
            fastAreas.append(rect->transformedRect);
            it.remove();
        } else if (!rect->transformedRect.intersects(m_fb.rect())) {
            it.remove();
        }
    }

    QPainter painter(&m_fb);
    painter.setBackground(Qt::white);

    for(const QRect &area : fastAreas) {
        painter.eraseRect(area);
    }
    //qDebug() << fastAreas.count();

    // Rects are sorted in z-order
    for(EPNode *rect : m_renderer->currentRects) {
        if (rect->dirty) {
            //painter.fillRect(rect->transformedRect, Qt::Dense2Pattern);
            rect->draw(&painter);
            //painter.drawRect(rect->transformedRect);
            //painter.drawText(rect->transformedRect.bottomLeft(), QString::number(rect->z));
            //painter.drawImage(rect.geometry, rect.content);
            rect->dirty = false;
            //painter.drawLine(rect->transformedRect.topLeft(), rect->transformedRect.bottomRight());
            fastAreas.append(rect->transformedRect);
            continue;
        }

        for (const QRect &area : fastAreas) {
            if (rect->transformedRect == m_fb.rect()) {
                qDebug() << "Skipping background rect?" << rect->transformedRect;
                continue;
            }
            if (rect->transformedRect.intersects(area)) {
                //painter.fillRect(rect->transformedRect, Qt::DiagCrossPattern);
                //painter.drawText(rect->transformedRect.topLeft(), QString::number(rect->z));
                //painter.drawRect(rect->transformedRect);
                rect->draw(&painter);
                //painter.drawLine(rect->transformedRect.topLeft(), rect->transformedRect.bottomRight());
                //painter.drawImage(rect.geometry, rect.content);
                fastAreas.append(rect->transformedRect);
                break;
            }
        }
    }

    locker.unlock();
    painter.end();

    //qDebug() << Q_FUNC_INFO << timer.restart() << "painted";

#if 0
    /*QList<EpaperRect>::iterator rect = m_renderer->currentRects.begin();
    while (rect != m_renderer->currentRects.end()) {
        if (rect->dirty) {
            sendUpdate(rect->geometry, Fast, PartialUpdate);
            rect->dirty = false;
        }
        rect++;
    }*/
    // FIXME: do something more clever here plz
    for (int i=0; i<fastAreas.size(); i++) {
        if (fastAreas[i].isEmpty()) {
            continue;
        }
        for (int j=0; j<fastAreas.size(); j++) {
            if (i==j) continue;
            if (fastAreas[i].intersects(fastAreas[j])) {
                fastAreas[i] = fastAreas[i].united(fastAreas[j]);
                fastAreas[j] = QRect();
            }
        }
    }

    qDebug() << "count" << damagedAreas.count();
    if (damagedAreas.count() > 20) {
        sendUpdate(m_fb.rect(), Grayscale, PartialUpdate, true);
    } else {
        int i = 0;
        for(const QRect &area : fastAreas) {
            if (area.isEmpty()) {
                continue;
            }
            sendUpdate(area, Fast, PartialUpdate);
            i++;
        }
        qDebug() << "damaged areas:" << i;
    }

#else
    sendUpdate(m_fb.rect(), Grayscale, FullUpdate, true);
#endif

    //qDebug() << Q_FUNC_INFO << timer.restart() << "updated";
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


