#include "drawingarea.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <errno.h>
#include <QLine>
#include "digitizer.h"
#include "mxcfb.h"
#include <unistd.h>
#include <QPainter>

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
    painter->fillRect(0, 0, width(), height(), Qt::white);
}

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

void DrawingArea::drawAAPixel(uchar *address, double distance, bool aa)
{
    if (address >= s_framebufferMem +
            s_fbVarInfo.xres * s_fbVarInfo.yres * s_fbVarInfo.bits_per_pixel / 8) {
//        //printf("overflow in painting by %ld bytes\n", (offset - s_last_byte));
        return;
    }
    if (address <= s_framebufferMem) {
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

void DrawingArea::drawAALine(const QLine &line, bool aa)
{
    QRect dispRect(0, 0, s_fbVarInfo.xres, s_fbVarInfo.yres);
    if (!dispRect.contains(line.p1()) || !dispRect.contains(line.p2())) {
        return;
    }

    uchar *addr = s_framebufferMem;
    addr += (line.y1() * s_fbVarInfo.xres + line.x1()) * s_fbVarInfo.bits_per_pixel / 8;

    int u, v;
    int du, dv;
    int uincr, vincr;
    if ((abs(line.dx()) > abs(line.dy()))) {
        du = abs(line.dx());
        dv = abs(line.dy());
        u = line.x2();
        v = line.y2();
        uincr = s_fbVarInfo.bits_per_pixel / 8;
        vincr = s_fbVarInfo.xres * uincr;
        if (line.dx() < 0) uincr = -uincr;
        if (line.dy() < 0) vincr = -vincr;
    } else {
        du = abs(line.dy());
        dv = abs(line.dx());
        u = line.y2();
        v = line.x2();
        vincr = s_fbVarInfo.bits_per_pixel / 8;
        uincr = s_fbVarInfo.xres * vincr;
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



static inline uchar *getAddress(int x, int y)
{
    return s_framebufferMem + (y * s_fbVarInfo.xres + x) * s_fbVarInfo.bits_per_pixel / 8;
}

static void drawSinglePoint(int x, int y, int color)
{
    uchar *addr = getAddress(x, y);

    *addr++ = (color & 0xFF);
    *addr++ = ((color >> 8) & 0xFF);
}

#define MAX_RADIUS 10
static void drawFullPoint(int cx, int cy, int color, float size)
{
    for (int dx = 0; dx <= MAX_RADIUS; dx++) {
        for (int dy = 0; dy <= MAX_RADIUS; dy++) {
            unsigned x = cx + dx;
            unsigned y = cy + dy;
            if (x > s_fbVarInfo.xres ||
                y > s_fbVarInfo.yres) {
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

void DrawingArea::drawThinLine(QLine line, int color)
{
    QRect dispRect(0, 0, s_fbVarInfo.xres, s_fbVarInfo.yres);
    if (!dispRect.contains(line.p1()) || !dispRect.contains(line.p2())) {
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

    sendUpdate(line, 6);
}

void DrawingArea::sendUpdate(const QLine &area, int waveform)
{
    mxcfb_update_data data;

    int x1 = qMin(area.x1(), area.x2());
    int x2 = qMax(area.x1(), area.x2());
    int y1 = qMin(area.y1(), area.y2());
    int y2 = qMax(area.y1(), area.y2());
    x1 -= 12;
    x2 += 12;
    y1 -= 16;
    y2 += 16;

    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 > s_fbVarInfo.xres) x2 = s_fbVarInfo.xres;
    if (y2 > s_fbVarInfo.yres) y2 = s_fbVarInfo.yres;

    data.update_region.top = y1;
    data.update_region.left = x1;
    data.update_region.width = x2 - x1;
    data.update_region.height = y2 - y1;
    data.waveform_mode = waveform;
    data.update_mode = UPDATE_MODE_PARTIAL;
    data.update_marker = 0;
    data.temp = 24; /*TEMP_USE_AMBIENT;*/
    data.flags = 0;

    while (ioctl(s_framebufferFd, MXCFB_SEND_UPDATE, &data)) {
        if (errno == -EAGAIN) {
            qDebug() << "update retry";
            usleep(50000);
        } else {
            qWarning() << "SEND_UPDATE error: %d %s" << strerror(errno);
            return;
        }
    }
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
    do {
        drawThinLine(QLine(prevPoint.x, prevPoint.y, point.x, point.y), 0);

        prevPoint = point;
    } while (digitizer->getPoint(&point));

    digitizer->releaseLock();
    qDebug() << "unlocked digitizer";
}
