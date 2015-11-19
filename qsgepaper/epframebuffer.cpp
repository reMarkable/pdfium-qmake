#include "epframebuffer.h"

#include <mxcfb.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <QElapsedTimer>
#include <QDebug>

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


#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

void EPFrameBuffer::drawAAPixel(uchar *address, double distance, bool aa)
{
    if (address >= m_fb.bits() + m_fb.byteCount()) {
//        //printf("overflow in painting by %ld bytes\n", (offset - s_last_byte));
        return;
    }
    if (address <= m_fb.bits()) {
//        //printf("underflow in painting by %ld bytes\n", (s_first_byte - offset));
        return;
    }

    double normalized = (distance * 2.0/3.0);
    normalized = pow(normalized, 2);
    if (m_invert) {
        normalized = 1.0 - normalized;
    }

    int col = normalized * 16;
    col *= 16;
    if (m_invert) {
        if (aa) {
            if (col > 128) {
                return;
            }
        } else {
            if (col < 128) {
                return;
            }
            col = 255;
        }
        col = (((col >> 3) & 0x001F) | ((col << 3) & 0x07E0) | ((col << 8) & 0xF800));
        *address++ |= (col >> 8) & 0xff;
        *address++ |= col & 0xff;
    } else {
        if (aa) {
            if (col < 128) {
                return;
            }
        } else {
            if (col > 128) {
                return;
            }
            col = 0;
        }
        col = (((col >> 3) & 0x001F) | ((col << 3) & 0x07E0) | ((col << 8) & 0xF800));
        *address++ &= (col >> 8) & 0xff;
        *address++ &= col & 0xff;
    }
}

void EPFrameBuffer::drawAALine(const QLine &line, bool aa)
{
    if (!m_fb.rect().contains(line.p1()) || !m_fb.rect().contains(line.p2())) {
        return;
    }

    int bytesPerPixel = m_fb.bytesPerLine() / m_fb.width();

    uchar *addr = m_fb.scanLine(line.y1()) + line.x1() * bytesPerPixel;

    int u, v;
    int du, dv;
    int uincr, vincr;
    if ((abs(line.dx()) > abs(line.dy()))) {
        du = abs(line.dx());
        dv = abs(line.dy());
        u = line.x2();
        v = line.y2();
        uincr = bytesPerPixel;
        vincr = m_fb.bytesPerLine();
        if (line.dx() < 0) uincr = -uincr;
        if (line.dy() < 0) vincr = -vincr;
    } else {
        du = abs(line.dy());
        dv = abs(line.dx());
        u = line.y2();
        v = line.x2();
        vincr = bytesPerPixel;
        uincr = m_fb.bytesPerLine();
        if (line.dy() < 0) uincr = -uincr;
        if (line.dx() < 0) vincr = -vincr;
    }

    const int uend = u + du;
    int d = (2 * dv) - du;	    /* Initial value as in Bresenham's */
    const int incrS = 2 * dv;	/* Δd for straight increments */
    const int incrD = 2 *(dv - du);	/* Δd for diagonal increments */
    int twovdu = 0;	/* Numerator of distance; starts at 0 */
    const float invD = 1.0 / (2.0*sqrt(du*du + dv*dv));   /* Precomputed inverse denominator */
    const float invD2du = 2.0 * (du*invD);   /* Precomputed constant */

    do {
        drawAAPixel(addr, twovdu*invD, aa);
        drawAAPixel(addr + vincr, invD2du - twovdu*invD, aa);
        drawAAPixel(addr - vincr, invD2du + twovdu*invD, aa);

        if (d < 0)
        {
            /* choose straight (u direction) */
            twovdu = d + du;
            d = d + incrS;
        }
        else
        {
            /* choose diagonal (u+v direction) */
            twovdu = d - du;
            d = d + incrD;
            v = v+1;
            addr = addr + vincr;
        }
        u = u+1;
        addr = addr+uincr;
    } while (u <= uend);
}



inline uchar *EPFrameBuffer::getAddress(int x, int y)
{
    return m_fb.scanLine(y) + x * 2; // 2 bytes per pixel
}

void EPFrameBuffer::drawSinglePoint(int x, int y, int color)
{
    uchar *addr = getAddress(x, y);

    *addr++ = (color & 0xFF);
    *addr++ = ((color >> 8) & 0xFF);
}

#define MAX_RADIUS 10
void EPFrameBuffer::drawFullPoint(int cx, int cy, int color, float size)
{
    for (int dx = 0; dx <= MAX_RADIUS; dx++) {
        for (int dy = 0; dy <= MAX_RADIUS; dy++) {
            unsigned x = cx + dx;
            unsigned y = cy + dy;
            if (x > m_fb.width() ||
                y > m_fb.height()) {
                continue;
            }
            uchar *pix = getAddress(x, y);
            if (hypot(dx, dy) < size) {
                *pix++ = (color & 0xFF);
                *pix++ = ((color >> 8) & 0xFF);
            } else if (hypot(dx, dy) + 1.0 < size/2 && (y + x) % 2) {
                *pix++ = (color & 0xFF);
                *pix++ = ((color >> 8) & 0xFF);
            }

        }
    }
}

void EPFrameBuffer::drawThinLine(QLine line, int color)
{
    if (!m_fb.rect().contains(line.p1()) || !m_fb.rect().contains(line.p2())) {
        return;
    }

    const int dx = line.dx();
    const int dy = line.dy();

    // Single pixel line
    if (dx == 0 && dy == 0) {
        drawSinglePoint(line.x1(), line.y1(), color);
        return;
    }

    bool vertical = (abs(dx) < abs(dy));

    int x1, y1, x2, y2;
    if ((vertical && (dy > 0)) || (!vertical && (dx > 0))) {
        x1 = line.x1();
        y1 = line.y1();
        x2 = line.x2();
        y2 = line.y2();
    } else {
        x1 = line.x2();
        y1 = line.y2();
        x2 = line.x1();
        y2 = line.y1();
    }

    if (vertical) {
        const int x0 = x1 - ((dx * y1) / dy);
        for (int x = x1, y = y1;
             y <= y2;
             x = ((dx * (++y)) / dy) + x0) {
            drawSinglePoint(x, y, color);
        }
    } else {
        const int y0 = y1 - ((dy * x1) / dx);

        for (int x = x1, y = y1;
             x <= x2;
             y = ((dy * (++x)) / dx) + y0) {
            drawSinglePoint(x, y, color);
        }
    }

    sendUpdate(QRect(line.p1(), line.p2()), Fast, PartialUpdate);
}

#if 0
void pldraw_draw_line(struct pldraw *pldraw, pldraw_color_t color,
                      const struct plep_rect *coords, float pressure, float thickness)
{
    void (* draw_point) (struct pldraw *, struct plep_point, pldraw_color_t, float);
    struct plep_rect rc;
    struct plep_point *a, *b;

    struct plep_point p;
    int dx, dy;
    int vertical;

    assert(pldraw != NULL);
    assert(coords != NULL);

    rotate_point(pldraw, &rc.a, &coords->a);
    rotate_point(pldraw, &rc.b, &coords->b);

    dx = rc.b.x - rc.a.x;
    dy = rc.b.y - rc.a.y;

    if (!dx && !dy) {
        if (is_point_in_area(&pldraw->crop_area, &rc.a))
            pldraw->set_pixel(pldraw, get_pixel_address(pldraw, &rc.a),
                      color);
        return;
    }

    vertical = (abs(dx) < abs(dy)) ? 1 : 0;

    if ((vertical && (dy > 0)) || (!vertical && (dx > 0))) {
        a = &rc.a;
        b = &rc.b;
    } else {
        a = &rc.b;
        b = &rc.a;
    }


    float pointsize = pressure * pressure * thickness;
    pointsize -= (fabs(line.dx()) + fabs(line.dy())) / thickness;
    if (pointsize < 2) pointsize = 2;
    float pointsize = 1;
    draw_point = draw_single_point;
    if (pressure > 0) {
        pointsize = pressure * pressure * thickness;
        pointsize -= (fabs(dx) + fabs(dy)) / thickness;
        if (pointsize < g_min_radius) pointsize = g_min_radius;
        draw_point = draw_full_point;
    }

    if (vertical) {
        const int x0 = a->x - ((dx * a->y) / dy);

        for (p.x = a->x, p.y = a->y;
             p.y <= b->y;
             p.x = ((dx * (++p.y)) / dy) + x0) {
            draw_point(pldraw, p, color, pointsize);
        }

    } else {
        const int y0 = a->y - ((dy * a->x) / dx);

        for (p.x = a->x, p.y = a->y;
             p.x <= b->x;
             p.y = ((dy * (++p.x)) / dx) + y0) {
            draw_point(pldraw, p, color, pointsize);
        }
    }
}
#endif


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
