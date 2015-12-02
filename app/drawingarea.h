#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QQuickPaintedItem>
#include <QImage>
#include "digitizer.h"

#ifdef Q_PROCESSOR_ARM
#include <epframebuffer.h>
#endif

class DrawingArea : public QQuickPaintedItem
{
    Q_OBJECT
    Q_ENUMS(Brush)
    Q_PROPERTY(Brush currentBrush MEMBER m_currentBrush NOTIFY currentBrushChanged)
    Q_PROPERTY(bool zoomtoolSelected MEMBER m_zoomSelected NOTIFY zoomtoolSelectedChanged)
    Q_PROPERTY(qreal zoomFactor READ zoomFactor NOTIFY zoomFactorChanged)

public:
    DrawingArea();

    enum Brush {
        Paintbrush,
        Pencil,
        Pen,
        Eraser,
        ZoomTool,
        InvalidBrush = -1
    };


    struct DrawnLine {
        Brush brush = InvalidBrush;
        QList<PenPoint> points;
    };

    void paint(QPainter *painter) override;

public slots:
    void clear();
    void undo();
    void redo();
    void setZoom(double x, double y, double width, double height);
    qreal zoomFactor() { return m_zoomFactor; }

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void currentBrushChanged();
    void zoomtoolSelectedChanged();
    void zoomFactorChanged();

private:
    void redrawBackbuffer();
    void doZoom();
#ifdef Q_PROCESSOR_ARM
    void sendUpdate(QRect rect, const EPFrameBuffer::Waveform waveform);
#endif

    bool m_invert;
    Brush m_currentBrush;
    QImage m_contents;
    bool m_hasEdited;
    QList<DrawnLine> m_lines;
    QList<DrawnLine> m_undoneLines;
    double m_zoomFactor;
    QRectF m_zoomRect;
    bool m_zoomSelected;
};

#endif // DRAWINGAREA_H
