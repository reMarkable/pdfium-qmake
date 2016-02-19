#include "drawingarea.h"
#include <QDebug>
#include <QLine>
#include <QPainter>
#include <QElapsedTimer>
#include <QThread>
#include <QTimer>
#include <QPolygonF>

#include <dlib/filtering/kalman_filter.h>

#include "digitizer.h"
#include "predictor.h"

#define DEBUG_THIS
#include "debug.h"

//#define DEBUG_PREDICTION

DrawingArea::DrawingArea() :
    m_invert(false),
    m_currentBrush(Line::Paintbrush),
    m_hasEdited(false),
    m_zoomFactor(1.0),
    m_zoomRect(0, 0, 1.0, 1.0),
    m_zoomSelected(false),
    m_currentColor(Line::Black),
    m_predict(true),
    m_smoothFactor(314),
    m_doublePredict(false)
{
    setAcceptedMouseButtons(Qt::LeftButton);
}

DrawingArea::~DrawingArea()
{
}

void DrawingArea::paint(QPainter *painter)
{
    if (m_contents.isNull() || (m_document && m_document->background().isNull())) {
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
    m_lastTransform = painter->transform();
}

void DrawingArea::clear()
{
    if (m_invert) {
        m_contents.fill(Qt::black);
    } else {
        m_contents.fill(Qt::white);
    }

    if (m_document) {
        m_document->setDrawnPage(QImage());
    }
    m_hasEdited = false;
    m_undoneLines.clear();
    if (m_document) {
        m_document->addLine(Line()); // empty dummy for undoing
    }

    redrawBackbuffer();

    update();
}

void DrawingArea::undo()
{
    if (!m_document || m_document->lines().isEmpty()) {
        qDebug() << "no lines to undo";
        return;
    }

    QElapsedTimer timer;
    timer.start();

    if (m_document) {
        m_undoneLines.append(m_document->popLine());
        m_document->setDrawnPage(QImage());
        //m_document->storeDrawnPage();
    }

    m_hasEdited = true;

    QPolygonF lastLine;
    foreach(const PenPoint &penPoint, m_undoneLines.last().points) {
        QPointF point((penPoint.x - m_zoomRect.x()) / m_zoomRect.width(),
                   (penPoint.y - m_zoomRect.y()) / m_zoomRect.height());
        lastLine.append(point);
    }

    QRectF updateRect = lastLine.boundingRect().marginsAdded(QMarginsF(12, 16, 12, 16));
    redrawBackbuffer(updateRect);

#ifdef Q_PROCESSOR_ARM
    QPainter painter(EPFrameBuffer::instance()->framebuffer());
    painter.setClipRect(updateRect);
    painter.setTransform(m_lastTransform);
    painter.drawImage(0, 0, m_contents);
//    sendUpdate(QRectF(0, 0, 1600, 1200), EPFrameBuffer::Grayscale);
    sendUpdate(lastLine.boundingRect(), EPFrameBuffer::Grayscale);
#endif

    if (m_document) {
        m_document->setDrawnPage(m_contents);
    }

}

void DrawingArea::redo()
{
    if (m_undoneLines.isEmpty()) {
        return;
    }

    if (m_document) {
        m_document->addLine(m_undoneLines.takeLast());
        m_document->setDrawnPage(QImage());
        //m_document->storeDrawnPage();
    }

    m_hasEdited = true;

    QPolygonF lastLine;
    foreach(const PenPoint &penPoint, m_document->lines().last().points) {
        QPointF point((penPoint.x - m_zoomRect.x()) / m_zoomRect.width(),
                   (penPoint.y - m_zoomRect.y()) / m_zoomRect.height());
        lastLine.append(point);
    }

    QRectF updateRect = lastLine.boundingRect().marginsAdded(QMarginsF(12, 16, 12, 16));
    redrawBackbuffer(updateRect);

#ifdef Q_PROCESSOR_ARM
    QPainter painter(EPFrameBuffer::instance()->framebuffer());
    painter.setClipRect(lastLine.boundingRect().marginsAdded(QMarginsF(12, 16, 12, 16)));
    painter.setTransform(m_lastTransform);
    painter.drawImage(0, 0, m_contents);
//    sendUpdate(QRectF(0, 0, 1600, 1200), EPFrameBuffer::Grayscale);
    sendUpdate(lastLine.boundingRect(), EPFrameBuffer::Grayscale);
#endif

    if (m_document) {
        m_document->setDrawnPage(m_contents);
    }
}

void DrawingArea::setZoom(double x, double y, double width, double height)
{
    if (width == 0 || height == 0) {
        qDebug() << "invalid zoom specified";
        return;
    }

    if (m_document) {
        m_document->setDrawnPage(QImage());
    }

    m_zoomRect = QRectF(x, y, width, height);
    m_zoomFactor = qMax(1600/width, 1200/height);
    emit zoomFactorChanged();
    redrawBackbuffer();
    update();
}

void DrawingArea::setDocument(Document *document)
{
    m_document = document;
    if (!document) {
        return;
    }

    m_undoneLines.clear();

    redrawBackbuffer();
    connect(document, SIGNAL(backgroundChanged()), SLOT(onBackgroundChanged()));
    connect(document, SIGNAL(currentIndexChanged()), SLOT(onBackgroundChanged()));
}

#define SMOOTHFACTOR_P 0.370

void DrawingArea::mousePressEvent(QMouseEvent *)
{
    DEBUG_BLOCK
    if (m_zoomSelected) {
        DEBUG_BLOCK
        doZoom();
        m_zoomSelected = false;
        emit zoomtoolSelectedChanged();
        return;
    }
    m_hasEdited = true;

#ifdef Q_PROCESSOR_ARM

    Digitizer *digitizer = Digitizer::instance();

    if (digitizer->buttonPressed()) {
        handleGesture();
        return;
    }

    QPainter painter(EPFrameBuffer::instance()->framebuffer());
    QPainter selfPainter(&m_contents);
    QPen pen(Qt::black);
    pen.setCapStyle(Qt::RoundCap);

    // For drawing AA lines
    int skippedUpdatesCounter = 0;
    QRectF delayedUpdateRect;

    struct LineFragment {
        LineFragment() {}
        LineFragment(QPointF p1_, QPointF p2_, qreal pressure_) : p1(p1_), p2(p2_), pressure(pressure_) {}
        QPointF p1;
        QPointF p2;
        qreal pressure;
    };
    QVector<LineFragment> queuedLines;
    QElapsedTimer lutTimer;
    int freeLuts = 1;

#ifdef DEBUG_PREDICTION
    PenPoint prevRealPoint = prevPoint;
    QPen debugPen;
    debugPen.setColor(Qt::black);
    debugPen.setWidth(5);
    debugPen.setBrush(Qt::Dense4Pattern);
#endif

    //// Set up kalman filter
    dlib::matrix<double> measurementNoise = m_smoothFactor * dlib::identity_matrix<double, 2>();
    dlib::matrix<double> processNoise = 0.1 * dlib::identity_matrix<double, 6>();

    // The state stores x,y dx,dy ddx,ddy
    dlib::matrix<double, 6, 6> transitionModel;
    transitionModel =
            1, 0, 0, 1, 0, 0,
            0, 1, 0, 0, 1, 0,
            0, 0, 1, 0, 0, 1,
            0, 0, 0, 1, 0, 0,
            0, 0, 0, 0, 1, 0,
            0, 0, 0, 0, 0, 1;

    // We only observe x,y,p
    dlib::matrix<double, 2, 6> observationModel;
    observationModel =
            1, 0, 0, 0, 0, 0,
            0, 1, 0, 0, 0, 0;

    dlib::kalman_filter<6, 2> kalmanFilter;
    kalmanFilter.set_measurement_noise(measurementNoise);
    kalmanFilter.set_process_noise(processNoise);
    kalmanFilter.set_observation_model(observationModel);
    kalmanFilter.set_transition_model(transitionModel);

    // Exclusively lock the digitizer, and get the last reported position
    PenPoint prevPenPoint = digitizer->acquireLock();
    prevPenPoint.x *= 1600;
    prevPenPoint.y *= 1200;

    // Store the entire drawn line
    Line drawnLine;
    drawnLine.color = m_currentColor;
    drawnLine.brush = m_currentBrush;
    drawnLine.points.append(PenPoint(prevPenPoint.x * m_zoomRect.width() + m_zoomRect.x(),
                                     prevPenPoint.y * m_zoomRect.height() + m_zoomRect.y(),
                                     prevPenPoint.pressure));

    PenPoint penPoint;
    while (digitizer->getPoint(&penPoint)) {
#ifdef DEBUG_PREDICTION
        PenPoint realPoint = point;
        painter.setPen(debugPen);
        painter.drawLine(QLine(prevRealPoint.x * 1600, prevRealPoint.y * 1200, realPoint.x * 1600, realPoint.y * 1200));
        prevRealPoint = point;
        painter.setPen(pen);
#endif

        // Predict/smooth the position
        dlib::vector<double, 2> filterInput(penPoint.x, penPoint.y);
        kalmanFilter.update(filterInput);
        dlib::matrix<double, 6, 1> kalmanPrediction;
        if (m_predict) {
            kalmanPrediction = kalmanFilter.get_predicted_next_state();
        } else if (m_doublePredict) {
            kalmanPrediction = kalmanFilter.get_predicted_next_state();
            kalmanPrediction = transitionModel * kalmanPrediction;
        } else {
            kalmanPrediction = kalmanFilter.get_current_state();
        }

        // Get the predicted/smoothed position, scale it up
        penPoint.x = kalmanPrediction(0, 0) * 1600;
        penPoint.y = kalmanPrediction(1, 0) * 1200;

        // Store the point in the line
        drawnLine.points.append(PenPoint(penPoint.x * m_zoomRect.width() + m_zoomRect.x(),
                                         penPoint.y * m_zoomRect.height() + m_zoomRect.y(),
                                         penPoint.pressure));


        QPointF point(penPoint.x, penPoint.y);
        QPointF prevPoint(prevPenPoint.x, prevPenPoint.y);
        prevPenPoint = penPoint;

        if (m_currentBrush != Line::Pen) {
            QRectF updateRect = drawLine(&painter, m_currentBrush, m_currentColor, point, prevPoint, penPoint.pressure);
            sendUpdate(updateRect, EPFrameBuffer::Mono);
            drawLine(&selfPainter, m_currentBrush, m_currentColor, mapFromScene(point), mapFromScene(prevPoint), penPoint.pressure);
        } else {
            selfPainter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::Antialiasing, false);
            QRectF updateRect = drawLine(&painter, m_currentBrush, m_currentColor, point, prevPoint, penPoint.pressure);
            drawLine(&selfPainter, m_currentBrush, m_currentColor, mapFromScene(point), mapFromScene(prevPoint), penPoint.pressure);

            // Because we use DU, we only have 16 LUTs available, and therefore need to batch
            // up updates we send
            if (skippedUpdatesCounter > 3) {
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
            queuedLines.append(LineFragment(point, prevPoint, penPoint.pressure));

            // Try to do semi-intelligently handling of LUT usage for DUs
            if (freeLuts < 3 && lutTimer.elapsed() > 250) {
                freeLuts++;
            }

            if (freeLuts < 1) {
                continue;
            }

            // Start looping over stored lines to see if we can do AA drawing on some of them
            QMutableVectorIterator<LineFragment> it(queuedLines);
            updateRect = QRectF();
            while(it.hasNext()) {
                const LineFragment &oldLine = it.next();

                // Avoid overlapping with the rect we just updated
                QRectF testRect = updateRect.united(lineBoundingRect(oldLine.p1, oldLine.p2));
                testRect.setX(testRect.x() - 24);
                testRect.setY(testRect.y() - 32);
                testRect.setWidth(testRect.width() + 48);
                testRect.setHeight(testRect.height() + 36);

                if (testRect.contains(point) || testRect.contains(prevPoint)) {
                    continue;
                }

                // Re-draw line with AA pixels
                it.remove();

                painter.setRenderHint(QPainter::Antialiasing, true);
                QRectF updatedRect = drawLine(&painter, m_currentBrush, m_currentColor, oldLine.p1, oldLine.p2, oldLine.pressure);
                painter.setRenderHint(QPainter::Antialiasing, false);

                updateRect = updateRect.united(updatedRect);
            }

            if (updateRect.isEmpty()) {
                continue;
            }

            // If we're going to do an update (expensive), do all the (cheap) drawings as well
            it.toFront();
            while (it.hasNext()) {
                const LineFragment &queuedLine = it.next();
                if (updateRect.contains(queuedLine.p1) && updateRect.contains(queuedLine.p2)) {
                    painter.setRenderHint(QPainter::Antialiasing, true);
                    drawLine(&painter, m_currentBrush, m_currentColor, queuedLine.p1, queuedLine.p2, queuedLine.pressure);
                    painter.setRenderHint(QPainter::Antialiasing, false);
                    it.remove();
                }
            }
            freeLuts--;
            lutTimer.restart();
            sendUpdate(updateRect, EPFrameBuffer::Grayscale);
        }
    }

    digitizer->releaseLock();

    m_undoneLines.clear();
    if (m_document) {
        DEBUG_BLOCK
        m_document->addLine(drawnLine);
        m_document->setDrawnPage(m_contents);
    } else {
        qWarning() << "Can't store line, no document set";
    }

    if (skippedUpdatesCounter > 0) {
        sendUpdate(delayedUpdateRect, EPFrameBuffer::Mono);
    }

    // Check if we have queued AA lines to draw
    if (m_currentBrush != Line::Pen || queuedLines.isEmpty()) {
        return;
    }

    QRectF updateRect;
    for (const LineFragment &line : queuedLines) {
        painter.setRenderHint(QPainter::Antialiasing, true);
        QRectF updatedRect = drawLine(&painter, m_currentBrush, m_currentColor, line.p1, line.p2, line.pressure);

        updateRect = updateRect.united(updatedRect);
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

void DrawingArea::redrawBackbuffer(QRectF part)
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

    if (!m_document) {
        return;
    }


    part = mapRectFromScene(part);
    QPainter painter(&m_contents);
    if (!part.isEmpty()) {
        painter.setClipRect(part);
    } else {
        part = m_contents.rect();
    }

    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(m_contents.rect(), Qt::white);
    drawBackground(&painter, part);

    int start = 0;
    for (int i=0; i<m_document->lines().count(); i++) {
        if (m_document->lines()[i].brush == Line::InvalidBrush) {
            start = i + 1;
        }
    }

    // Re-draw lines on top
    for (int i=start; i<m_document->lines().count(); i++) {
        const Line &drawnLine = m_document->lines()[i];

        const QVector<PenPoint> &points = drawnLine.points;
        for (int i=1; i<points.count(); i++) {

            QPointF point((points[i].x - m_zoomRect.x()) / m_zoomRect.width() * 1600,
                       (points[i].y - m_zoomRect.y()) / m_zoomRect.height() * 1200);
            QPointF prevPoint((points[i-1].x - m_zoomRect.x()) / m_zoomRect.width() * 1600,
                    (points[i-1].y - m_zoomRect.y()) / m_zoomRect.height() * 1200);

            point = mapFromScene(point);
            prevPoint = mapFromScene(prevPoint);

            if (!part.contains(point) && !part.contains(prevPoint)) {
                continue;
            }

            drawLine(&painter, drawnLine.brush, drawnLine.color, point, prevPoint, points[i].pressure);
        }
    }

    if (timer.elapsed() > 75) {
        qDebug() << "Redrawing backbuffer completed in" <<  timer.elapsed() << "ms";
    }
}

void DrawingArea::onBackgroundChanged()
{
    redrawBackbuffer();
    m_document->setDrawnPage(m_contents);
    update();
}

void DrawingArea::drawBackground(QPainter *painter, const QRectF part)
{
    Q_UNUSED(part); // FIXME

    QImage background = m_document->background();

    if (background.isNull()) {
        // Background not loaded yet
        return;
    }

    // Draw the background image
    int backgroundHeight = background.height();
    int backgroundWidth = background.width();
    float scaleRatio = qMax(float(backgroundWidth) / float(width()),
                            float(backgroundHeight) / float(height())) / m_zoomFactor;

    if (scaleRatio > 1) {
        backgroundHeight /= scaleRatio;
        backgroundWidth /= scaleRatio;
    }

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

    painter->drawImage(QRect(targetX, targetY, targetWidth, targetHeight), background);
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
    setZoom(boundingRect.x(), boundingRect.y(), boundingRect.width(), boundingRect.height());
#endif//Q_PROCESSOR_ARM
}

enum Direction {
    Invalid,
    Up,
    Right,
    Left,
    Down
};

Direction getDirection(PenPoint first, PenPoint second)
{
    const float dx = first.x - second.x;
    const float dy = first.y - second.y;
    if (fabs(dx) > fabs(dy)) {
        if (dx > 0) {
            return Up;
        } else {
            return Down;
        }
    } else {
        if (dy > 0) {
            return Right;
        } else {
            return Left;
        }
    }
}

QString directionToString(const Direction &direction)
{
    switch(direction) {
    case Up: return QStringLiteral("Up");
    case Right: return QStringLiteral("Right");
    case Left: return QStringLiteral("Left");
    case Down: return QStringLiteral("Down");
    default: return QStringLiteral("Invalid");
    }
}

void DrawingArea::handleGesture()
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

    Predictor xPredictor;
    Predictor yPredictor;

    //xPredictor.smoothFactor = 0.001;
    //yPredictor.smoothFactor = 0.001;

    qDebug() << "Gesture:";

    Direction currentDirection = Invalid, newDirection = Invalid;
    QVector<Direction> directions;
    int newDirectionCount = 0;
    while (digitizer->getPoint(&penPoint)) {
        penPoint.x = xPredictor.getPrediction(penPoint.x);
        penPoint.y = yPredictor.getPrediction(penPoint.y);

        QPointF point(penPoint.x * 1600, penPoint.y * 1200);
        Direction direction = getDirection(prevPenPoint, penPoint);

        if (direction == newDirection) {
            newDirectionCount++;
        } else {
            newDirection = direction;
            newDirectionCount = 0;
        }

        if (newDirectionCount > 3 && currentDirection != newDirection) {
            currentDirection = newDirection;
            directions.append(currentDirection);
            qDebug() << directionToString(newDirection);
        }

        painter.drawLine(prevPoint, point);
        sendUpdate(QRect(prevPoint.toPoint(), point.toPoint()), EPFrameBuffer::Mono);
        prevPoint = point;
    }

    digitizer->releaseLock();
#endif//Q_PROCESSOR_ARM
}

QRectF DrawingArea::lineBoundingRect(const QPointF &point1, const QPointF &point2)
{
    const qreal x1 = qMin(point1.x(), point2.x());
    const qreal y1 = qMin(point1.y(), point2.y());
    const qreal x2 = qMax(point1.x(), point2.x());
    const qreal y2 = qMax(point1.y(), point2.y());
    return QRect(x1, y1, x2 - x1, y2 - y1);
}

QRectF DrawingArea::drawLine(QPainter *painter, const Line::Brush brush, const Line::Color color, const QPointF &point, const QPointF &prevPoint, qreal pressure)
{
    const QLineF line(point, prevPoint);

    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    switch(brush) {
    case Line::Paintbrush: {
        qreal pointsize = pressure * pressure * 10.0;
        pointsize -= (fabs(line.dx()) + fabs(line.dy())) / 10.0;
        if (pointsize < 2) pointsize = 2;
        if (color == Line::White) {
            pen.setColor(Qt::white);
        } else if (color == Line::Gray) {
            pen.setBrush(Qt::Dense4Pattern);
        }
        pen.setWidthF(pointsize * m_zoomFactor);
        painter->setPen(pen);
        break;
    }

    case Line::Eraser:
        pen.setWidthF(30 * m_zoomFactor);
        pen.setColor(Qt::white);
        painter->setPen(pen);
        break;

    case Line::Pencil:
        pen.setWidthF(m_zoomFactor);
        painter->setPen(pen);
        break;

    case Line::Pen: {
        if (pressure > 0.9) {
            pen.setWidth(4 * m_zoomFactor);
        } else {
            pen.setWidthF(3.7 * m_zoomFactor);
        }
        painter->setPen(pen);
        break;
    }
    default:
        qWarning() << Q_FUNC_INFO << "Invalid brush given";
    }

    painter->drawLine(line);

    return lineBoundingRect(point, prevPoint);
}

#ifdef Q_PROCESSOR_ARM
void DrawingArea::sendUpdate(QRectF rect, const EPFrameBuffer::Waveform waveform, bool blocking)
{
    rect.setWidth(rect.width() + 24 * m_zoomFactor);
    rect.setHeight(rect.height() + 32 * m_zoomFactor);
    rect.setX(rect.x() - 12 * m_zoomFactor);
    rect.setY(rect.y() - 16 * m_zoomFactor);
    EPFrameBuffer::instance()->sendUpdate(rect.toRect(), waveform, EPFrameBuffer::PartialUpdate, blocking);
}
#endif
