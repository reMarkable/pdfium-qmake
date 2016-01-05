#include "drawingarea.h"
#include <QDebug>
#include <QLine>
#include <QPainter>
#include <QElapsedTimer>
#include <QThread>
#include <QTimer>
#include <QPolygonF>

#include "predictor.h"

DrawingArea::DrawingArea() :
    m_invert(false),
    m_currentBrush(Page::Paintbrush),
    m_hasEdited(false),
    m_zoomFactor(1.0),
    m_zoomRect(0, 0, 1.0, 1.0),
    m_zoomSelected(false)
{
    setAcceptedMouseButtons(Qt::LeftButton);
}

void DrawingArea::paint(QPainter *painter)
{
    if (m_contents.isNull() || (m_page && m_page->background().isNull() && m_page->hasBackground())) {
        painter->setPen(Qt::black);
        painter->drawText(contentsBoundingRect().center(), "Loading...");
        return;
    }
    QElapsedTimer timer;
    timer.start();
    painter->drawImage(0, 0, m_contents);
    if (timer.elapsed() > 75) {
        qDebug() << Q_FUNC_INFO << "drawing done in" << timer.elapsed() << "ms";
    }
}

void DrawingArea::clear()
{
    // If the user just reclicks without drawing more, we assume he wants to clear out some ghosting
    if (!m_hasEdited) {
#ifdef Q_PROCESSOR_ARM
        EPFrameBuffer::instance()->sendUpdate(EPFrameBuffer::instance()->framebuffer()->rect(),
                                              EPFrameBuffer::Grayscale,
                                              EPFrameBuffer::FullUpdate);
#endif
    } else {
        m_contents.fill(Qt::white);
        update();
        m_hasEdited = false;
        m_undoneLines.clear();
        m_lines.append(Page::Line()); // empty dummy for undoing
    }
}

void DrawingArea::undo()
{
    if (m_lines.isEmpty()) {
        return;
    }

    QElapsedTimer timer;
    timer.start();

    m_undoneLines.append(m_lines.takeLast());

    redrawBackbuffer();

    m_hasEdited = true;
    qDebug() << Q_FUNC_INFO << "Undo completed in" << timer.elapsed();
    update();
}

void DrawingArea::redo()
{
    if (m_undoneLines.isEmpty()) {
        return;
    }

    m_lines.append(m_undoneLines.takeLast());
    redrawBackbuffer();
    m_hasEdited = true;
    update();
}

void DrawingArea::setZoom(double x, double y, double width, double height)
{
    if (width == 0 || height == 0) {
        qDebug() << "invalid zoom specified";
        return;
    }

    m_zoomRect = QRectF(x, y, width, height);
    m_zoomFactor = qMax(1.0/width, 1.0/height);
    emit zoomFactorChanged();
    redrawBackbuffer();
    update();
}

void DrawingArea::setPage(Page *page)
{
    m_page = page;
    if (!page) {
        return;
    }
    m_lines = page->lines();
    m_undoneLines.clear();

    redrawBackbuffer();
    connect(page, SIGNAL(backgroundLoaded()), SLOT(redrawBackbuffer()));
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
        if (fb->format() == QImage::Format_ARGB32_Premultiplied) {
            QRgb *pix = (QRgb*)address;
            *pix = qRgb(col, col, col);
        } else {
            col = (((col >> 3) & 0x001F) | ((col << 3) & 0x07E0) | ((col << 8) & 0xF800));
            *address++ &= (col >> 8) & 0xff;
            *address++ &= col & 0xff;
        }
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

#define SMOOTHFACTOR_P 0.370
#define TRENDFACTOR_P 0.245

void DrawingArea::mousePressEvent(QMouseEvent *event)
{
    if (m_zoomSelected) {
        doZoom();
        m_zoomSelected = false;
        emit zoomtoolSelectedChanged();
        return;
    }
    m_hasEdited = true;

#ifdef Q_PROCESSOR_ARM
    QThread::currentThread()->setPriority(QThread::HighestPriority);

    Digitizer *digitizer = Digitizer::instance();

    PenPoint point;
    PenPoint prevPoint = digitizer->acquireLock();

    QPainter painter(EPFrameBuffer::instance()->framebuffer());
    QPainter selfPainter(&m_contents);
    QPen pen(Qt::black);
    pen.setCapStyle(Qt::RoundCap);

    // For drawing AA lines
    int skippedUpdatesCounter = 0;
    QRect delayedUpdateRect;
    QVector<QLine> queuedLines;
    QElapsedTimer lutTimer;
    int freeLuts = 1;

    if (m_currentBrush == Page::Eraser) {
        pen.setWidthF(10 * m_zoomFactor);
        pen.setColor(Qt::white);
        painter.setPen(pen);
        selfPainter.setPen(pen);
    }

    if (m_currentBrush == Page::Pen &&  (prevPoint.x * 1600 != event->globalX() || prevPoint.y * 1200 != event->globalY())) {
        QLine line(prevPoint.x * 1600, prevPoint.y * 1200, event->globalX(), event->globalY());
        QRect updateRect(line.p1(), line.p2());
        drawAALine(&m_contents, line, false, m_invert);
        drawAALine(EPFrameBuffer::instance()->framebuffer(), line, false, m_invert);
        sendUpdate(updateRect, EPFrameBuffer::Mono);
    }

    Page::Line drawnLine;
    drawnLine.brush = m_currentBrush;
    drawnLine.points.append(PenPoint(prevPoint.x * m_zoomRect.width() + m_zoomRect.x(),
                                     prevPoint.y * m_zoomRect.height() + m_zoomRect.y(),
                                     prevPoint.pressure));

    double pressureTrend = -1;

    Predictor xPredictor;
    Predictor yPredictor;
    while (digitizer->getPoint(&point)) {
        point.x = xPredictor.getPrediction(point.x);
        point.y = yPredictor.getPrediction(point.y);

        if (pressureTrend == -1) {
            pressureTrend = point.pressure - prevPoint.pressure;
        } else {
            point.pressure = SMOOTHFACTOR_P * point.pressure + (1.0 - SMOOTHFACTOR_P) * (prevPoint.pressure + pressureTrend);
            pressureTrend = TRENDFACTOR_P * (point.pressure - prevPoint.pressure) + (1.0 - TRENDFACTOR_P) * pressureTrend;
        }

        const QLine line(prevPoint.x * 1600, prevPoint.y * 1200, point.x * 1600, point.y * 1200);
        QRect updateRect(line.p1(), line.p2());

        switch(m_currentBrush) {
        case Page::Paintbrush: {
            qreal pointsize = point.pressure * point.pressure * 10.0;
            pointsize -= (fabs(line.dx()) + fabs(line.dy())) / 10.0;
            if (pointsize < 2) pointsize = 2;
            pen.setWidthF(pointsize * m_zoomFactor);
            painter.setPen(pen);
            painter.drawLine(line);
            selfPainter.setPen(pen);
            selfPainter.drawLine(line);
            sendUpdate(updateRect, EPFrameBuffer::Mono);
            break;
        }

        case Page::Eraser:
            painter.drawLine(line);
            selfPainter.drawLine(line);
            sendUpdate(updateRect, EPFrameBuffer::Mono);
            break;

        case Page::Pencil:
            pen.setWidthF(m_zoomFactor);
            painter.setPen(pen);
            painter.drawLine(line);
            selfPainter.setPen(pen);
            selfPainter.drawLine(line);
            sendUpdate(updateRect, EPFrameBuffer::Mono);
            break;

        case Page::Pen: {
            drawAALine(&m_contents, line, false, m_invert);
            drawAALine(EPFrameBuffer::instance()->framebuffer(), line, false, m_invert);

            // Because we use DU, we only have 16 LUTs available, and therefore need to batch
            // up updates we send
            if (skippedUpdatesCounter > 2) {
                sendUpdate(delayedUpdateRect, EPFrameBuffer::Mono);
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

                //qDebug() << hypot(distanceX, distanceY) << hypot(xPredictor.trendDelta * 1600, yPredictor.trendDelta * 1200);
                if (hypot(distanceX, distanceY) < 80 * hypot(xPredictor.trendDelta * 1600, yPredictor.trendDelta * 1200) ||
                        testRect.contains(line.p1()) ||
                        testRect.contains(line.p2())) {
                    continue;
                }

                // Re-draw line with AA pixels
                it.remove();
                drawAALine(EPFrameBuffer::instance()->framebuffer(), oldLine, true, m_invert);
                drawAALine(&m_contents, oldLine, true, m_invert);
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
                    drawAALine(&m_contents, queuedLine, true, m_invert);
                    it.remove();
                }
            }
            freeLuts--;
            lutTimer.restart();
            sendUpdate(updateRect, EPFrameBuffer::Grayscale);

            break;
        }
        default:
            break;
        }


        drawnLine.points.append(PenPoint(point.x * m_zoomRect.width() + m_zoomRect.x(),
                                         point.y * m_zoomRect.height() + m_zoomRect.y(),
                                         point.pressure));

        prevPoint = point;
    }

    digitizer->releaseLock();

    m_undoneLines.clear();
    if (m_page) {
        m_page->addLine(drawnLine);
    }
    m_lines.append(drawnLine);

    // Check if we have queued AA lines to draw
    if (m_currentBrush != Page::Pen || queuedLines.isEmpty()) {
        return;
    }

    if (skippedUpdatesCounter > 0) {
        sendUpdate(delayedUpdateRect, EPFrameBuffer::Mono);
    }

    QRect updateRect;
    for (const QLine &line : queuedLines) {
        drawAALine(EPFrameBuffer::instance()->framebuffer(), line, true, m_invert);

        updateRect = updateRect.united(QRect(line.p1(), line.p2()));
    }
    sendUpdate(updateRect, EPFrameBuffer::Grayscale);
#endif//Q_PROCESSOR_ARM
}

void DrawingArea::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (newGeometry.isValid()) {
        m_contents = QImage(newGeometry.size().toSize(), QImage::Format_RGB16);
        redrawBackbuffer();
    }
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
}

void DrawingArea::redrawBackbuffer()
{
    QElapsedTimer timer;
    timer.start();

    if (!width() || !height()) {
        // Failsafe
        return;
    }

    if (m_contents.isNull()) {
        // Probably haven't gotten the proper geometry yet
        return;
    }
    m_contents.fill(Qt::white);

    QPainter painter(&m_contents);

    if (m_page) {
        if (m_page->hasBackground() && m_page->background().isNull()) {
            // Background not loaded yet
            return;
        }

        // Draw the background image
        int backgroundHeight = m_page->background().height();
        int backgroundWidth = m_page->background().width();
        float scaleRatio = qMax(float(backgroundWidth) / float(width()),
                                float(backgroundHeight) / float(height())) / m_zoomFactor;
        backgroundHeight /= scaleRatio;
        backgroundWidth /= scaleRatio;

        float widthRatio = float(backgroundWidth) / float(width());
        float heightRatio = float(backgroundHeight) / float(height());

        int targetX = 0;
        int targetWidth = backgroundWidth;
        int targetY = 0;
        int targetHeight = backgroundHeight;

        if (widthRatio < 1) {
            const int excessWidth = width() - backgroundWidth;
            targetX = excessWidth / 2;
        } else {
            //targetWidth = width();
        }

        if (heightRatio < 1) {
            const int excessHeight = height() - backgroundHeight;
            targetY = excessHeight / 2;
        } else {
            //targetHeight = height();
        }

        painter.drawImage(QRect(targetX, targetY, targetWidth, targetHeight), m_page->background());
    }

    // Re-draw lines on top
    for (const Page::Line &drawnLine : m_lines) {
        QPen pen(Qt::black);
        pen.setCapStyle(Qt::RoundCap);

        painter.save();

        if (drawnLine.brush == Page::InvalidBrush) { // FIXME: hack for detecting clears
            m_contents.fill(Qt::white);
            continue;
        } else if (drawnLine.brush == Page::Eraser) {
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
        }

        for (int i=1; i<drawnLine.points.size(); i++) {
            QLine line((drawnLine.points[i-1].x - m_zoomRect.x()) / m_zoomRect.width() * 1600,
                       (drawnLine.points[i-1].y - m_zoomRect.y()) / m_zoomRect.height() * 1200,
                       (drawnLine.points[i].x - m_zoomRect.x()) / m_zoomRect.width() * 1600,
                       (drawnLine.points[i].y - m_zoomRect.y()) / m_zoomRect.height() * 1200);

            switch(drawnLine.brush){
            case Page::Paintbrush: {
                qreal pointsize = drawnLine.points[i].pressure * drawnLine.points[i].pressure * 10.0;
                pointsize -= (fabs(line.dx()) + fabs(line.dy())) / 10.0;
                if (pointsize < 2) pointsize = 2;
                pen.setWidthF(pointsize * m_zoomFactor);
                painter.setPen(pen);
                painter.drawLine(line);
                break;
            }
            case Page::Eraser:
                pen.setWidthF(10 * m_zoomFactor);
                pen.setColor(Qt::white);
                painter.setPen(pen);
                painter.drawLine(line);
                break;
            case Page::Pencil:
                pen.setWidthF(m_zoomFactor);
                painter.setPen(pen);
                painter.drawLine(line);
                break;
            case Page::Pen:
                drawAALine(&m_contents, line, false, m_invert);
                drawAALine(&m_contents, line, true, m_invert);
                break;
            default:
                break;
            }
        }
        painter.restore();
    }
    update();
    if (timer.elapsed() > 75) {
        qDebug() << "Redrawing backbuffer completed in" <<  timer.elapsed() << "ms";
    }
}

void DrawingArea::doZoom()
{
#ifdef Q_PROCESSOR_ARM
    Digitizer *digitizer = Digitizer::instance();

    PenPoint penPoint, prevPenPoint = digitizer->acquireLock();
    QPointF prevPoint(prevPenPoint.x * 1600, prevPenPoint.y * 1200);

    QPainter painter(EPFrameBuffer::instance()->framebuffer());
    QPen pen;
    pen.setWidth(5);
    pen.setCapStyle(Qt::RoundCap);
    pen.setBrush(Qt::Dense4Pattern);
    painter.setPen(pen);

    QPolygonF drawnLine;

    Predictor xPredictor;
    Predictor yPredictor;
    while (digitizer->getPoint(&penPoint)) {
        penPoint.x = xPredictor.getPrediction(penPoint.x);
        penPoint.y = yPredictor.getPrediction(penPoint.y);

        QPointF point(penPoint.x * 1600, penPoint.y * 1200);
        drawnLine << point;

        painter.drawLine(prevPoint, point);
        sendUpdate(QRect(prevPoint.toPoint(), point.toPoint()), EPFrameBuffer::Mono);
        prevPoint = point;
    }
    digitizer->releaseLock();

    QRectF boundingRect = drawnLine.boundingRect();
    setZoom(boundingRect.x() / 1600, boundingRect.y() / 1200, boundingRect.width() / 1600, boundingRect.height() / 1200);
#endif//Q_PROCESSOR_ARM
}

#ifdef Q_PROCESSOR_ARM
void DrawingArea::sendUpdate(QRect rect, const EPFrameBuffer::Waveform waveform)
{
    rect.setWidth(rect.width() + 24 * m_zoomFactor);
    rect.setHeight(rect.height() + 32 * m_zoomFactor);
    rect.setX(rect.x() - 12 * m_zoomFactor);
    rect.setY(rect.y() - 16 * m_zoomFactor);
    EPFrameBuffer::instance()->sendUpdate(rect, waveform, EPFrameBuffer::PartialUpdate);
}
#endif
