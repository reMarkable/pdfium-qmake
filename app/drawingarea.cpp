#include "drawingarea.h"
#include <QLine>
#include <QPainter>
#include <QElapsedTimer>

#ifdef Q_PROCESSOR_ARM
#include <epframebuffer.h>
#endif

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

inline double minDistance(const QLine &a, const QLine &b)
{
    int minDistSquared =                  QPoint::dotProduct(a.p1(), b.p1());
    minDistSquared = qMin(minDistSquared, QPoint::dotProduct(a.p1(), b.p2()));
    minDistSquared = qMin(minDistSquared, QPoint::dotProduct(a.p2(), b.p1()));
    minDistSquared = qMin(minDistSquared, QPoint::dotProduct(a.p2(), b.p2()));

    return sqrt(minDistSquared);
}

void DrawingArea::mousePressEvent(QMouseEvent *event)
{
#ifdef Q_PROCESSOR_ARM
    qDebug() << "Mouse event!:" << event->globalPos();

    Digitizer *digitizer = Digitizer::instance();

    PenPoint prevPoint(event->globalX(), event->globalY(), 0);
    PenPoint point = digitizer->acquireLock();


    QPainter painter(EPFrameBuffer::instance()->framebuffer());
    QPainter selfPainter(&m_contents);
    QPen thickPen(Qt::black);
    thickPen.setCapStyle(Qt::RoundCap);

    // For drawing AA lines
    int skippedUpdatesCounter = 0;
    QRect delayedUpdateRect;
    QVector<QLine> queuedLines;
    QElapsedTimer lutTimer;
    int freeLuts = 1;


    Predictor xPredictor;
    Predictor yPredictor;
    do {
        point.x = xPredictor.getPrediction(point.x);
        point.y = yPredictor.getPrediction(point.y);

        QLine line(prevPoint.x, prevPoint.y, point.x, point.y);
        QRect updateRect(line.p1(), line.p2());

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
            EPFrameBuffer::instance()->sendUpdate(updateRect, EPFrameBuffer::Fast, EPFrameBuffer::PartialUpdate);
            break;
        }

        case Pencil:
            painter.drawLine(line);
            selfPainter.drawLine(line);
            EPFrameBuffer::instance()->sendUpdate(updateRect, EPFrameBuffer::Fast, EPFrameBuffer::PartialUpdate);
            break;

        case Pen: {
            drawAALine(&m_contents, line, false, m_invert);
            drawAALine(&m_contents, line, true, m_invert);

            drawAALine(EPFrameBuffer::instance()->framebuffer(), line, false, m_invert);

            // Because we use DU, we only have 16 LUTs available, and therefore need to batch
            // up updates we send
            if (skippedUpdatesCounter > 2) {
                EPFrameBuffer::instance()->sendUpdate(delayedUpdateRect, EPFrameBuffer::Fast, EPFrameBuffer::PartialUpdate);
                skippedUpdatesCounter = 0;
            }


            if (skippedUpdatesCounter == 0) {
                delayedUpdateRect = updateRect;
            } else {
                delayedUpdateRect = delayedUpdateRect.united(updateRect);
            }
            skippedUpdatesCounter++;

            // Do delayed updates for drawing with DU
            queuedLines.append(line);

            // Try to do semi-intelligently handling of LUT usage for DUs
            if (freeLuts < 1 && lutTimer.elapsed() > 250) {
                freeLuts++;
            }

            if (freeLuts < 1) {
                break;
            }

            // Start looping over stored lines to see if we can do AA drawing on some of them
            QMutableVectorIterator<QLine> it(queuedLines);
            updateRect = QRect();
            while(it.hasNext()) {
                QLine oldLine = it.next();

                // Avoid overlapping with the rect we just updated
                QRect testRect = updateRect.united(QRect(oldLine.p1(), oldLine.p2()));
                testRect.setX(testRect.x() - 12);
                testRect.setY(testRect.y() - 16);
                testRect.setWidth(testRect.width() + 24);
                testRect.setHeight(testRect.height() + 32);

                // Calculate minimal distance
                int distanceX =             abs(oldLine.x1() - line.x1());
                distanceX = qMin(distanceX, abs(oldLine.x2() - line.x1()));
                distanceX = qMin(distanceX, abs(oldLine.x1() - line.x2()));
                distanceX = qMin(distanceX, abs(oldLine.x2() - line.x2()));

                int distanceY =             abs(oldLine.y1() - line.y1());
                distanceY = qMin(distanceY, abs(oldLine.y2() - line.y1()));
                distanceY = qMin(distanceY, abs(oldLine.y1() - line.y2()));
                distanceY = qMin(distanceY, abs(oldLine.y2() - line.y2()));

                if (hypot(distanceX, distanceY) < 1000 * hypot(xPredictor.trendDelta, yPredictor.trendDelta) ||
                        testRect.contains(line.p1()) ||
                        testRect.contains(line.p2())) {
                    continue;
                }

                // Re-draw line with AA pixels
                it.remove();
                drawAALine(EPFrameBuffer::instance()->framebuffer(), oldLine, true, m_invert);
                updateRect = updateRect.united(QRect(oldLine.p1(), oldLine.p2()));
            }

            if (updateRect.isEmpty()) {
                break;
            }

            // If we're going to do an update (expensive), do all the (cheap) drawings as well
            it.toFront();
            while (it.hasNext()) {
                QLine queuedLine = it.next();
                if (updateRect.contains(queuedLine.p1()) && updateRect.contains(queuedLine.p2())) {
                    drawAALine(EPFrameBuffer::instance()->framebuffer(), queuedLine, true, m_invert);
                    it.remove();
                }
            }
            freeLuts--;
            lutTimer.restart();
            EPFrameBuffer::instance()->sendUpdate(updateRect, EPFrameBuffer::Grayscale, EPFrameBuffer::PartialUpdate);

            break;
        }
        }

        prevPoint = point;
    } while (digitizer->getPoint(&point));

    digitizer->releaseLock();

    // Check if we have queued AA lines to draw
    if (m_currentBrush != Pen || queuedLines.isEmpty()) {
        return;
    }

    if (skippedUpdatesCounter > 0) {
        EPFrameBuffer::instance()->sendUpdate(delayedUpdateRect, EPFrameBuffer::Fast, EPFrameBuffer::PartialUpdate, true);
    }

    QRect updateRect;
    for (const QLine &line : queuedLines) {
        drawAALine(EPFrameBuffer::instance()->framebuffer(), line, true, m_invert);
        updateRect = updateRect.united(QRect(line.p1(), line.p2()));
    }
    EPFrameBuffer::instance()->sendUpdate(updateRect, EPFrameBuffer::Grayscale, EPFrameBuffer::PartialUpdate);

    qDebug() << "unlocked digitizer";
#endif
}

