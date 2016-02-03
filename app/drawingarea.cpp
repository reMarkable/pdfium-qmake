#include "drawingarea.h"
#include <QDebug>
#include <QLine>
#include <QPainter>
#include <QElapsedTimer>
#include <QThread>
#include <QTimer>
#include <QPolygonF>

#include "../../../magma-experiments/dlib/filtering/kalman_filter.h"

#include "digitizer.h"
#include "predictor.h"

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
    QElapsedTimer timer;
    timer.start();
    QFile csvFile("/data/points.csv");
    if (!csvFile.open(QIODevice::WriteOnly)) {
        return;
    }
    for(const Line &line : m_document->lines()) {
        csvFile.write("0,0,0\n");
        for (const PenPoint &point : line.points) {
            csvFile.write(QByteArray::number(point.x) + ',');
            csvFile.write(QByteArray::number(point.y) + ',');
            csvFile.write(QByteArray::number(point.pressure) + '\n');
        }
        csvFile.write("0,0,0\n");
    }
    qDebug() << "Storing points completed in" << timer.elapsed() << "ms";
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
        m_document->storeDrawnPage();
    }

    QPainter painter(&m_contents);
    drawBackground(&painter);
    update();
    m_hasEdited = false;
    m_undoneLines.clear();
    if (m_document) {
        m_document->addLine(Line()); // empty dummy for undoing
    }
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
        m_document->storeDrawnPage();
    }

    m_hasEdited = true;

    QPolygon lastLine;
    foreach(const PenPoint &penPoint, m_undoneLines.last().points) {
        QPoint point((penPoint.x - m_zoomRect.x()) / m_zoomRect.width() * 1600,
                   (penPoint.y - m_zoomRect.y()) / m_zoomRect.height() * 1200);
        lastLine.append(point);
    }

    redrawBackbuffer();

    if (m_document) {
        m_document->setDrawnPage(m_contents);
    }

    //QPainter painter(EPFrameBuffer::instance()->framebuffer());
    //painter.drawImage(0, 0, m_contents);
    //sendUpdate(lastLine.boundingRect(), EPFrameBuffer::Mono);
    update(lastLine.boundingRect());
}

void DrawingArea::redo()
{
    if (m_undoneLines.isEmpty()) {
        return;
    }

    if (m_document) {
        m_document->addLine(m_undoneLines.takeLast());
        m_document->setDrawnPage(QImage());
        m_document->storeDrawnPage();
    }

    m_document->setDrawnPage(QImage());
    redrawBackbuffer();

    if (m_document) {
        m_document->setDrawnPage(m_contents);
    }

    m_hasEdited = true;
    update();
}

void DrawingArea::setZoom(double x, double y, double width, double height)
{
    if (width == 0 || height == 0) {
        qDebug() << "invalid zoom specified";
        return;
    }

    if (m_document) {
        m_document->setDrawnPage(QImage());
        m_document->storeDrawnPage();
    }

    m_zoomRect = QRectF(x, y, width, height);
    m_zoomFactor = qMax(1.0/width, 1.0/height);
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
    connect(document, SIGNAL(backgroundChanged()), SLOT(redrawBackbuffer()));
    connect(document, SIGNAL(currentIndexChanged()), SLOT(redrawBackbuffer()));
}

#define SMOOTHFACTOR_P 0.370

void DrawingArea::mousePressEvent(QMouseEvent *)
{
    if (m_zoomSelected) {
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

    if (m_currentBrush == Line::Eraser) {
        pen.setWidthF(30 * m_zoomFactor);
        pen.setColor(Qt::white);
        painter.setPen(pen);
        selfPainter.setPen(pen);
    }

    if (m_currentBrush == Line::Paintbrush) {
        if (m_currentColor == Line::White) {
            pen.setColor(Qt::white);
        } else if (m_currentColor == Line::Gray) {
            pen.setBrush(Qt::Dense4Pattern);
        }
    }

    // Store the entire drawn line
    Line drawnLine;
    drawnLine.color = m_currentColor;
    drawnLine.brush = m_currentBrush;
    drawnLine.points.append(PenPoint(prevPoint.x * m_zoomRect.width() + m_zoomRect.x(),
                                     prevPoint.y * m_zoomRect.height() + m_zoomRect.y(),
                                     prevPoint.pressure));

    if (m_currentBrush == Line::Pen) {
        pen.setWidth(3.5);
        painter.setPen(pen);
        selfPainter.setRenderHint(QPainter::Antialiasing, true);
    }

#ifdef DEBUG_PREDICTION
    PenPoint prevRealPoint = prevPoint;
    QPen debugPen;
    debugPen.setColor(Qt::black);
    debugPen.setWidth(5);
    debugPen.setBrush(Qt::Dense4Pattern);
#endif

    //// Set up kalman filter
    dlib::matrix<double> measurementNoise = m_smoothFactor * dlib::identity_matrix<double, 3>();
    dlib::matrix<double> processNoise = 0.1 * dlib::identity_matrix<double, 9>();

    // The state stores x,y,p dx,dy,dp ddx,ddy,ddp
    dlib::matrix<double, 9, 9> transitionModel;
    transitionModel =
            1, 0, 0, 1, 0, 0, 1, 0.1, 0,
            0, 1, 0, 0, 1, 0, 0, 1, 0.1,
            0, 0, 1, 0, 0, 1, 0, 0, 1,
            0, 0, 0, 1, 0, 0, 1, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 1, 0,
            0, 0, 0, 0, 0, 1, 0, 0, 1,
            0, 0, 0, 0, 0, 0, 1, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 1, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 1;

    // We only observe x,y,p
    dlib::matrix<double, 3, 9> observationModel;
    observationModel =
            1, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 1, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 1, 0, 0, 0, 0, 0, 0;

    dlib::kalman_filter<9, 3> kalmanFilter;
    kalmanFilter.set_measurement_noise(measurementNoise);
    kalmanFilter.set_process_noise(processNoise);
    kalmanFilter.set_observation_model(observationModel);
    kalmanFilter.set_transition_model(transitionModel);

    while (digitizer->getPoint(&point)) {
#ifdef DEBUG_PREDICTION
        PenPoint realPoint = point;
        painter.setPen(debugPen);
        painter.drawLine(QLine(prevRealPoint.x * 1600, prevRealPoint.y * 1200, realPoint.x * 1600, realPoint.y * 1200));
        prevRealPoint = point;
        painter.setPen(pen);
#endif

        dlib::vector<double, 3> filterInput(point.x, point.y, point.pressure);
        kalmanFilter.update(filterInput);
        dlib::matrix<double, 9, 1> kalmanPrediction;
        if (m_predict) {
            kalmanPrediction = kalmanFilter.get_predicted_next_state();
        } else if (m_doublePredict) {
            kalmanPrediction = kalmanFilter.get_predicted_next_state();
            kalmanPrediction = transitionModel * kalmanPrediction;
        } else {
            kalmanPrediction = kalmanFilter.get_current_state();
        }

        point.x = kalmanPrediction(0, 0);
        point.y = kalmanPrediction(1, 0);
        point.pressure = kalmanPrediction(2, 0);

        const QLine globalLine(mapFromScene(QPointF(prevPoint.x * 1600, prevPoint.y * 1200)).toPoint(),
                               mapFromScene(QPointF(point.x * 1600, point.y * 1200)).toPoint());
        const QLine line(prevPoint.x * 1600, prevPoint.y * 1200, point.x * 1600, point.y * 1200);


        QRect updateRect = lineBoundingRect(line);

        switch(m_currentBrush) {
        case Line::Paintbrush: {
            qreal pointsize = point.pressure * point.pressure * point.pressure * 10.0;
            pointsize -= (fabs(line.dx()) + fabs(line.dy())) / 10.0;
            if (pointsize < 2) pointsize = 2;
            pen.setWidthF(pointsize * m_zoomFactor);
            painter.setPen(pen);
            painter.drawLine(line);
            selfPainter.setPen(pen);
            selfPainter.drawLine(globalLine);
            sendUpdate(updateRect, EPFrameBuffer::Mono);
            break;
        }

        case Line::Eraser:
            painter.drawLine(line);
            selfPainter.drawLine(globalLine);
            sendUpdate(updateRect, EPFrameBuffer::Mono);
            break;

        case Line::Pencil:
            pen.setWidthF(m_zoomFactor);
            painter.setPen(pen);
            painter.drawLine(line);
            selfPainter.setPen(pen);
            selfPainter.drawLine(globalLine);
            sendUpdate(updateRect, EPFrameBuffer::Mono);
            break;

        case Line::Pen: {
            painter.setRenderHint(QPainter::Antialiasing, false);
            if (point.pressure > 0.9) {
                pen.setWidth(4);
            } else {
                pen.setWidth(3.5);
            }
            painter.setPen(pen);
            selfPainter.setPen(pen);
            painter.drawLine(line);
            painter.setRenderHint(QPainter::Antialiasing, true);
            selfPainter.drawLine(globalLine);

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
                QRect testRect = updateRect.united(lineBoundingRect(oldLine));
                testRect.setX(testRect.x() - 24);
                testRect.setY(testRect.y() - 32);
                testRect.setWidth(testRect.width() + 48);
                testRect.setHeight(testRect.height() + 36);

                // Calculate minimal distance
                int distanceX =             abs(oldLine.x1() - line.x1());
                distanceX = qMin(distanceX, abs(oldLine.x2() - line.x1()));
                distanceX = qMin(distanceX, abs(oldLine.x1() - line.x2()));
                distanceX = qMin(distanceX, abs(oldLine.x2() - line.x2()));

                int distanceY =             abs(oldLine.y1() - line.y1());
                distanceY = qMin(distanceY, abs(oldLine.y2() - line.y1()));
                distanceY = qMin(distanceY, abs(oldLine.y1() - line.y2()));
                distanceY = qMin(distanceY, abs(oldLine.y2() - line.y2()));

                if (testRect.contains(line.p1()) || testRect.contains(line.p2())) {
                    continue;
                }

                // Re-draw line with AA pixels
                it.remove();

                painter.drawLine(oldLine);
                selfPainter.drawLine(QLine(mapFromScene(oldLine.p1()).toPoint(), mapFromScene(oldLine.p2()).toPoint()));

                updateRect = updateRect.united(lineBoundingRect(oldLine));
            }

            if (updateRect.isEmpty()) {
                break;
            }

            // If we're going to do an update (expensive), do all the (cheap) drawings as well
            it.toFront();
            while (it.hasNext()) {
                QLine queuedLine = it.next();
                if (updateRect.contains(queuedLine.p1()) && updateRect.contains(queuedLine.p2())) {
                    painter.drawLine(queuedLine);
                    selfPainter.drawLine(QLine(mapFromScene(queuedLine.p1()).toPoint(), mapFromScene(queuedLine.p2()).toPoint()));
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
    if (m_document) {
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

    QRect updateRect;
    for (const QLine &line : queuedLines) {
        painter.drawLine(line);
        selfPainter.drawLine(QLine(mapFromScene(line.p1()).toPoint(), mapFromScene(line.p2()).toPoint()));

        if (!updateRect.intersects(delayedUpdateRect)) {
            updateRect = updateRect.united(lineBoundingRect(line));
        }
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
    if (m_invert) {
        m_contents.fill(Qt::black);
    } else {
        m_contents.fill(Qt::white);
    }

    if (!m_document) {
        return;
    }

    QPainter painter(&m_contents);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(&painter);

    int start = 0;
    for (int i=0; i<m_document->lines().count(); i++) {
        if (m_document->lines()[i].brush == Line::InvalidBrush) {
            start = i + 1;
        }
    }

    // Re-draw lines on top
    for (int i=start; i<m_document->lines().count(); i++) {
        const Line &drawnLine = m_document->lines()[i];
        QPen pen(Qt::black);
        pen.setCapStyle(Qt::RoundCap);

        painter.save();

        if (drawnLine.brush == Line::InvalidBrush) { // FIXME: hack for detecting clears
            qWarning() << Q_FUNC_INFO << "Impossible situation, we got invalid brush after we should be after the last";
            continue;
        } else if (drawnLine.brush == Line::Eraser) {
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
        } else if (drawnLine.brush == Line::Paintbrush) {
            switch(drawnLine.color) {
            case Line::White:
                pen.setBrush(Qt::SolidPattern);
                pen.setColor(Qt::white);
                break;
            case Line::Black:
                pen.setBrush(Qt::SolidPattern);
                pen.setColor(Qt::black);
                break;
            case Line::Gray:
                pen.setBrush(Qt::Dense4Pattern);
                break;
            }
        }

        for (int i=1; i<drawnLine.points.size(); i++) {
            QPointF pointA((drawnLine.points[i-1].x - m_zoomRect.x()) / m_zoomRect.width() * 1600,
                    (drawnLine.points[i-1].y - m_zoomRect.y()) / m_zoomRect.height() * 1200);
            QPointF pointB((drawnLine.points[i].x - m_zoomRect.x()) / m_zoomRect.width() * 1600,
                       (drawnLine.points[i].y - m_zoomRect.y()) / m_zoomRect.height() * 1200);
            QLine globalLine(mapFromScene(pointA).toPoint(), mapFromScene(pointB).toPoint());

            switch(drawnLine.brush){
            case Line::Paintbrush: {
                qreal pointsize = drawnLine.points[i].pressure * drawnLine.points[i].pressure * 10.0;
                pointsize -= (fabs(globalLine.dx()) + fabs(globalLine.dy())) / (10.0 * m_zoomFactor);
                if (pointsize < 2) pointsize = 2;
                pen.setWidthF(pointsize * m_zoomFactor);
                painter.setPen(pen);
                painter.drawLine(globalLine);
                break;
            }
            case Line::Eraser:
                pen.setWidthF(10 * m_zoomFactor);
                pen.setColor(Qt::white);
                painter.setPen(pen);
                painter.drawLine(globalLine);
                break;
            case Line::Pencil:
                pen.setWidthF(m_zoomFactor);
                painter.setPen(pen);
                painter.drawLine(globalLine);
                break;
            case Line::Pen:
                if (drawnLine.points[i].pressure > 0.8) {
                    pen.setWidth(4);
                    painter.setPen(pen);
                } else {
                    pen.setWidth(3.5);
                    painter.setPen(pen);
                }
                painter.drawLine(globalLine);

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

void DrawingArea::drawBackground(QPainter *painter)
{
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
    setZoom(boundingRect.x() / 1600, boundingRect.y() / 1200, boundingRect.width() / 1600, boundingRect.height() / 1200);
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

QRect DrawingArea::lineBoundingRect(const QLine &line)
{
    const int x1 = qMin(line.x1(), line.x2());
    const int y1 = qMin(line.y1(), line.y2());
    const int x2 = qMax(line.x1(), line.x2());
    const int y2 = qMax(line.y1(), line.y2());
    return QRect(x1, y1, x2 - x1, y2 - y1);
}

#ifdef Q_PROCESSOR_ARM
void DrawingArea::sendUpdate(QRect rect, const EPFrameBuffer::Waveform waveform, bool blocking)
{
    rect.setWidth(rect.width() + 24 * m_zoomFactor);
    rect.setHeight(rect.height() + 32 * m_zoomFactor);
    rect.setX(rect.x() - 12 * m_zoomFactor);
    rect.setY(rect.y() - 16 * m_zoomFactor);
    EPFrameBuffer::instance()->sendUpdate(rect, waveform, EPFrameBuffer::PartialUpdate, blocking);
}
#endif
