#include "drawingarea.h"
#include <QLine>
#include <QPainter>
#include <epframebuffer.h>

DrawingArea::DrawingArea() :
    m_invert(false),
    m_currentBrush(Paintbrush)
{
    m_contents = QImage(1600, 1200, QImage::Format_RGB16);
    m_contents.fill(Qt::white);
    setAcceptedMouseButtons(Qt::LeftButton);
}

void DrawingArea::paint(QPainter *painter)
{
    painter->drawImage(QRect(0, 0, 1560, 1200), m_contents, QRect(0, 0, 1560, 1200));
}


static void drawAAPixel(QImage *fb, uchar *address, double distance, bool aa, bool invert)
{
    if (address >= fb->bits() + fb->byteCount()) {
//        //printf("overflow in painting by %ld bytes\n", (offset - s_last_byte));
        return;
    }
    if (address <= fb->bits()) {
//        //printf("underflow in painting by %ld bytes\n", (s_first_byte - offset));
        return;
    }

    double normalized = (distance * 2.0/3.0);
    normalized = pow(normalized, 2);
    if (invert) {
        normalized = 1.0 - normalized;
    }

    int col = normalized * 16;
    col *= 16;
    if (invert) {
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

static void drawAALine(QImage *fb, const QLine &line, bool aa, bool invert)
{
    if (!fb->rect().contains(line.p1()) || !fb->rect().contains(line.p2())) {
        return;
    }

    int bytesPerPixel = fb->bytesPerLine() / fb->width();

    uchar *addr = fb->scanLine(line.y1()) + line.x1() * bytesPerPixel;

    int u, v;
    int du, dv;
    int uincr, vincr;
    if ((abs(line.dx()) > abs(line.dy()))) {
        du = abs(line.dx());
        dv = abs(line.dy());
        u = line.x2();
        v = line.y2();
        uincr = bytesPerPixel;
        vincr = fb->bytesPerLine();
        if (line.dx() < 0) uincr = -uincr;
        if (line.dy() < 0) vincr = -vincr;
    } else {
        du = abs(line.dy());
        dv = abs(line.dx());
        u = line.y2();
        v = line.x2();
        vincr = bytesPerPixel;
        uincr = fb->bytesPerLine();
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
        drawAAPixel(fb, addr, twovdu*invD, aa, invert);
        drawAAPixel(fb, addr + vincr, invD2du - twovdu*invD, aa, invert);
        drawAAPixel(fb, addr - vincr, invD2du + twovdu*invD, aa, invert);

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

void DrawingArea::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "Mouse event!:" << event->globalPos();

    Digitizer *digitizer = Digitizer::instance();

    PenPoint prevPoint(event->globalX(), event->globalY(), 0);
    PenPoint point = digitizer->acquireLock();
    //QVector<PenPoint> line;

    QPainter painter(EPFrameBuffer::instance()->framebuffer());
    QPainter selfPainter(&m_contents);
    QPen thickPen(Qt::black);
    thickPen.setCapStyle(Qt::RoundCap);

    do {
        //line.append(point);
        QLine line(prevPoint.x, prevPoint.y, point.x, point.y);

        switch(m_currentBrush) {
        case Paintbrush: {
            qreal pointsize = point.pressure * point.pressure * 10.0;
            pointsize -= (fabs(line.dx()) + fabs(line.dy())) / 10.0;
            if (pointsize < 2) pointsize = 2;
            thickPen.setWidthF(pointsize);
            painter.setPen(thickPen);
            painter.drawLine(line);
            selfPainter.setPen(thickPen);
            selfPainter.drawLine(line);
            break;
        }
        case Pencil:
            painter.drawLine(line);
            selfPainter.drawLine(line);
            break;
        case Pen:
            drawAALine(EPFrameBuffer::instance()->framebuffer(), line, false, false);
            drawAALine(&m_contents, line, false, false);
            drawAALine(&m_contents, line, true, false);
            break;
        }

        EPFrameBuffer::instance()->sendUpdate(QRect(prevPoint.x, prevPoint.y, point.x, point.y), EPFrameBuffer::Fast, EPFrameBuffer::PartialUpdate);

        prevPoint = point;
    } while (digitizer->getPoint(&point));

    digitizer->releaseLock();
    qDebug() << "unlocked digitizer";
}

