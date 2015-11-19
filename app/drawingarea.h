#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QQuickPaintedItem>
#include <QImage>

class DrawingArea : public QQuickPaintedItem
{
    Q_OBJECT
public:
    DrawingArea();

    static bool initFb(const char *dev);
    static void closeFb();

    void paint(QPainter *painter) override;

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:

public slots:

private:
    bool m_invert;
};

#endif // DRAWINGAREA_H
