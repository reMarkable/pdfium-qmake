#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QQuickPaintedItem>
#include <QImage>
#include <QElapsedTimer>
#include "line.h"
#include "documentworker.h"
#include <QPointer>

#ifdef Q_PROCESSOR_ARM
#include <epframebuffer.h>
#endif

class DrawingArea : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(Line::Brush currentBrush MEMBER m_currentBrush NOTIFY currentBrushChanged)
    Q_PROPERTY(bool zoomtoolSelected MEMBER m_zoomSelected NOTIFY zoomtoolSelectedChanged)
    Q_PROPERTY(qreal zoomFactor READ zoomFactor NOTIFY zoomFactorChanged)
    Q_PROPERTY(Line::Color currentColor MEMBER m_currentColor NOTIFY currentColorChanged)
    Q_PROPERTY(Document* document WRITE setDocument MEMBER m_document)
    Q_PROPERTY(bool predictionEnabled MEMBER m_predict NOTIFY predictionToggled)
    Q_PROPERTY(bool doublePredictionEnabled MEMBER m_doublePredict NOTIFY doublePredictionChanged)
    Q_PROPERTY(int smoothFactor MEMBER m_smoothFactor NOTIFY smoothFactorChanged)

public:
    DrawingArea();
    ~DrawingArea();

    void paint(QPainter *painter) override;

public slots:
    void clear();
    void undo();
    void redo();
    void setZoom(double x, double y, double width, double height);
    qreal zoomFactor() { return m_zoomFactor; }

    void setDocument(Document *document);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void itemChange(ItemChange change, const ItemChangeData &value) override;

signals:
    void currentBrushChanged();
    void zoomtoolSelectedChanged();
    void zoomFactorChanged();
    void currentColorChanged();
    void predictionToggled();
    void smoothFactorChanged();
    void doublePredictionChanged();

private slots:
    void redrawBackbuffer(QRectF part = QRectF());
    void onBackgroundChanged();

private:
    void drawBackground(QPainter *painter, const QRectF part);
    void doZoom();
    QRectF lineBoundingRect(const QPointF &point1, const QPointF &point2);
    QRectF drawLine(QPainter *painter, const Line::Brush brush, const Line::Color color, const QPointF &point , const QPointF &prevPoint, qreal pressure);

#ifdef Q_PROCESSOR_ARM
    void sendUpdate(QRectF rect, const EPFrameBuffer::Waveform waveform, bool blocking = false);
#endif

    bool m_invert;
    Line::Brush m_currentBrush;
    bool m_hasEdited;
    QList<Line> m_undoneLines;
    double m_zoomFactor;
    QRectF m_zoomRect;
    bool m_zoomSelected;
    Line::Color m_currentColor;
    QPointer<DocumentWorker> m_documentWorker;
    bool m_predict;
    int m_smoothFactor;
    bool m_doublePredict;
    QTransform m_lastTransform;
    Document *m_document;
    QElapsedTimer m_drawTimer;
};

#endif // DRAWINGAREA_H
