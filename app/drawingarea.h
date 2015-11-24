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
        Pen
    };
    Brush currentBrush() const { return m_currentBrush; }
    void setCurrentBrush(Brush brush) { m_currentBrush = brush; emit currentBrushChanged(); }

    void paint(QPainter *painter) override;

public slots:
    void clear();

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void currentBrushChanged();

private:
    bool m_invert;
    QVector<QVector<PenPoint>> m_lines;
    Brush m_currentBrush;
    QImage m_contents;
    bool m_hasEdited;
};

#endif // DRAWINGAREA_H
