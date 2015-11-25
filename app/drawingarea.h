#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QQuickPaintedItem>
#include <QImage>
#include "digitizer.h"


class DrawingArea : public QQuickPaintedItem
{
    Q_OBJECT
    Q_ENUMS(Brush)
    Q_PROPERTY(Brush currentBrush READ currentBrush WRITE setCurrentBrush NOTIFY currentBrushChanged)
public:
    DrawingArea();

    enum Brush {
        Paintbrush,
        Pencil,
        Pen,
        InvalidBrush = -1
    };


    struct DrawnLine {
        Brush brush = InvalidBrush;
        QList<PenPoint> points;
    };

    Brush currentBrush() const { return m_currentBrush; }
    void setCurrentBrush(Brush brush) { m_currentBrush = brush; emit currentBrushChanged(); }

    void paint(QPainter *painter) override;

public slots:
    void clear();
    void undo();
    void redo();
    void setZoom(double x, double y, double width, double height);

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void currentBrushChanged();

private:
    void redrawBackbuffer();

    bool m_invert;
    Brush m_currentBrush;
    QImage m_contents;
    bool m_hasEdited;
    QList<DrawnLine> m_lines;
    QList<DrawnLine> m_undoneLines;
    double m_zoomFactor;
    QRectF m_zoomRect;
};

#endif // DRAWINGAREA_H
